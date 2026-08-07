#include <unity.h>

#include "communication/ble/ble_service.h"
#include "communication/ble/ble_protocol.h"
#include "communication/ble/ble_notifications.h"
#include "communication/ble/ble_commands.h"

#include "core/battery/battery_pack.h"
#include "core/fault/fault_reason.h"

// ============================================================================
// Testes unitários — BLE
// ============================================================================

void setUp(void) {}
void tearDown(void) {}

// Testa que os 6 serviços são criados
void test_ble_services_created(void)
{
    BLEServiceManager mgr;
    mgr.init();

    BLEService* battery = mgr.getService(BLEServiceId::BATTERY);
    TEST_ASSERT_NOT_NULL(battery);
    TEST_ASSERT_EQUAL_UINT8(2, battery->characteristicCount);

    TEST_ASSERT_NOT_NULL(mgr.getService(BLEServiceId::FAULT));
    TEST_ASSERT_NOT_NULL(mgr.getService(BLEServiceId::TEMPERATURE));
    TEST_ASSERT_NOT_NULL(mgr.getService(BLEServiceId::CURRENT));
    TEST_ASSERT_NOT_NULL(mgr.getService(BLEServiceId::CONFIG));
    TEST_ASSERT_NOT_NULL(mgr.getService(BLEServiceId::LOGS));
}

// Testa UUIDs dos serviços
void test_ble_service_uuids(void)
{
    BLEServiceManager mgr;
    mgr.init();

    TEST_ASSERT_EQUAL_UINT16(0xB000, mgr.getService(BLEServiceId::BATTERY)->uuid);
    TEST_ASSERT_EQUAL_UINT16(0xB001, mgr.getService(BLEServiceId::FAULT)->uuid);
    TEST_ASSERT_EQUAL_UINT16(0xB002, mgr.getService(BLEServiceId::TEMPERATURE)->uuid);
    TEST_ASSERT_EQUAL_UINT16(0xB003, mgr.getService(BLEServiceId::CURRENT)->uuid);
    TEST_ASSERT_EQUAL_UINT16(0xB004, mgr.getService(BLEServiceId::CONFIG)->uuid);
    TEST_ASSERT_EQUAL_UINT16(0xB005, mgr.getService(BLEServiceId::LOGS)->uuid);
}

// Testa setCharacteristicData grava payload
void test_ble_set_characteristic_data(void)
{
    BLEServiceManager mgr;
    mgr.init();

    const std::uint8_t payload[] = {0x01, 0x02, 0x03, 0x04};
    bool ok = mgr.setCharacteristicData(BLEServiceId::BATTERY, 0, payload, 4);

    TEST_ASSERT_TRUE(ok);

    BLECharacteristic* ch = mgr.getCharacteristic(BLEServiceId::BATTERY, 0);
    TEST_ASSERT_NOT_NULL(ch);
    TEST_ASSERT_EQUAL_UINT8(4, ch->length);
    TEST_ASSERT_EQUAL_UINT8(0x01, ch->data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x04, ch->data[3]);
}

// Testa setCharacteristicData com índice inválido
void test_ble_set_characteristic_invalid_index(void)
{
    BLEServiceManager mgr;
    mgr.init();

    const std::uint8_t payload[] = {0x01};
    bool ok = mgr.setCharacteristicData(BLEServiceId::BATTERY, 99, payload, 1);

    TEST_ASSERT_FALSE(ok);
}

// Testa getCharacteristic com serviço inválido
void test_ble_get_characteristic_invalid_service(void)
{
    BLEServiceManager mgr;
    mgr.init();

    BLECharacteristic* ch = mgr.getCharacteristic(static_cast<BLEServiceId>(99), 0);
    TEST_ASSERT_EQUAL_PTR(nullptr, ch);
}

// Testa encodeBattery serializa corretamente
void test_ble_encode_battery_length(void)
{
    BatteryPack pack;
    pack.totalVoltage = 12.6f;
    pack.current = 1.5f;
    pack.soc = 80;
    pack.soh = 95;
    pack.valid = true;
    pack.charging = true;
    pack.discharging = false;
    pack.balancing = false;

    std::uint8_t out[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeBattery(pack, out);

    // Layout: voltage(2) + current(2) + soc(1) + soh(1) + 4 flags = 10 bytes
    TEST_ASSERT_EQUAL_UINT8(10, n);

    // SOC no byte 4
    TEST_ASSERT_EQUAL_UINT8(80, out[4]);
    // SOH no byte 5
    TEST_ASSERT_EQUAL_UINT8(95, out[5]);
    // valid, charging, discharging, balancing
    TEST_ASSERT_EQUAL_UINT8(1, out[6]);
    TEST_ASSERT_EQUAL_UINT8(1, out[7]);
    TEST_ASSERT_EQUAL_UINT8(0, out[8]);
    TEST_ASSERT_EQUAL_UINT8(0, out[9]);
}

// Testa packFloat com escala
void test_ble_pack_float(void)
{
    // 12.6 V * 100 = 1260
    TEST_ASSERT_EQUAL_UINT16(1260, BLEProtocol::packFloat(12.6f, 100.0f));
    // 25.5 °C * 10 = 255
    TEST_ASSERT_EQUAL_UINT16(255, BLEProtocol::packFloat(25.5f, 10.0f));
}

// Testa encodeState
void test_ble_encode_state(void)
{
    std::uint8_t out[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeState(3, out);

    TEST_ASSERT_EQUAL_UINT8(1, n);
    TEST_ASSERT_EQUAL_UINT8(3, out[0]);
}

// Testa encodeTemperature
void test_ble_encode_temperature_length(void)
{
    std::uint8_t out[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeTemperature(25.0f, 26.0f, 24.0f, out);

    // 3 x int16 = 6 bytes
    TEST_ASSERT_EQUAL_UINT8(6, n);
}

// Testa encodeFault
void test_ble_encode_fault(void)
{
    FaultInfo fi;
    fi.active = true;
    fi.reason = FaultReason::CELL_OVERVOLTAGE;
    fi.code = 0x0101;
    fi.source = 1;
    fi.value = 4.31f;
    fi.limit = 4.25f;
    fi.timestamp = 1234;

    std::uint8_t out[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeFault(fi, out);

    TEST_ASSERT_EQUAL_UINT8(13, n);
    TEST_ASSERT_EQUAL_UINT8(1, out[0]);        // active
    TEST_ASSERT_EQUAL_UINT8(1, out[4]);        // source
    TEST_ASSERT_EQUAL_UINT8(0x01, out[2]);     // code low byte
    TEST_ASSERT_EQUAL_UINT8(0x01, out[3]);     // code high byte
}

// Testa encodeConfig
void test_ble_encode_config_length(void)
{
    std::uint8_t out[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeConfig(4.25f, 3.0f, 60.0f, 30.0f, out);

    // 4 x uint16 = 8 bytes
    TEST_ASSERT_EQUAL_UINT8(8, n);
}

// Testa encodeLog
void test_ble_encode_log_length(void)
{
    std::uint8_t out[BLE_PAYLOAD_MAX];
    std::uint8_t n = BLEProtocol::encodeLog(0x01020304, 0x00AA, out);

    // timestamp(4) + code(2) = 6 bytes
    TEST_ASSERT_EQUAL_UINT8(6, n);
    TEST_ASSERT_EQUAL_UINT8(0x04, out[0]); // little-endian primeiro byte
}

// Testa comandos BLE definidos
void test_ble_commands_enum(void)
{
    TEST_ASSERT_TRUE(BLECommand::READ_STATUS != BLECommand::READ_CELLS);
    TEST_ASSERT_TRUE(BLECommand::STOP_BALANCING != BLECommand::START_BALANCING);
    TEST_ASSERT_TRUE(BLECommand::SAVE_CONFIGURATION != BLECommand::REBOOT);
}

// Testa notificações BLE definidas
void test_ble_notifications_enum(void)
{
    TEST_ASSERT_TRUE(BLENotification::PACK_STATUS != BLENotification::CELL_VOLTAGES);
    TEST_ASSERT_TRUE(BLENotification::TEMPERATURES != BLENotification::CURRENT);
    TEST_ASSERT_TRUE(BLENotification::SOC != BLENotification::ALARMS);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_ble_services_created);
    RUN_TEST(test_ble_service_uuids);
    RUN_TEST(test_ble_set_characteristic_data);
    RUN_TEST(test_ble_set_characteristic_invalid_index);
    RUN_TEST(test_ble_get_characteristic_invalid_service);
    RUN_TEST(test_ble_encode_battery_length);
    RUN_TEST(test_ble_pack_float);
    RUN_TEST(test_ble_encode_state);
    RUN_TEST(test_ble_encode_temperature_length);
    RUN_TEST(test_ble_encode_fault);
    RUN_TEST(test_ble_encode_config_length);
    RUN_TEST(test_ble_encode_log_length);
    RUN_TEST(test_ble_commands_enum);
    RUN_TEST(test_ble_notifications_enum);

    return UNITY_END();
}
