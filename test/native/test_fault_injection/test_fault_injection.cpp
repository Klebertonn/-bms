#include <unity.h>

#include "config/config.h"
#include "core/fault/fault_manager.h"
#include "core/protection/protection_manager.h"

namespace
{
BatteryPack normalPack()
{
    BatteryPack pack;
    pack.valid = true;
    for (std::uint8_t index = 0; index < PACK_CELL_COUNT; ++index)
    {
        pack.cells[index].valid = true;
        pack.cells[index].voltage = 3.70f;
    }
    pack.minVoltage = 3.70f;
    pack.maxVoltage = 3.70f;
    pack.minTemperature = 25.0f;
    pack.maxTemperature = 25.0f;
    pack.current = 0.0f;
    return pack;
}

void assertFaultAndRecovery(BatteryPack pack, FaultCode code)
{
    FaultManager faults;
    ProtectionManager protection;
    faults.init();
    protection.init();

    protection.update(pack, faults);
    TEST_ASSERT_TRUE(faults.hasFault(code));
    TEST_ASSERT_TRUE(protection.isFault());

    protection.update(normalPack(), faults);
    TEST_ASSERT_FALSE(faults.hasFault(code));
    TEST_ASSERT_FALSE(protection.isFault());
}
}

void setUp(void) {}
void tearDown(void) {}

void test_inject_overvoltage(void)
{
    BatteryPack pack = normalPack();
    pack.cells[1].voltage = 4.30f;
    pack.maxVoltage = 4.30f;
    assertFaultAndRecovery(pack, FaultCode::BMS_CELL_OVERVOLTAGE);
}

void test_inject_undervoltage(void)
{
    BatteryPack pack = normalPack();
    pack.cells[0].voltage = 2.90f;
    pack.minVoltage = 2.90f;
    assertFaultAndRecovery(pack, FaultCode::BMS_CELL_UNDERVOLTAGE);
}

void test_inject_overtemperature(void)
{
    BatteryPack pack = normalPack();
    pack.maxTemperature = 61.0f;
    assertFaultAndRecovery(pack, FaultCode::BMS_OVER_TEMPERATURE);
}

void test_inject_overcurrent(void)
{
    BatteryPack pack = normalPack();
    pack.current = MAX_CHARGE_CURRENT + 1.0f;
    assertFaultAndRecovery(pack, FaultCode::BMS_OVER_CURRENT);
}

void test_inject_short_circuit(void)
{
    BatteryPack pack = normalPack();
    pack.current = MAX_CHARGE_CURRENT * 10.0f + 1.0f;
    assertFaultAndRecovery(pack, FaultCode::BMS_SHORT_CIRCUIT);
}

void test_inject_overvoltage_disables_charge_only(void)
{
    BatteryPack pack = normalPack();
    pack.maxVoltage = 4.30f;
    pack.cells[0].voltage = 4.30f;

    FaultManager faults;
    ProtectionManager protection;
    faults.init();
    protection.init();
    protection.update(pack, faults);

    TEST_ASSERT_FALSE(protection.chargeEnabled());
    TEST_ASSERT_TRUE(protection.dischargeEnabled());
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();
    RUN_TEST(test_inject_overvoltage);
    RUN_TEST(test_inject_undervoltage);
    RUN_TEST(test_inject_overtemperature);
    RUN_TEST(test_inject_overcurrent);
    RUN_TEST(test_inject_short_circuit);
    RUN_TEST(test_inject_overvoltage_disables_charge_only);
    return UNITY_END();
}
