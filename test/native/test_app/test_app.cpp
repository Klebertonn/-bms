#include <unity.h>

#include "app/app.h"

void setUp(void) {}
void tearDown(void) {}

void test_app_initializes_and_updates(void)
{
    App app;

    TEST_ASSERT_TRUE(app.init());

    for (int cycle = 0; cycle < 10; ++cycle)
    {
        app.update();
    }

    const AppDiagnostics diagnostics = app.diagnostics();
    TEST_ASSERT_EQUAL_UINT64(1, diagnostics.faultsRaised);
    TEST_ASSERT_EQUAL_UINT64(0, diagnostics.faultsCleared);
    TEST_ASSERT_EQUAL_STRING("FAULT", diagnostics.state);
    TEST_ASSERT_TRUE(diagnostics.watchdogHealthy);
    TEST_ASSERT_EQUAL_UINT32(0, diagnostics.watchdogMisses);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();
    RUN_TEST(test_app_initializes_and_updates);
    return UNITY_END();
}
