#include <unity.h>

#include "system/scheduler/scheduler.h"
#include "system/scheduler/task.h"
#include "system/scheduler/scheduler_tasks.h"
#include "system/rtos/task_priorities.h"

// ============================================================================
// Testes unitários — Scheduler
// ============================================================================

// Contadores globais para verificar ordem de execução
static int g_callOrder[8];
static int g_callCount = 0;

void setUp(void)
{
    g_callCount = 0;
    for (int i = 0; i < 8; ++i)
    {
        g_callOrder[i] = -1;
    }
}

void tearDown(void) {}

// Callbacks das tasks
static void cbCurrent()      { g_callOrder[g_callCount++] = 0; }
static void cbTemperature()  { g_callOrder[g_callCount++] = 1; }
static void cbProtection()   { g_callOrder[g_callCount++] = 2; }
static void cbLogger()       { g_callOrder[g_callCount++] = 3; }
static void cbCAN()          { g_callOrder[g_callCount++] = 4; }
static void cbBLE()          { g_callOrder[g_callCount++] = 5; }

// Testa adição de task com prioridade padrão
void test_scheduler_add_task_default_priority(void)
{
    Scheduler sched;
    Task t;
    t.name = "T";
    t.period_ms = 0;
    t.callback = cbCurrent;

    sched.addTask(&t);

    // A prioridade padrão deve ser MEDIUM
    TEST_ASSERT_EQUAL(TaskPriority::MEDIUM, t.priority);
}

// Testa adição de task com prioridade explícita
void test_scheduler_add_task_with_priority(void)
{
    Scheduler sched;
    Task t;
    t.name = "T";
    t.period_ms = 0;
    t.callback = cbCurrent;

    sched.addTask(&t, TaskPriority::VERY_HIGH);

    TEST_ASSERT_EQUAL(TaskPriority::VERY_HIGH, t.priority);
}

// Testa execução em ordem de prioridade (mais alta primeiro)
void test_scheduler_priority_order(void)
{
    Scheduler sched;

    Task low, med, high, veryHigh;

    low.name = "LOW";
    low.period_ms = 0;
    low.callback = cbBLE;          // guarda como 5
    sched.addTask(&low, TaskPriority::LOW);

    med.name = "MED";
    med.period_ms = 0;
    med.callback = cbLogger;       // guarda como 3
    sched.addTask(&med, TaskPriority::MEDIUM);

    high.name = "HIGH";
    high.period_ms = 0;
    high.callback = cbCurrent;     // guarda como 0
    sched.addTask(&high, TaskPriority::HIGH);

    veryHigh.name = "VERY_HIGH";
    veryHigh.period_ms = 0;
    veryHigh.callback = cbProtection; // guarda como 2
    sched.addTask(&veryHigh, TaskPriority::VERY_HIGH);

    sched.run();

    TEST_ASSERT_EQUAL_INT(4, g_callCount);
    // VERY_HIGH primeiro (cbProtection -> 2)
    TEST_ASSERT_EQUAL_INT(2, g_callOrder[0]);
    // Depois HIGH (cbCurrent -> 0)
    TEST_ASSERT_EQUAL_INT(0, g_callOrder[1]);
    // Depois MEDIUM (cbLogger -> 3)
    TEST_ASSERT_EQUAL_INT(3, g_callOrder[2]);
    // Por último LOW (cbBLE -> 5)
    TEST_ASSERT_EQUAL_INT(5, g_callOrder[3]);
}

// Testa que uma task desabilitada não executa
void test_scheduler_disabled_task_not_executed(void)
{
    Scheduler sched;

    Task t1, t2;
    t1.name = "ENABLED";
    t1.period_ms = 0;
    t1.callback = cbCurrent;
    t2.name = "DISABLED";
    t2.period_ms = 0;
    t2.callback = cbTemperature;

    sched.addTask(&t1);
    sched.addTask(&t2);
    sched.setTaskEnabled(&t2, false);

    sched.run();

    TEST_ASSERT_EQUAL_INT(1, g_callCount);
    TEST_ASSERT_EQUAL_INT(0, g_callOrder[0]); // apenas cbCurrent rodou
}

// Testa que task com período não executa antes do tempo
void test_scheduler_task_period_gating(void)
{
    // Neste teste, usamos período alto para garantir que não executa
    // imediatamente se o tempo decorrido < período.
    Scheduler sched;

    Task t;
    t.name = "PERIODIC";
    t.period_ms = 1000000; // 1000s, não deve executar agora
    t.callback = cbCurrent;

    sched.addTask(&t);
    sched.run();

    TEST_ASSERT_EQUAL_INT(0, g_callCount);
}

// Testa prioridades definidas nos perfis organizados
void test_scheduler_profiles_priorities(void)
{
    TEST_ASSERT_EQUAL(TaskPriority::VERY_HIGH, SchedulerProfiles::PROTECTION.priority);
    TEST_ASSERT_EQUAL(TaskPriority::VERY_HIGH, SchedulerProfiles::WATCHDOG.priority);
    TEST_ASSERT_EQUAL(TaskPriority::HIGH,       SchedulerProfiles::CURRENT.priority);
    TEST_ASSERT_EQUAL(TaskPriority::HIGH,       SchedulerProfiles::TEMPERATURE.priority);
    TEST_ASSERT_EQUAL(TaskPriority::MEDIUM,     SchedulerProfiles::LOGGER.priority);
    TEST_ASSERT_EQUAL(TaskPriority::MEDIUM,     SchedulerProfiles::CAN.priority);
    TEST_ASSERT_EQUAL(TaskPriority::LOW,        SchedulerProfiles::BLE.priority);
    TEST_ASSERT_EQUAL(TaskPriority::LOW,        SchedulerProfiles::DASHBOARD.priority);
}

// Testa períodos dos perfis
void test_scheduler_profiles_periods(void)
{
    TEST_ASSERT_EQUAL_UINT32(50u,   SchedulerProfiles::PROTECTION.periodMs);
    TEST_ASSERT_EQUAL_UINT32(100u,  SchedulerProfiles::CURRENT.periodMs);
    TEST_ASSERT_EQUAL_UINT32(100u,  SchedulerProfiles::TEMPERATURE.periodMs);
    TEST_ASSERT_EQUAL_UINT32(1000u, SchedulerProfiles::LOGGER.periodMs);
    TEST_ASSERT_EQUAL_UINT32(100u,  SchedulerProfiles::CAN.periodMs);
    TEST_ASSERT_EQUAL_UINT32(500u,  SchedulerProfiles::BLE.periodMs);
    TEST_ASSERT_EQUAL_UINT32(1000u, SchedulerProfiles::DASHBOARD.periodMs);
    TEST_ASSERT_EQUAL_UINT32(100u,  SchedulerProfiles::WATCHDOG.periodMs);
}

// Testa configureTaskFromProfile preenche os campos
void test_scheduler_configure_task_from_profile(void)
{
    Task t;
    t.name = nullptr;
    t.priority = TaskPriority::LOW;
    t.period_ms = 0;

    configureTaskFromProfile(t, SchedulerProfiles::CAN);

    TEST_ASSERT_EQUAL_STRING("CAN", t.name);
    TEST_ASSERT_EQUAL(TaskPriority::MEDIUM, t.priority);
    TEST_ASSERT_EQUAL_UINT32(100u, t.period_ms);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_scheduler_add_task_default_priority);
    RUN_TEST(test_scheduler_add_task_with_priority);
    RUN_TEST(test_scheduler_priority_order);
    RUN_TEST(test_scheduler_disabled_task_not_executed);
    RUN_TEST(test_scheduler_task_period_gating);
    RUN_TEST(test_scheduler_profiles_priorities);
    RUN_TEST(test_scheduler_profiles_periods);
    RUN_TEST(test_scheduler_configure_task_from_profile);

    return UNITY_END();
}
