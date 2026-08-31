#pragma once

#include <cstdint>

/*
 * ==========================================================
 * Self Test — Power-On Self Test (POST)
 * ==========================================================
 * Valida os periféricos críticos do BMS durante o boot:
 *   - ADC
 *   - EEPROM
 *   - FLASH
 *   - NVS
 *   - Sensor de Corrente
 *   - Sensor de Temperatura
 *   - Driver de MOSFET
 *   - RAM
 *
 * Resultado global: PASS ou FAIL.
 * Integrado com a BmsStateMachine: SELF_TEST -> READY (PASS)
 *                                      ou  -> FAULT (FAIL)
 * ==========================================================
 */

enum class SelfTestResult
{
    NOT_RUN,   // Self test ainda não foi executado
    PASS,      // Todos os testes passaram
    FAIL       // Pelo menos um teste falhou
};

class SelfTest
{
public:
    // Executa todos os testes de hardware e imprime o log.
    // @return PASS se todos passaram, FAIL caso contrário.
    SelfTestResult run();

    // Resultado agregado registrado no último run().
    SelfTestResult result() const { return result_; }

    // Atalho: true se o último run() passou.
    bool passed() const { return result_ == SelfTestResult::PASS; }

    // Getters individuais (para diagnóstico/BD).
    bool adcOk() const { return adcOk_; }
    bool eepromOk() const { return eepromOk_; }
    bool flashOk() const { return flashOk_; }
    bool nvsOk() const { return nvsOk_; }
    bool currentOk() const { return currentOk_; }
    bool temperatureOk() const { return temperatureOk_; }
    bool mosfetOk() const { return mosfetOk_; }
    bool ramOk() const { return ramOk_; }

private:
    // Removemos os testes individuais da API pública; cada teste retorna bool.
    bool testADC();
    bool testEEPROM();
    bool testFLASH();
    bool testNVS();
    bool testCurrentSensor();
    bool testTemperatureSensor();
    bool testMOSFETDriver();
    bool testRAM();

// Resultado agregado
    SelfTestResult result_ = SelfTestResult::NOT_RUN;

    bool adcOk_ = false;
    bool eepromOk_ = false;
    bool flashOk_ = false;
    bool nvsOk_ = false;
    bool currentOk_ = false;
    bool temperatureOk_ = false;
    bool mosfetOk_ = false;
    bool ramOk_ = false;
};
