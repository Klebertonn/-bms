#include <unity.h>

#include <cstdint>

#include "system/storage/storage_types.h"
#include "system/storage/storage_interface.h"
#include "system/storage/storage_crc.h"
#include "system/storage/storage_manager.h"

// ============================================================================
// Testes unitários — EEPROM / Flash / Armazenamento
// ============================================================================
// Como os drivers EEPROM/FLASH são stubs no ambiente native, os testes
// validam a lógica de armazenamento (CRC, StorageManager, round-trip)
// e a interface IStorageDriver (NVS mock).

void setUp(void) {}
void tearDown(void) {}

// Testa CRC determinístico
void test_eeprom_crc_deterministic(void)
{
    StorageData a{};
    StorageData b{};

    a.soc = 50.0f;
    a.soh = 90.0f;
    a.cycleCount = 3;
    b = a;

    const std::uint32_t ca = crc32(&a, sizeof(StorageData));
    const std::uint32_t cb = crc32(&b, sizeof(StorageData));

    TEST_ASSERT_EQUAL_UINT32(ca, cb);
    TEST_ASSERT_NOT_EQUAL_UINT32(0, ca);
}

// Testa CRC muda quando dados mudam
void test_eeprom_crc_detects_change(void)
{
    StorageData a{};
    StorageData b{};

    a.cycleCount = 1;
    b.cycleCount = 2;

    TEST_ASSERT_NOT_EQUAL_UINT32(crc32(&a, sizeof(StorageData)),
                                 crc32(&b, sizeof(StorageData)));
}

// Testa round-trip save/load via StorageManager (backed por mock driver)
void test_eeprom_storage_roundtrip(void)
{
    StorageManager sm;
    sm.init();

    sm.data().soc = 88.0f;
    sm.data().soh = 94.0f;
    sm.data().cycleCount = 12;
    sm.data().batteryCapacityAh = 50.0f;

    TEST_ASSERT_TRUE(sm.save());

    // Recarrega e verifica
    TEST_ASSERT_TRUE(sm.load());
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 88.0f, sm.data().soc);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 94.0f, sm.data().soh);
    TEST_ASSERT_EQUAL_UINT32(12, sm.data().cycleCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 50.0f, sm.data().batteryCapacityAh);
}

// Testa factory reset
void test_eeprom_factory_reset(void)
{
    StorageManager sm;
    sm.init();

    sm.data().soc = 30.0f;
    sm.data().cycleCount = 99;
    sm.save();

    sm.factoryReset();

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, sm.data().soc);
    TEST_ASSERT_EQUAL_UINT32(0, sm.data().cycleCount);
}

// Testa valores padrão após init
void test_eeprom_defaults(void)
{
    StorageManager sm;
    sm.init();

    TEST_ASSERT_EQUAL_UINT32(1, sm.data().version);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, sm.data().soh);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 4.2f, sm.data().overVoltageLimit);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, sm.data().underVoltageLimit);
}

// Testa que o CRC é gravado e validado (campo crc)
void test_eeprom_crc_field_updated(void)
{
    StorageManager sm;
    sm.init();

    sm.data().soc = 75.0f;
    sm.data().cycleCount = 5;
    sm.save();

    // Após save, o campo crc não deve ser zero (foi calculado)
    TEST_ASSERT_NOT_EQUAL_UINT32(0, sm.data().crc);
}

// Testa que o StorageData tem tamanho estável (para persistência)
void test_eeprom_storage_struct_size(void)
{
    // O struct deve ter tamanho razoável para persistência
    TEST_ASSERT_TRUE(sizeof(StorageData) > 0);
    TEST_ASSERT_TRUE(sizeof(StorageData) <= 256);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_eeprom_crc_deterministic);
    RUN_TEST(test_eeprom_crc_detects_change);
    RUN_TEST(test_eeprom_storage_roundtrip);
    RUN_TEST(test_eeprom_factory_reset);
    RUN_TEST(test_eeprom_defaults);
    RUN_TEST(test_eeprom_crc_field_updated);
    RUN_TEST(test_eeprom_storage_struct_size);

    return UNITY_END();
}
