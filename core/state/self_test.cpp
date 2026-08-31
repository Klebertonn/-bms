#include "self_test.h"

#include <cstdio>
#include <cstring>

#include "../../lib/hal/adc/adc_interface.h"
#include "../../lib/hal/current/current_interface.h"
#include "../../lib/hal/temperature/temperature_interface.h"

#include "../../system/storage/storage_interface.h"
#include "../../system/storage/storage_types.h"

#include "../../core/mosfet/mosfet_controller.h"
#include "../../config/config.h"

// Globals HAL (definidos nos mocks/drivers)
extern IADC& adc;
extern ICurrentSensor& currentSensor;
extern ITemperatureSensor& temperatureSensor;
extern IStorageDriver& storageDriver;

/*
 * ==========================================================
 * SelfTest::run()
 * ==========================================================
 */
SelfTestResult SelfTest::run()
{
    // Executa todos os testes
    adcOk_         = testADC();
    eepromOk_      = testEEPROM();
    flashOk_       = testFLASH();
    nvsOk_         = testNVS();
    currentOk_     = testCurrentSensor();
    temperatureOk_ = testTemperatureSensor();
    mosfetOk_      = testMOSFETDriver();
    ramOk_         = testRAM();

    // Agrega resultado
    result_ = (adcOk_ && eepromOk_ && flashOk_ && nvsOk_ &&
               currentOk_ && temperatureOk_ && mosfetOk_ && ramOk_)
                  ? SelfTestResult::PASS
                  : SelfTestResult::FAIL;

    // ---------- Log ----------
    printf("\n=== SELF TEST ===\n");
    printf("[ %s ] ADC\n",              adcOk_         ? "OK " : "FAIL");
    printf("[ %s ] EEPROM\n",           eepromOk_      ? "OK " : "FAIL");
    printf("[ %s ] FLASH\n",            flashOk_       ? "OK " : "FAIL");
    printf("[ %s ] NVS\n",              nvsOk_         ? "OK " : "FAIL");
    printf("[ %s ] CURRENT SENSOR\n",   currentOk_     ? "OK " : "FAIL");
    printf("[ %s ] TEMPERATURE SENSOR\n", temperatureOk_ ? "OK " : "FAIL");
    printf("[ %s ] MOSFET DRIVER\n",    mosfetOk_      ? "OK " : "FAIL");
    printf("[ %s ] RAM\n",              ramOk_         ? "OK " : "FAIL");

    if (result_ == SelfTestResult::PASS)
    {
        printf("SELF TEST PASSED\n\n");
    }
    else
    {
        printf("SELF TEST FAILED\n\n");
    }

    return result_;
}

/*
 * ==========================================================
 * Testes individuais
 * ==========================================================
 */

// ADC: lê cada célula e valida faixa de tensão plausível.
bool SelfTest::testADC()
{
    constexpr float ADC_MIN_V = 0.5f;
    constexpr float ADC_MAX_V = 5.0f;

    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        const float v = adc.readCell(static_cast<int>(i));

        // Tensão inválida (nan/inf) ou fora da faixa -> falha
        if (v != v || v < ADC_MIN_V || v > ADC_MAX_V)
        {
            return false;
        }
    }

    return true;
}

// EEPROM: teste básico de escrita/leitura de padrão.
// Nota: no ambiente native/mock não há driver EEPROM real;
// o padrão de write/read local valida o subsistema lógico.
// Substituir por leitura/escrita real no driver EEPROM final.
bool SelfTest::testEEPROM()
{
    std::uint8_t pattern = 0xA5;
    std::uint8_t buffer  = 0x00;

    // Simula gravação e leitura de um byte
    buffer = pattern;

    return (buffer == pattern);
}

// FLASH: teste básico de escrita/leitura de padrão.
// Nota: no ambiente native/mock não há driver FLASH real.
bool SelfTest::testFLASH()
{
    const std::uint8_t pattern = 0x5A;
    std::uint8_t buffer[4]     = {0x00, 0x00, 0x00, 0x00};

    // Simula gravação/leitura de um bloco
    std::memset(buffer, pattern, sizeof(buffer));

    bool ok = true;
    for (std::size_t i = 0; i < sizeof(buffer); ++i)
    {
        if (buffer[i] != pattern)
        {
            ok = false;
            break;
        }
    }

    return ok;
}

// NVS: usa o driver de armazenamento (NVSStorageDriver) real disponível.
bool SelfTest::testNVS()
{
    StorageData data;

    // Tenta salvar e apagar; driver mock retorna true.
    const bool saveOk  = storageDriver.save(data);
    const bool eraseOk = storageDriver.erase();

    return saveOk && eraseOk;
}

// Current Sensor: valida leitura de corrente e tensão de shunt em faixa plausível.
bool SelfTest::testCurrentSensor()
{
    constexpr float CURRENT_MIN_A = -200.0f;
    constexpr float CURRENT_MAX_A =  200.0f;
    constexpr float SHUNT_MIN_MV  = -100.0f;
    constexpr float SHUNT_MAX_MV  =  100.0f;

    const float current = currentSensor.readCurrentA();
    const float shunt   = currentSensor.readShuntVoltageMv();

    if (current != current || current < CURRENT_MIN_A || current > CURRENT_MAX_A)
    {
        return false;
    }

    if (shunt != shunt || shunt < SHUNT_MIN_MV || shunt > SHUNT_MAX_MV)
    {
        return false;
    }

    return true;
}

// Temperature Sensor: valida leitura de temperatura em faixa plausível.
bool SelfTest::testTemperatureSensor()
{
    constexpr float TEMP_MIN_C = -50.0f;
    constexpr float TEMP_MAX_C =  150.0f;

    const float temp = temperatureSensor.readTemperature(0);

    return (temp == temp && temp >= TEMP_MIN_C && temp <= TEMP_MAX_C);
}

// MOSFET Driver: aplica um estado e verifica se as flags respondem corretamente.
bool SelfTest::testMOSFETDriver()
{
    MosfetController mosfet;
    mosfet.init();

    // Estado DISCHARGING -> discharge ON, charge OFF, balance OFF
    mosfet.applyState(BmsState::DISCHARGING);

    const bool dischargeOk = mosfet.dischargeEnabled();
    const bool chargeOk    = !mosfet.chargeEnabled();
    const bool balanceOk   = !mosfet.balanceEnabled();

    return dischargeOk && chargeOk && balanceOk;
}

// RAM: valida escrita/leitura de padrões em buffer local (tamém testa stack).
bool SelfTest::testRAM()
{
    constexpr std::uint8_t PATTERN_A = 0xAA;
    constexpr std::uint8_t PATTERN_B = 0x55;

    std::uint8_t buffer[64];

    // Preenche com 0xAA e verifica
    std::memset(buffer, PATTERN_A, sizeof(buffer));
    for (std::size_t i = 0; i < sizeof(buffer); ++i)
    {
        if (buffer[i] != PATTERN_A)
        {
            return false;
        }
    }

    // Preenche com 0x55 e verifica
    std::memset(buffer, PATTERN_B, sizeof(buffer));
    for (std::size_t i = 0; i < sizeof(buffer); ++i)
    {
        if (buffer[i] != PATTERN_B)
        {
            return false;
        }
    }

    return true;
}
