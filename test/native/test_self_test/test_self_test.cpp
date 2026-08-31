#include <unity.h>

#include "core/state/self_test.h"

// ============================================================================
// Testes unitários — Self Test (Power-On Self Test)
// ============================================================================
// No ambiente native/mock, todos os sensores retornam valores fixos válidos,
// portanto espera-se que o self test passe (SELF TEST PASSED).

void setUp(void) {}
void tearDown(void) {}

void test_self_test_initial_result(void)
{
    SelfTest st;
    TEST_ASSERT_EQUAL(SelfTestResult::NOT_RUN, st.result());
    TEST_ASSERT_FALSE(st.passed());
}

void test_self_test_run_passes(void)
{
    SelfTest st;
    SelfTestResult r = st.run();

    TEST_ASSERT_EQUAL(SelfTestResult::PASS, r);
    TEST_ASSERT_TRUE(st.passed());
    TEST_ASSERT_EQUAL(SelfTestResult::PASS, st.result());
}

void test_self_test_all_components_ok(void)
{
    SelfTest st;
    st.run();

    TEST_ASSERT_TRUE(st.adcOk());
    TEST_ASSERT_TRUE(st.eepromOk());
    TEST_ASSERT_TRUE(st.flashOk());
    TEST_ASSERT_TRUE(st.nvsOk());
    TEST_ASSERT_TRUE(st.currentOk());
    TEST_ASSERT_TRUE(st.temperatureOk());
    TEST_ASSERT_TRUE(st.mosfetOk());
    TEST_ASSERT_TRUE(st.ramOk());
}

void test_self_test_deterministic(void)
{
    SelfTest st;
    st.run();
    SelfTestResult first = st.result();

    st.run();
    SelfTestResult second = st.result();

    TEST_ASSERT_EQUAL(first, second);
    TEST_ASSERT_EQUAL(SelfTestResult::PASS, second);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_self_test_initial_result);
    RUN_TEST(test_self_test_run_passes);
    RUN_TEST(test_self_test_all_components_ok);
    RUN_TEST(test_self_test_deterministic);

    return UNITY_END();
}
