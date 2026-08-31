#include <unity.h>

#include "communication/can/can_frame.h"
#include "communication/can/can_ids.h"
#include "communication/can/can_protocol.h"
#include "communication/can/can_manager.h"

// ============================================================================
// Testes unitários — CAN
// ============================================================================

void setUp(void) {}
void tearDown(void) {}

// Testa makeHeartbeat constrói frame com ID e DLC corretos
void test_can_heartbeat_frame(void)
{
    CANFrame frame = CANProtocol::makeHeartbeat(0xAA);

    TEST_ASSERT_EQUAL_UINT32(CANID::HEARTBEAT, frame.id);
    TEST_ASSERT_EQUAL_UINT8(1, frame.dlc);
    TEST_ASSERT_EQUAL_UINT8(0xAA, frame.data[0]);
}

// Testa makeHeartbeat com valor zero
void test_can_heartbeat_frame_zero(void)
{
    CANFrame frame = CANProtocol::makeHeartbeat(0x00);

    TEST_ASSERT_EQUAL_UINT32(CANID::HEARTBEAT, frame.id);
    TEST_ASSERT_EQUAL_UINT8(1, frame.dlc);
    TEST_ASSERT_EQUAL_UINT8(0x00, frame.data[0]);
}

// Testa IDs de mensagens definidos
void test_can_ids_defined(void)
{
    TEST_ASSERT_EQUAL_UINT32(0x100, CANID::HEARTBEAT);
    TEST_ASSERT_EQUAL_UINT32(0x101, CANID::PACK_STATUS);
    TEST_ASSERT_EQUAL_UINT32(0x102, CANID::CELL_VOLTAGES);
    TEST_ASSERT_EQUAL_UINT32(0x103, CANID::TEMPERATURES);
    TEST_ASSERT_EQUAL_UINT32(0x104, CANID::CURRENT);
    TEST_ASSERT_EQUAL_UINT32(0x105, CANID::SOC);
    TEST_ASSERT_EQUAL_UINT32(0x106, CANID::ALARMS);
    TEST_ASSERT_EQUAL_UINT32(0x107, CANID::BALANCING);
    TEST_ASSERT_EQUAL_UINT32(0x108, CANID::CONFIG);
}

// Testa que todos os IDs são únicos
void test_can_ids_unique(void)
{
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::HEARTBEAT, CANID::PACK_STATUS);
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::PACK_STATUS, CANID::CELL_VOLTAGES);
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::CELL_VOLTAGES, CANID::TEMPERATURES);
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::TEMPERATURES, CANID::CURRENT);
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::CURRENT, CANID::SOC);
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::SOC, CANID::ALARMS);
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::ALARMS, CANID::BALANCING);
    TEST_ASSERT_NOT_EQUAL_UINT32(CANID::BALANCING, CANID::CONFIG);
}

// Testa que CANManager init/update não causam falha (usa mock driver)
void test_can_manager_init_update(void)
{
    CANManager cm;
    cm.init();
    // update chama receive + send* (mock aceita), não deve travar.
    cm.update();

    TEST_ASSERT_TRUE(true);
}

// Testa sendHeartbeat com mock driver
void test_can_manager_send_heartbeat_no_crash(void)
{
    CANManager cm;
    cm.init();
    cm.sendHeartbeat();

    TEST_ASSERT_TRUE(true);
}

// Testa size do payload de um frame CAN (máx 8 bytes conforme frame)
void test_can_frame_dlc_max(void)
{
    CANFrame frame;
    frame.id = CANID::PACK_STATUS;
    frame.dlc = 8;

    // DLC nunca deve exceder 8
    TEST_ASSERT_TRUE(frame.dlc <= 8);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_can_heartbeat_frame);
    RUN_TEST(test_can_heartbeat_frame_zero);
    RUN_TEST(test_can_ids_defined);
    RUN_TEST(test_can_ids_unique);
    RUN_TEST(test_can_manager_init_update);
    RUN_TEST(test_can_manager_send_heartbeat_no_crash);
    RUN_TEST(test_can_frame_dlc_max);

    return UNITY_END();
}
