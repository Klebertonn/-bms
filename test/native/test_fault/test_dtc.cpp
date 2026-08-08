#include <unity.h>

#include "core/fault/fault_manager.h"
#include "core/fault/fault_registry.h"
#include "core/fault/fault_sink.h"
#include "core/battery/battery_pack.h"
#include "core/protection/protection_manager.h"
#include "core/protection/protection_types.h"

// ============================================================================
// Testes unitários — FaultManager (API legada + API DTC)
// ============================================================================

void setUp(void) {}
void tearDown(void) {}

// ---------------------------------------------------------------------------
// API Legada (compatibilidade)
// ---------------------------------------------------------------------------

void test_fault_init_has_no_faults(void)
{
    FaultManager fm;
    fm.init();

    TEST_ASSERT_FALSE(fm.hasFault());
    TEST_ASSERT_EQUAL_UINT32(FAULT_NONE, fm.getFaults());
    TEST_ASSERT_FALSE(fm.getFaultInfo().active);
}

void test_fault_add_fault(void)
{
    FaultManager fm;
    fm.init();

    fm.addFault(FAULT_CELL_OVERVOLTAGE);

    TEST_ASSERT_TRUE(fm.hasFault());
    TEST_ASSERT_TRUE(fm.hasFault(FAULT_CELL_OVERVOLTAGE));
    TEST_ASSERT_FALSE(fm.hasFault(FAULT_CELL_UNDERVOLTAGE));
}

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

void test_fault_evaluate_overvoltage(void)
{
    FaultManager fm;
    fm.init();

    const float v[PACK_CELL_COUNT] = {4.30f, 4.18f, 4.19f};
    BatteryPack pack = makePack(v, 0.0f, 25.0f);

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault());
    TEST_ASSERT_TRUE(fm.hasFault(FAULT_CELL_OVERVOLTAGE));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::CELL_OVERVOLTAGE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0101, fm.getFaultInfo().code);
    TEST_ASSERT_EQUAL_UINT8(1, fm.getFaultInfo().source);
}

void test_fault_evaluate_undervoltage(void)
{
    FaultManager fm;
    fm.init();

    const float v[PACK_CELL_COUNT] = {3.20f, 2.95f, 3.10f};
    BatteryPack pack = makePack(v, 0.0f, 25.0f);

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault(FAULT_CELL_UNDERVOLTAGE));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::CELL_UNDERVOLTAGE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0102, fm.getFaultInfo().code);
    TEST_ASSERT_EQUAL_UINT8(2, fm.getFaultInfo().source);
}

void test_fault_evaluate_overtemperature(void)
{
    FaultManager fm;
    fm.init();

    const float v[PACK_CELL_COUNT] = {3.70f, 3.70f, 3.70f};
    BatteryPack pack = makePack(v, 0.0f, 65.0f);

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault(FAULT_OVER_TEMPERATURE));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::OVERTEMPERATURE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0201, fm.getFaultInfo().code);
    TEST_ASSERT_EQUAL_UINT8(0xFF, fm.getFaultInfo().source);
}

void test_fault_evaluate_overcurrent(void)
{
    FaultManager fm;
    fm.init();

    const float v[PACK_CELL_COUNT] = {3.70f, 3.70f, 3.70f};
    BatteryPack pack = makePack(v, 35.0f, 25.0f);

    fm.evaluate(pack);

    TEST_ASSERT_TRUE(fm.hasFault(FAULT_OVER_CURRENT));
    TEST_ASSERT_TRUE(fm.getFaultInfo().active);
    TEST_ASSERT_EQUAL(FaultReason::OVERCURRENT_CHARGE, fm.getFaultInfo().reason);
    TEST_ASSERT_EQUAL_UINT16(0x0301, fm.getFaultInfo().code);
}

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

// ---------------------------------------------------------------------------
// API DTC (Diagnostic Trouble Code)
// ---------------------------------------------------------------------------

// Sink de teste: captura eventos de log
class TestLogSink : public IFaultLogSink
{
public:
    int logCount = 0;
    FaultEvent lastEvent;

    void onFaultLogged(const FaultEvent& event) override
    {
        lastEvent = event;
        ++logCount;
    }
};

// Sink de teste: captura eventos de persistência
class TestStorageSink : public IFaultStorageSink
{
public:
    int persistCount = 0;
    FaultEvent lastEvent;

    bool onFaultPersist(const FaultEvent& event) override
    {
        lastEvent = event;
        ++persistCount;
        return true;
    }
};

void test_dtc_raise_fault_creates_active_event(void)
{
    FaultManager fm;
    fm.init();

    TestLogSink logSink;
    TestStorageSink storageSink;
    fm.setLogSink(&logSink);
    fm.setStorageSink(&storageSink);

    fm.raiseFault(FaultCode::BMS_CELL_OVERVOLTAGE);

    TEST_ASSERT_TRUE(fm.hasFault());
    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE));
    TEST_ASSERT_EQUAL_UINT32(1, fm.getActiveFaults());

    // Sinks foram notificados
    TEST_ASSERT_EQUAL_INT(1, logSink.logCount);
    TEST_ASSERT_EQUAL_INT(1, storageSink.persistCount);

    // O evento ativo tem a descrição/resolução correta
    FaultEvent ev;
    TEST_ASSERT_TRUE(fm.getActiveFault(0, ev));
    TEST_ASSERT_EQUAL(FaultCode::BMS_CELL_OVERVOLTAGE, ev.code);
    TEST_ASSERT_EQUAL(FaultState::ACTIVE, ev.state);
    TEST_ASSERT_EQUAL(UINT32_C(1), ev.occurrence);
    TEST_ASSERT_EQUAL_STRING("Cell over voltage", FaultRegistry::codeToString(ev.code));
}

void test_dtc_raise_same_fault_increments_occurrence(void)
{
    FaultManager fm;
    fm.init();

    fm.raiseFault(FaultCode::BMS_OVER_TEMPERATURE);
    fm.raiseFault(FaultCode::BMS_OVER_TEMPERATURE);

    // Não duplica: continua 1 ativo, mas ocorrência incrementa
    TEST_ASSERT_EQUAL_UINT32(1, fm.getActiveFaults());

    FaultEvent ev;
    fm.getActiveFault(0, ev);
    TEST_ASSERT_EQUAL(UINT32_C(2), ev.occurrence);
}

void test_dtc_clear_fault_removes_active(void)
{
    FaultManager fm;
    fm.init();

    TestLogSink logSink;
    fm.setLogSink(&logSink);

    fm.raiseFault(FaultCode::BMS_CAN_FAILURE);
    TEST_ASSERT_TRUE(fm.hasFault());

    fm.clearFault(FaultCode::BMS_CAN_FAILURE);

    TEST_ASSERT_FALSE(fm.hasFault());
    TEST_ASSERT_FALSE(fm.hasFault(FaultCode::BMS_CAN_FAILURE));
    TEST_ASSERT_EQUAL_UINT32(0, fm.getActiveFaults());
}

void test_dtc_raise_multiple_distinct_faults(void)
{
    FaultManager fm;
    fm.init();

    fm.raiseFault(FaultCode::BMS_CELL_OVERVOLTAGE);
    fm.raiseFault(FaultCode::BMS_OVER_CURRENT);
    fm.raiseFault(FaultCode::BMS_SENSOR_FAILURE);

    TEST_ASSERT_EQUAL_UINT32(3, fm.getActiveFaults());
    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE));
    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_OVER_CURRENT));
    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_SENSOR_FAILURE));
}

// ---------------------------------------------------------------------------
// FaultRegistry
// ---------------------------------------------------------------------------

void test_registry_code_to_string(void)
{
    TEST_ASSERT_EQUAL_STRING("Cell over voltage", FaultRegistry::codeToString(FaultCode::BMS_CELL_OVERVOLTAGE));
    TEST_ASSERT_EQUAL_STRING("Over temperature", FaultRegistry::codeToString(FaultCode::BMS_OVER_TEMPERATURE));
    TEST_ASSERT_EQUAL_STRING("Over current", FaultRegistry::codeToString(FaultCode::BMS_OVER_CURRENT));
    TEST_ASSERT_EQUAL_STRING("Battery over voltage", FaultRegistry::codeToString(FaultCode::BMS_BAT_OVERVOLTAGE));
    TEST_ASSERT_EQUAL_STRING("Unknown fault", FaultRegistry::codeToString(FaultCode::NONE));
}

void test_registry_severity(void)
{
    TEST_ASSERT_EQUAL(FaultSeverity::CRITICAL, FaultRegistry::findSeverity(FaultCode::BMS_CELL_OVERVOLTAGE));
    TEST_ASSERT_EQUAL(FaultSeverity::FATAL, FaultRegistry::findSeverity(FaultCode::BMS_SHORT_CIRCUIT));
    TEST_ASSERT_EQUAL(FaultSeverity::WARNING, FaultRegistry::findSeverity(FaultCode::BMS_LOW_TEMPERATURE));
    TEST_ASSERT_EQUAL(FaultSeverity::ERROR, FaultRegistry::findSeverity(FaultCode::BMS_STORAGE_FAILURE));
}

void test_registry_string_to_code(void)
{
    TEST_ASSERT_EQUAL(FaultCode::BMS_OVER_CURRENT, FaultRegistry::stringToCode("Over current"));
    TEST_ASSERT_EQUAL(FaultCode::NONE, FaultRegistry::stringToCode("Does not exist"));
}

// ---------------------------------------------------------------------------
// Sprint DTC-02 – Proteção unificada ProtectionManager -> FaultManager -> DTC
// ---------------------------------------------------------------------------

// Constrói um pack 4 células com tensões customizadas e preenche max/min.
static BatteryPack makePack4(const float* voltages, float current, float maxTemp, float minTemp)
{
    BatteryPack pack;

    for (std::uint8_t i = 0; i < PACK_CELL_COUNT && i < 4u; ++i)
    {
        pack.cells[i].voltage = voltages[i];
        pack.cells[i].valid = true;
    }

    pack.current = current;
    pack.maxTemperature = maxTemp;
    pack.minTemperature = minTemp;
    pack.averageTemperature = maxTemp;

    pack.maxVoltage = voltages[0];
    pack.minVoltage = voltages[0];
    for (std::uint8_t i = 1; i < PACK_CELL_COUNT && i < 4u; ++i)
    {
        if (voltages[i] > pack.maxVoltage) pack.maxVoltage = voltages[i];
        if (voltages[i] < pack.minVoltage) pack.minVoltage = voltages[i];
    }

    return pack;
}

// DTC-02.A: ProtectionManager detecta overvoltage e identifica a célula correta
// (source 1-based) com o valor medido/limite.
void test_dtc02_protection_cell_overvoltage_source(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    // Célula 3 é a maior (4.30V) -> source deve ser 3
    const float v[4] = {4.10f, 4.15f, 4.30f, 4.20f};
    BatteryPack pack = makePack4(v, 0.0f, 25.0f, 25.0f);

    pm.update(pack, fm);

    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE));

    FaultEvent ev;
    TEST_ASSERT_TRUE(fm.getActiveFault(0, ev));
    TEST_ASSERT_EQUAL(FaultCode::BMS_CELL_OVERVOLTAGE, ev.code);
    TEST_ASSERT_EQUAL(FaultState::ACTIVE, ev.state);

    // Célula correta (1-based) que disparou a falha
    TEST_ASSERT_EQUAL_UINT8(3, ev.source);
    // Valor medido da célula ofensora
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 4.30f, ev.measuredValue);
    // Limite de sobretensão
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 4.20f, ev.limit);
}

// DTC-02.B: Normalização -> CLEARED (fault some quando o pack volta ao normal)
void test_dtc02_protection_normalization_clears_fault(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    const float v[4] = {4.10f, 4.15f, 4.30f, 4.20f};
    BatteryPack pack = makePack4(v, 0.0f, 25.0f, 25.0f);

    pm.update(pack, fm);
    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE));

    // Normaliza o pack
    const float ok[4] = {3.70f, 3.72f, 3.71f, 3.70f};
    BatteryPack normal = makePack4(ok, 0.0f, 25.0f, 25.0f);

    pm.update(normal, fm);

    TEST_ASSERT_FALSE(fm.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE));
    TEST_ASSERT_FALSE(fm.hasFault());
    TEST_ASSERT_EQUAL_UINT32(0, fm.getActiveFaults());
}

// DTC-02.C: Under voltage com identificação da célula correta
void test_dtc02_protection_cell_undervoltage_source(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    // Célula 2 é a menor (2.95V) -> source deve ser 2
    const float v[4] = {3.20f, 2.95f, 3.10f, 3.18f};
    BatteryPack pack = makePack4(v, 0.0f, 25.0f, 25.0f);

    pm.update(pack, fm);

    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_CELL_UNDERVOLTAGE));

    FaultEvent ev;
    TEST_ASSERT_TRUE(fm.getActiveFault(0, ev));
    TEST_ASSERT_EQUAL(FaultCode::BMS_CELL_UNDERVOLTAGE, ev.code);
    TEST_ASSERT_EQUAL_UINT8(2, ev.source);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.95f, ev.measuredValue);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.00f, ev.limit);
}

// DTC-02.D: Over temperature
void test_dtc02_protection_over_temperature(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    const float v[4] = {3.70f, 3.70f, 3.70f, 3.70f};
    BatteryPack pack = makePack4(v, 0.0f, 65.0f, 65.0f);

    pm.update(pack, fm);

    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_OVER_TEMPERATURE));
    TEST_ASSERT_EQUAL(ProtectionState::OVER_TEMPERATURE, pm.getState());
    TEST_ASSERT_FALSE(pm.chargeEnabled());
    TEST_ASSERT_FALSE(pm.dischargeEnabled());
}

// DTC-02.E: Over current protege os MOSFETs
void test_dtc02_protection_over_current(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    const float v[4] = {3.70f, 3.70f, 3.70f, 3.70f};
    BatteryPack pack = makePack4(v, 35.0f, 25.0f, 25.0f);

    pm.update(pack, fm);

    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_OVER_CURRENT));
    TEST_ASSERT_EQUAL(ProtectionState::OVER_CURRENT_CHARGE, pm.getState());
    TEST_ASSERT_FALSE(pm.chargeEnabled());
    TEST_ASSERT_FALSE(pm.dischargeEnabled());
}

// DTC-02.F: Múltiplas falhas simultâneas (overvoltage + overtemperature)
void test_dtc02_protection_multiple_simultaneous_faults(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    const float v[4] = {4.25f, 4.10f, 4.10f, 4.20f};
    BatteryPack pack = makePack4(v, 0.0f, 65.0f, 65.0f);

    pm.update(pack, fm);

    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE));
    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_OVER_TEMPERATURE));
    TEST_ASSERT_EQUAL_UINT32(2, fm.getActiveFaults());
}

// DTC-02.G: Overvoltage protege MOSFET de carga (charge off, discharge on)
void test_dtc02_protection_overvoltage_mosfet(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    const float v[4] = {4.10f, 4.15f, 4.30f, 4.20f};
    BatteryPack pack = makePack4(v, 0.0f, 25.0f, 25.0f);

    pm.update(pack, fm);

    TEST_ASSERT_EQUAL(ProtectionState::OVER_VOLTAGE, pm.getState());
    TEST_ASSERT_FALSE(pm.chargeEnabled());
    TEST_ASSERT_TRUE(pm.dischargeEnabled());
}

// DTC-02.H: Short circuit (corrente 10x limite) -> FATAL, MOSFETs off
void test_dtc02_protection_short_circuit(void)
{
    FaultManager fm;
    fm.init();
    ProtectionManager pm;
    pm.init();

    const float v[4] = {3.70f, 3.70f, 3.70f, 3.70f};
    BatteryPack pack = makePack4(v, 350.0f, 25.0f, 25.0f);

    pm.update(pack, fm);

    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_SHORT_CIRCUIT));
    TEST_ASSERT_TRUE(fm.hasFault(FaultCode::BMS_OVER_CURRENT));
    TEST_ASSERT_EQUAL(ProtectionState::SHORT_CIRCUIT, pm.getState());
    TEST_ASSERT_FALSE(pm.chargeEnabled());
    TEST_ASSERT_FALSE(pm.dischargeEnabled());
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    // API legada
    RUN_TEST(test_fault_init_has_no_faults);
    RUN_TEST(test_fault_add_fault);
    RUN_TEST(test_fault_add_multiple);
    RUN_TEST(test_fault_clear);
    RUN_TEST(test_fault_evaluate_overvoltage);
    RUN_TEST(test_fault_evaluate_undervoltage);
    RUN_TEST(test_fault_evaluate_overtemperature);
    RUN_TEST(test_fault_evaluate_overcurrent);
    RUN_TEST(test_fault_evaluate_normal_pack);

    // API DTC
    RUN_TEST(test_dtc_raise_fault_creates_active_event);
    RUN_TEST(test_dtc_raise_same_fault_increments_occurrence);
    RUN_TEST(test_dtc_clear_fault_removes_active);
    RUN_TEST(test_dtc_raise_multiple_distinct_faults);

// Registry
    RUN_TEST(test_registry_code_to_string);
    RUN_TEST(test_registry_severity);
    RUN_TEST(test_registry_string_to_code);

    // Sprint DTC-02 – Proteção unificada
    RUN_TEST(test_dtc02_protection_cell_overvoltage_source);
    RUN_TEST(test_dtc02_protection_normalization_clears_fault);
    RUN_TEST(test_dtc02_protection_cell_undervoltage_source);
    RUN_TEST(test_dtc02_protection_over_temperature);
    RUN_TEST(test_dtc02_protection_over_current);
    RUN_TEST(test_dtc02_protection_multiple_simultaneous_faults);
    RUN_TEST(test_dtc02_protection_overvoltage_mosfet);
    RUN_TEST(test_dtc02_protection_short_circuit);

    return UNITY_END();
}
