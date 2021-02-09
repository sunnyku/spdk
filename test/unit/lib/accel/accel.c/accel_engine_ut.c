/*-
 *   BSD LICENSE
 *
 *   Copyright (c) Intel Corporation.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "spdk_cunit.h"
#include "spdk_internal/mock.h"
#include "common/lib/test_env.c"

#include "accel/accel_engine.c"

DEFINE_STUB(spdk_json_write_array_begin, int, (struct spdk_json_write_ctx *w), 0);
DEFINE_STUB(spdk_json_write_array_end, int, (struct spdk_json_write_ctx *w), 0);

static void
test_spdk_accel_hw_engine_register(void)
{
	struct spdk_accel_engine accel_engine;

	/* Run once with no engine assigned, assign it. */
	g_hw_accel_engine = NULL;
	spdk_accel_hw_engine_register(&accel_engine);
	CU_ASSERT(g_hw_accel_engine == &accel_engine);

	/* Run with one assigned, should not change. */
	spdk_accel_hw_engine_register(&accel_engine);
	CU_ASSERT(g_hw_accel_engine == &accel_engine);
}

static int
test_accel_sw_register(void)
{
	struct spdk_accel_engine accel_engine;

	/* Run once with no engine assigned, assign it. */
	g_sw_accel_engine = NULL;
	accel_sw_register(&accel_engine);
	CU_ASSERT(g_sw_accel_engine == &accel_engine);

	return 0;
}

static void
test_accel_sw_unregister(void)
{
	struct spdk_accel_engine accel_engine;

	/* Run once engine assigned, make sure it gets unassigned. */
	g_sw_accel_engine = &accel_engine;
	accel_sw_unregister();
	CU_ASSERT(g_sw_accel_engine == NULL);
}

static void
test_is_supported(void)
{
	struct spdk_accel_engine engine;

	engine.capabilities = ACCEL_COPY | ACCEL_DUALCAST | ACCEL_CRC32C;
	CU_ASSERT(_is_supported(&engine, ACCEL_COPY) == true);
	CU_ASSERT(_is_supported(&engine, ACCEL_FILL) == false);
	CU_ASSERT(_is_supported(&engine, ACCEL_DUALCAST) == true);
	CU_ASSERT(_is_supported(&engine, ACCEL_COMPARE) == false);
	CU_ASSERT(_is_supported(&engine, ACCEL_CRC32C) == true);
	CU_ASSERT(_is_supported(&engine, ACCEL_DIF) == false);
}

int main(int argc, char **argv)
{
	CU_pSuite	suite = NULL;
	unsigned int	num_failures;

	CU_set_error_action(CUEA_ABORT);
	CU_initialize_registry();

	suite = CU_add_suite("accel", NULL, NULL);

	CU_ADD_TEST(suite, test_spdk_accel_hw_engine_register);
	CU_ADD_TEST(suite, test_accel_sw_register);
	CU_ADD_TEST(suite, test_accel_sw_unregister);
	CU_ADD_TEST(suite, test_is_supported);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	num_failures = CU_get_number_of_failures();
	CU_cleanup_registry();

	return num_failures;
}
