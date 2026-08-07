#include <unity.h>

#include "system/storage/storage_manager.h"
#include "system/storage/storage_interface.h"
#include "system/storage/storage_crc.h"

// ============================================================================
// Mock driver de armazenamento em RAM (para teste do StorageManager)
// ============================================================================
namespace
{
    StorageData g_storage{};
    bool g_load_ok = true;
    bool g_save_ok = true;
    bool g_erase_ok = true;

    class MockStorageDriver : public IStorageDriver
    {
    public:
        bool load(StorageData& data) override
        {
            if (!g_load_ok) return false;
            data = g_storage;
            return true;
        }

        bool save(const StorageData& data) override
        {
            if (!g_save_ok) return false;
            g_storage = data;
            return true;
        }

        bool erase() override
        {
            if (!g_erase_ok) return false;
            g_storage = StorageData{};
            return true;
        }
    };

    MockStorageDriver drive;

    void resetMock()
    {
        g_storage = StorageData{};
        g_load_ok = true;
        g_save_ok = true;
        g_erase_ok = true;
    }
}

void setUp(void)
{
    resetMock();
}

void tearDown(void) {}

// ============================================================================
// Testes — CRC
// ============================================================================
void test_storage_crc_deterministic(void)
{
    StorageData d1{};
    StorageData d2{};

    d1.soc = 85.0f;
    d1.soh = 90.0f;
    d2 = d1;

    uint32_t c1 = crc32(&d1, sizeof(StorageData));
    uint32_t c2 = crc32(&d2, sizeof(StorageData));

    TEST_ASSERT_EQUAL_UINT32(c1, c2);
    TEST_ASSERT_NOT_EQUAL_UINT32(0, c1);
}

void test_storage_crc_differs_on_change(void)
{
    StorageData a{};
    StorageData b{};

    a.cycleCount = 10;
    b.cycleCount = 11;

    uint32_t ca = crc32(&a, sizeof(StorageData));
    uint32_t cb = crc32(&b, sizeof(StorageData));

    TEST_ASSERT_NOT_EQUAL_UINT32(ca, cb);
}

// ============================================================================
// Testes — StorageManager
// ============================================================================
void test_storage_init_saves_defaults(void)
{
    // g_storage vazio -> load falha? Na prática mock retorna true com dados vazios.
    // Para testar defaults, forçamos load falha.
    g_load_ok = false;

    StorageManager sm;
    sm.init();

    StorageData& d = sm.data();

    // init chamou loadDefaults + save
    TEST_ASSERT_EQUAL_UINT32(1, d.version);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, d.soc);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, d.soh);
    TEST_ASSERT_EQUAL_UINT32(0, d.cycleCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 4.2f, d.overVoltageLimit);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, d.underVoltageLimit);
}

void test_storage_save_then_load_roundtrip(void)
{
    StorageManager sm;
    sm.init();

    // Grava dados customizados
    StorageData& d = sm.data();
    d.soc = 42.0f;
    d.soh = 88.0f;
    d.cycleCount = 7;

    TEST_ASSERT_TRUE(sm.save());

    // Novo manager - deve carregar o que foi salvo
    g_load_ok = true;
    StorageManager sm2;
    TEST_ASSERT_TRUE(sm2.load());

    StorageData& loaded = sm2.data();
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 42.0f, loaded.soc);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 88.0f, loaded.soh);
    TEST_ASSERT_EQUAL_UINT32(7, loaded.cycleCount);
}

void test_storage_load_defaults_when_invalid(void)
{
    // Corrompe storage (CRC inválido)
    g_load_ok = true;
    g_storage = StorageData{};
    g_storage.version = 0;
    g_storage.crc = 0xBAD;

    StorageManager sm;
    // load() deve retornar false (CRC inválido ou version errada)
    TEST_ASSERT_FALSE(sm.load());

    // init() então aplica defaults
    sm.init();
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, sm.data().soc);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, sm.data().underVoltageLimit);
}

void test_storage_factory_reset(void)
{
    StorageManager sm;
    sm.init();

    sm.data().soh = 50.0f;
    sm.data().cycleCount = 100;
    TEST_ASSERT_TRUE(sm.save());

    sm.factoryReset();

    StorageData& d = sm.data();
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, d.soh);
    TEST_ASSERT_EQUAL_UINT32(0, d.cycleCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, d.soc);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_storage_crc_deterministic);
    RUN_TEST(test_storage_crc_differs_on_change);
    RUN_TEST(test_storage_init_saves_defaults);
    RUN_TEST(test_storage_save_then_load_roundtrip);
    RUN_TEST(test_storage_load_defaults_when_invalid);
    RUN_TEST(test_storage_factory_reset);

    return UNITY_END();
}
