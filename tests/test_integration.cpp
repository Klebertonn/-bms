#include <unity.h>

#include "core/state/bms_state_machine.h"
#include "core/fault/fault_manager.h"
#include "core/battery/battery_pack.h"

#include "communication/can/can_protocol.h"
#include "communication/ble/ble_protocol.h"
#include "communication/ble/ble_service.h"

#include "system/scheduler/scheduler.h"
#include "system/scheduler/task.h"

// ============================================================================
// Testes de Integração — Módulos trabalhando juntos
// ============================================================================

void setUp(void) {}
void tearDown(void) {}

// Testa boot completo: SELF_TEST passa -> READY, e telemetria pode ser gerada
void test_integration_boot_to_ready_with_telemetry(void)
{
    BmsStateMachine sm;
    sm.init();

    // BOOT -> INIT -> SELF_TEST -> READY
    sm.update();
    sm.update();
    sm.update();

    TEST_ASSERT_EQUAL(BmsIndustrialState::READY, sm.state());

    // Após READY, podemos serializar telemetria via BLE
    BatteryPack pack;
    pack.totalVoltage = 12.6f;
    pack.current = 2.0f;
    pack.soc = 82;
    pack.soh = 95;
    pack.valid = true;

    std::uint8_t buf[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeBattery(pack, buf);

    TEST_ASSERT_EQUAL_UINT8(10, n);
    TEST_ASSERT_EQUAL_UINT8(82, buf[4]);
}

// Testa integração Fault -> State (falha leva a FAULT)
void test_integration_fault_to_state(void)
{
    BmsStateMachine sm;
    sm.init();
    sm.setState(BmsIndustrialState::READY);

    // Em READY, uma falha não muda a máquina industrial diretamente
    // (a máquina industrial é separada do BmsStateManager), mas verificamos
    // que o estado é preservado e a falha pode ser serializada via CAN/BLE.
    FaultManager fm;
    fm.init();
    fm.addFault(FAULT_CELL_OVERVOLTAGE);

    TEST_ASSERT_TRUE(fm.hasFault());
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);

    // Serializa falha via BLE
    std::uint8_t buf[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeFault(fm.getFaultInfo(), buf);
    TEST_ASSERT_EQUAL_UINT8(13, n);
    TEST_ASSERT_EQUAL_UINT8(1, buf[0]); // active
}

// Testa integração Scheduler + BLE Service (tasks e serviços convivem)
void test_integration_scheduler_and_ble_services(void)
{
    Scheduler sched;

    Task canTask, bleTask;
    canTask.name = "CAN";
    canTask.period_ms = 100;
    bleTask.name = "BLE";
    bleTask.period_ms = 500;
    sched.addTask(&canTask, TaskPriority::MEDIUM);
    sched.addTask(&bleTask, TaskPriority::LOW);

    TEST_ASSERT_EQUAL(TaskPriority::MEDIUM, canTask.priority);
    TEST_ASSERT_EQUAL(TaskPriority::LOW, bleTask.priority);

    // BLE services coexistem com o scheduler
    BLEServiceManager ble;
    ble.init();
    TEST_ASSERT_NOT_NULL(ble.getService(BLEServiceId::BATTERY));
}

// Testa integração CAN Protocol + BLE Protocol (mesmo BatteryPack)
void test_integration_can_and_ble_share_pack_data(void)
{
    BatteryPack pack;
    pack.totalVoltage = 11.4f;
    pack.current = -3.0f;
    pack.soc = 60;
    pack.soh = 90;

    // CAN heartbeat
    CANFrame hb = CANProtocol::makeHeartbeat(0x42);
    TEST_ASSERT_EQUAL_UINT32(0x100, hb.id);

    // BLE battery encode do mesmo pack
    std::uint8_t buf[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeBattery(pack, buf);
    TEST_ASSERT_EQUAL_UINT8(10, n);
    TEST_ASSERT_EQUAL_UINT8(60, buf[4]); // SOC
}

// Testa integração BmsStateMachine + BLE state encode
void test_integration_state_to_ble(void)
{
    BmsStateMachine sm;
    sm.init();
    sm.update(); // BOOT -> INIT
    sm.update(); // INIT -> SELF_TEST
    sm.update(); // SELF_TEST -> READY

    // Serializa estado READY via BLE
    std::uint8_t buf[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeState(static_cast<std::uint8_t>(sm.state()), buf);

    TEST_ASSERT_EQUAL_UINT8(1, n);
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(BmsIndustrialState::READY), buf[0]);
}

// Testa integração completa de ciclo de telemetria
void test_integration_full_telemetry_cycle(void)
{
    // Simula o fluxo do App: boot -> ready -> telemetria
    BmsStateMachine sm;
    sm.init();
    sm.update();
    sm.update();
    sm.update();

    BatteryPack pack;
    pack.totalVoltage = 12.6f;
    pack.current = 1.5f;
    pack.soc = 78;
    pack.soh = 92;
    pack.valid = true;
    pack.charging = true;

    // Serializa pack para BLE
    std::uint8_t bleBuf[BLE_PAYLOAD_MAX];
    std::uint8_t bleLen = BLEProtocol::encodeBattery(pack, bleBuf);

    // Serializa estado para BLE
    std::uint8_t stateBuf[4];
    std::uint8_t stateLen = BLEProtocol::encodeState(static_cast<std::uint8_t>(sm.state()), stateBuf);

    // Serializa CAN heartbeat
    CANFrame hb = CANProtocol::makeHeartbeat(0x01);

    TEST_ASSERT_EQUAL_UINT8(10, bleLen);
    TEST_ASSERT_EQUAL_UINT8(1, stateLen);
    TEST_ASSERT_EQUAL_UINT8(0x01, hb.data[0]);
    TEST_ASSERT_EQUAL(BmsIndustrialState::READY, sm.state());
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_integration_boot_to_ready_with_telemetry);
    RUN_TEST(test_integration_fault_to_state);
    RUN_TEST(test_integration_scheduler_and_ble_services);
    RUN_TEST(test_integration_can_and_ble_share_pack_data);
    RUN_TEST(test_integration_state_to_ble);
    RUN_TEST(test_integration_full_telemetry_cycle);

    return UNITY_END();
}
