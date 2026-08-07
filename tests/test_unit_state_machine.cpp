#include <unity.h>

#include "core/state/bms_state_machine.h"

// ============================================================================
// Testes unitários — BmsStateMachine
// ============================================================================

void setUp(void) {}
void tearDown(void) {}

// Testa init() começa em BOOT
void test_state_machine_init_starts_boot(void)
{
    BmsStateMachine sm;
    sm.init();

    TEST_ASSERT_EQUAL(BmsIndustrialState::BOOT, sm.state());
    TEST_ASSERT_EQUAL(BmsIndustrialState::BOOT, sm.previousState());
}

// Testa a sequência BOOT -> INIT -> SELF_TEST -> READY (quando self test passa)
void test_state_machine_boot_to_ready(void)
{
    BmsStateMachine sm;
    sm.init();

    // BOOT -> INIT
    sm.update();
    TEST_ASSERT_EQUAL(BmsIndustrialState::INIT, sm.state());

    // INIT -> SELF_TEST
    sm.update();
    TEST_ASSERT_EQUAL(BmsIndustrialState::SELF_TEST, sm.state());

    // SELF_TEST -> READY (self test passa no ambiente mock)
    sm.update();
    TEST_ASSERT_EQUAL(BmsIndustrialState::READY, sm.state());
}

// Testa transição manual via setState
void test_state_machine_set_state(void)
{
    BmsStateMachine sm;
    sm.init();

    sm.setState(BmsIndustrialState::CHARGING);
    TEST_ASSERT_EQUAL(BmsIndustrialState::CHARGING, sm.state());
    TEST_ASSERT_EQUAL(BmsIndustrialState::BOOT, sm.previousState());
}

// Testa toString para todos os estados
void test_state_machine_to_string(void)
{
    BmsStateMachine sm;
    sm.init();

    TEST_ASSERT_EQUAL_STRING("BOOT", sm.toString());

    sm.setState(BmsIndustrialState::INIT);
    TEST_ASSERT_EQUAL_STRING("INIT", sm.toString());

    sm.setState(BmsIndustrialState::SELF_TEST);
    TEST_ASSERT_EQUAL_STRING("SELF_TEST", sm.toString());

    sm.setState(BmsIndustrialState::READY);
    TEST_ASSERT_EQUAL_STRING("READY", sm.toString());

    sm.setState(BmsIndustrialState::IDLE);
    TEST_ASSERT_EQUAL_STRING("IDLE", sm.toString());

    sm.setState(BmsIndustrialState::CHARGING);
    TEST_ASSERT_EQUAL_STRING("CHARGING", sm.toString());

    sm.setState(BmsIndustrialState::DISCHARGING);
    TEST_ASSERT_EQUAL_STRING("DISCHARGING", sm.toString());

    sm.setState(BmsIndustrialState::BALANCING);
    TEST_ASSERT_EQUAL_STRING("BALANCING", sm.toString());

    sm.setState(BmsIndustrialState::FAULT);
    TEST_ASSERT_EQUAL_STRING("FAULT", sm.toString());

    sm.setState(BmsIndustrialState::SHUTDOWN);
    TEST_ASSERT_EQUAL_STRING("SHUTDOWN", sm.toString());
}

// Testa que transição para o mesmo estado não muda (idempotente)
void test_state_machine_no_transition_on_same_state(void)
{
    BmsStateMachine sm;
    sm.init();

    sm.setState(BmsIndustrialState::READY);
    const BmsIndustrialState prev = sm.previousState();

    // Transição para o mesmo estado deve ser ignorada
    sm.setState(BmsIndustrialState::READY);
    TEST_ASSERT_EQUAL(BmsIndustrialState::READY, sm.state());
    TEST_ASSERT_EQUAL(prev, sm.previousState());
}

// Testa que READY se mantém estável (estado absorvente no update)
void test_state_machine_ready_is_stable(void)
{
    BmsStateMachine sm;
    sm.init();

    // Avança até READY
    sm.update();
    sm.update();
    sm.update();
    TEST_ASSERT_EQUAL(BmsIndustrialState::READY, sm.state());

    // update adicional não muda
    sm.update();
    TEST_ASSERT_EQUAL(BmsIndustrialState::READY, sm.state());
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_state_machine_init_starts_boot);
    RUN_TEST(test_state_machine_boot_to_ready);
    RUN_TEST(test_state_machine_set_state);
    RUN_TEST(test_state_machine_to_string);
    RUN_TEST(test_state_machine_no_transition_on_same_state);
    RUN_TEST(test_state_machine_ready_is_stable);

    return UNITY_END();
}
