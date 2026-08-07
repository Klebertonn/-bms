#pragma once

#include <cstdint>

/*
 * ==========================================================
 * BLE Service — Serviços GATT do BMS
 * ==========================================================
 * Define os serviços e características disponíveis para leitura
 * via Bluetooth Low Energy. Cada característica possui um buffer
 * de dados (payload) que é preenchido pela telemetria.
 *
 * Serviços:
 *   - Battery     : pack voltage, current, SOC, SOH, estado
 *   - Fault       : falha ativa (código, razão, fonte, valor, limite)
 *   - Temperature : média, máx, mín
 *   - Current     : corrente e tensão de shunt
 *   - Config      : limites de configuração
 *   - Logs        : evento/registro mais recente
 *
 * Módulo novo e autossuficiente; não altera arquitetura existente.
 * ==========================================================
 */

// Identificadores dos serviços GATT
enum class BLEServiceId
{
    BATTERY,
    FAULT,
    TEMPERATURE,
    CURRENT,
    CONFIG,
    LOGS,
    COUNT   // número de serviços
};

// Propriedades das características (bitmask)
enum BLECharProperty : uint8_t
{
    BLE_CHAR_READ   = (1u << 0),   // característica legível
    BLE_CHAR_NOTIFY = (1u << 1)    // característica notificável
};

// Tamanho máximo do payload de uma característica
constexpr std::uint8_t BLE_CHAR_MAX_LEN = 64;

// Descrição de uma característica
struct BLECharacteristic
{
    const char* name = nullptr;          // nome descritivo
    std::uint16_t uuid = 0;              // UUID 16-bit
    std::uint8_t properties = BLE_CHAR_READ;
    std::uint8_t length = 0;             // tamanho atual do payload
    std::uint8_t data[BLE_CHAR_MAX_LEN]{}; // payload
};

// Serviço GATT com um conjunto de características
struct BLEService
{
    const char* name = nullptr;          // nome do serviço
    std::uint16_t uuid = 0;              // UUID 16-bit
    std::uint8_t characteristicCount = 0;
    BLECharacteristic* characteristics = nullptr; // array de características
};

/*
 * ==========================================================
 * BLEServiceManager
 * ==========================================================
 * Gerencia os serviços GATT do BMS. Cada serviço expõe
 * características legíveis (READ). Fornece acesso ao buffer
 * de cada característica para escrita pela telemetria.
 * ==========================================================
 */
class BLEServiceManager
{
public:
    // Inicializa todos os serviços e características.
    void init();

    // Processa eventos (ex.: registra callbacks de leitura).
    void poll();

    // Retorna um serviço pelo seu id (ou nullptr se não existir).
    BLEService* getService(BLEServiceId id);

    // Retorna a característica de um serviço pelo índice.
    BLECharacteristic* getCharacteristic(BLEServiceId service, std::uint8_t index);

    // Atualiza o payload de uma característica (copia dados).
    bool setCharacteristicData(BLEServiceId service, std::uint8_t index,
                               const std::uint8_t* data, std::uint8_t len);

private:
    // Cria os serviços (Battery, Fault, Temperature, Current, Config, Logs).
    void createServices();

    // Arrays de características (um por serviço)
    BLECharacteristic batteryChars_[2];
    BLECharacteristic faultChars_[1];
    BLECharacteristic tempChars_[1];
    BLECharacteristic currentChars_[1];
    BLECharacteristic configChars_[1];
    BLECharacteristic logChars_[1];

    // Serviços
    BLEService services_[static_cast<int>(BLEServiceId::COUNT)];
};
