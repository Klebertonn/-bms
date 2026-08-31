#pragma once

#include <cstdint>

/*
 * ==========================================================
 * Heartbeat Manager
 * ==========================================================
 * Emite periodicamente (1000 ms) um heartbeat com:
 *   - Firmware vivo (alive)
 *   - Estado do BMS
 *   - Tempo de operação (uptime)
 *   - SOC
 *   - Temperatura
 *
 * Canais de saída:
 *   - CAN  (via CANProtocol::makeHeartbeat + ICANDriver)
 *   - BLE  (via BLENotification — hook para publicação)
 *   - UART / Dashboard (linha formatada via printf)
 *
 * Módulo novo e autossuficiente; não altera arquitetura existente.
 * ==========================================================
 */

// Período de emissão do heartbeat (ms)
constexpr std::uint32_t HEARTBEAT_PERIOD_MS = 1000;

// Dados transmitidos no heartbeat
struct HeartbeatData
{
    bool alive = true;              // Firmware vivo
    const char* state = "UNKNOWN";  // Estado do BMS (ex.: READY, CHARGING...)
    std::uint32_t uptimeMs = 0;     // Tempo desde o boot
    std::uint8_t soc = 0;           // State of Charge (%)
    float temperatureC = 0.0f;      // Temperatura média (°C)
};

class HeartbeatManager
{
public:
    // Inicializa o heartbeat (zera contadores).
    void init();

    // Deve ser chamado no loop principal.
    // Emite o heartbeat quando o período (1000 ms) é atingido.
    void update();

    // Envia o heartbeat imediatamente (independente do período).
    void send();

    // Setters de dados (chamados pelo App/telemetria).
    void setState(const char* state) { data_.state = state; }
    void setSOC(std::uint8_t soc) { data_.soc = soc; }
    void setTemperature(float temperatureC) { data_.temperatureC = temperatureC; }

    // Getters
    std::uint32_t lastBeatMs() const { return lastBeatMs_; }
    bool isAlive() const { return data_.alive; }
    const HeartbeatData& data() const { return data_; }

private:
    // Envia via CAN (frame 0x100 HEARTBEAT).
    void sendCAN();

    // Envia via BLE (hook para publicação de notificação).
    void sendBLE();

    // Envia via UART/Dashboard (linha formatada).
    void sendUART();

HeartbeatData data_{};
    std::uint32_t lastBeatMs_ = 0;
};
