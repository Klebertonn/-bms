#include <unity.h>

#include "core/fault/fault_manager.h"
#include "core/battery/battery_pack.h"

// ============================================================================
// Testes unitários — FaultManager
// ============================================================================

void setUp(void) {}
void tearDown(void) {}

// Testa init() -> sem falhas
void test_fault_init_has_no_faults(void)
{
    FaultManager fm;
    fm.init();

    TEST_ASSERT_FALSE(fm.hasFault());
    TEST_ASSERT_EQUAL_UINT32(FAULT_NONE, fm.getFaults());
    TEST_ASSERT_FALSE(fm.getFaultInfo().active);
}

// Testa addFault individual
void test_fault_add_fault(void)
{
    FaultManager fm;
    fm.init();

    fm.addFault(FAULT_CELL_OVERVOLTAGE);

    TEST_ASSERT_TRUE(fm.hasFault());
    TEST_ASSERT_TRUE(fm.hasFault(FAULT_CELL_OVERVOLTAGE));
    TEST_ASSERT_FALSE(fm.hasFault(FAULT_CELL_UNDERVOLTAGE));
}

// Testa múltiplas falhas combinadas
void test_fault_add_multiple(void)
{
    FaultManager fm;
    fm.init();

    fm.addFault(FAULT_CELL_OVERVOLTAGE);
    fm.addFault(FAULT_OVER_TEMPERATURE);

    TEST_ASSERT_TRUE(fm.hasFault(FAULT_CELL_OVERVOLTAGE));
    TEST_ASSERT_TRUE(fm.hasFault(FAULT_OVER_TEMPERATURE));
    TEST_ASSERT_EQUAL_UINT32(
        static_cast<uint32_t>(FAULT_CELL_OVERVOLTAGE) |
        static_cast<uint32_t>(FAULT_OVER_TEMPERATURE),
        fm.getFaults());
}

// Testa clear()
void test_fault_clear(void)
{
    FaultManager fm;
    fm.init();

    fm.addFault(FAULT_CELL_OVERVOLTAGE);
    fm.addFault(FAULT_OVER_CURRENT);
    TEST_ASSERT_TRUE(fm.hasFault());

    fm.clear();

    TEST_ASSERT_FALSE(fm.hasFault());
    TEST_ASSERT_EQUAL_UINT32(FAULT_NONE, fm.getFaults());
}

// Helper: cria um BatteryPack com tensões customizadas
static BatteryPack makePack(const float* voltages, float current, float avgTemp)
{
    BatteryPack pack;

    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        pack.cells[i].voltage = voltages[i];
        pack.cells[i].valid = true;
    }

    pack.current = current;
    pack.averageTemperature = avgTemp;

    pack.maxVoltage = voltages[0];
    pack.minVoltage = voltages[0];
    for (std::uint8_t i = 1; i < PACK_CELL_COUNT; ++i)
    {
        if (voltages[i] > pack.maxVoltage) pack.maxVoltage = voltages[i];
        if (voltages[i] < pack.minVoltage) pack.minVoltage = voltages[i];
    }

    return pack;
}

// Testa sobre-tensão de célula
void test_fault_evaluate_overvoltage(void)
{
    FaultManager fm;
    fm.init();

    // Célula 0 em 4.30V > 4.25V (limite)
    const float v[PACK_CELL_COUNT] = {4.30f, 4.18f, 4.19f};
    BatteryPack pack = makePack(v, 0.0f, 25.0f);

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault());
    TEST_ASSERT_TRUE(fm.hasFault(FAULT_CELL_OVERVOLTAGE));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::CELL_OVERVOLTAGE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0101, fm.getFaultInfo().code);

    // Célula 1 (0-based) == 1-based source 1
    TEST_ASSERT_EQUAL_UINT8(1, fm.getFaultInfo().source);
}

// Testa sub-tensão de célula
void test_fault_evaluate_undervoltage(void)
{
    FaultManager fm;
    fm.init();

    // Célula 1 em 2.95V < 3.00V (limite)
    const float v[PACK_CELL_COUNT] = {3.20f, 2.95f, 3.10f};
    BatteryPack pack = makePack(v, 0.0f, 25.0f);

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault(FAULT_CELL_UNDERVOLTAGE));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::CELL_UNDERVOLTAGE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0102, fm.getFaultInfo().code);
    TEST_ASSERT_EQUAL_UINT8(2, fm.getFaultInfo().source); // célula 1 (0-based) -> 1-based 2
}

// Testa sobre-temperatura
void test_fault_evaluate_overtemperature(void)
{
    FaultManager fm;
    fm.init();

    const float v[PACK_CELL_COUNT] = {3.70f, 3.70f, 3.70f};
    BatteryPack pack = makePack(v, 0.0f, 65.0f); // > 60°C

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault(FAULT_OVER_TEMPERATURE));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::OVERTEMPERATURE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0201, fm.getFaultInfo().code);
    TEST_ASSERT_EQUAL_UINT8(0xFF, fm.getFaultInfo().source);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 65.0f, fm.getFaultInfo().value);
}

// Testa sobre-corrente
void test_fault_evaluate_overcurrent(void)
{
    FaultManager fm;
    fm.init();

    const float v[PACK_CELL_COUNT] = {3.70f, 3.70f, 3.70f};
    BatteryPack pack = makePack(v, 35.0f, 25.0f); // > 30A

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault(FAULT_OVER_CURRENT));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::OVERCURRENT_CHARGE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0301, fm.getFaultInfo().code);
}

// Testa pack normal -> sem falhas
void test_fault_evaluate_normal_pack(void)
{
    FaultManager fm;
    fm.init();

    const float v[PACK_CELL_COUNT] = {3.70f, 3.72f, 3.71f};
    BatteryPack pack = makePack(v, 2.0f, 25.0f);

    fm.evaluate(pack);

    TEST_ASSERT_FALSE(fm.hasFault());
    TEST_ASSERT_FALSE(fm.getFaultInfo().active);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_fault_init_has_no_faults);
    RUN_TEST(test_fault_add_fault);
    RUN_TEST(test_fault_add_multiple);
    RUN_TEST(test_fault_clear);
    RUN_TEST(test_fault_evaluate_overvoltage);
    RUN_TEST(test_fault_evaluate_undervoltage);
    RUN_TEST(test_fault_evaluate_overtemperature);
    RUN_TEST(test_fault_evaluate_overcurrent);
    RUN_TEST(test_fault_evaluate_normal_pack);

    return UNITY_END();
}
