#pragma once

#include "../../core/battery/battery_manager.h"
#include "../../core/current/current_manager.h"
#include "../../core/temperature/temperature_manager.h"
#include "../../core/protection/protection_manager.h"
#include "../../core/balance/balance_manager.h"
#include "../../core/fault/fault_manager.h"
#include "../../core/state/bms_state_manager.h"
#include "../../core/state/bms_state_machine.h"

#include "../../core/mosfet/mosfet_controller.h"
#include "../../core/fault/fault_history.h"

#include "../../system/logger/logger.h"
#include "../../system/storage/fault_storage.h"
#include "../../system/heartbeat/heartbeat_manager.h"

#include "../../lib/hal/gpio/mosfet_driver_interface.h"
#include "../hal/gpio/mock_mosfet_driver.h"

/**
 * @brief Aplicação principal do BMS.
 *
 * Orquestra a lógica de negócio (core/) e a integração com a camada HAL.
 * Responsável por: telemetria, proteções, máquina de estados, MOSFETs,
 * balanceamento, histórico de falhas e heartbeat.
 */
class App
{
public:
    bool init();
    void update();

    // Injeção de dependência: permite substituir o driver de MOSFET
    // (mock no native, HalMosfetController no esp32dev).
    // Deve ser chamado antes de init().
    void setMosfetDriver(IMosfetDriver* driver);

private:
    void printTelemetry(const BatteryPack& pack);
    void printFaultHistory();

    // Driver de MOSFET selecionado (mock por padrão para native).
    MockMosfetDriver mockMosfetDriver_{};
    IMosfetDriver* mosfetDriver_ = &mockMosfetDriver_;

    BatteryManager battery_{};
    CurrentManager current_{};
    TemperatureManager temperature_{};
    ProtectionManager protection_{};
    FaultManager fault_{};

    BmsStateManager stateManager_{};
    BmsStateMachine bmsStateMachine_{};

    BalanceManager balance_{};
    MosfetController mosfet_{};

    FaultHistory faultHistory_{};
    FaultStorage faultStorage_{};
    HeartbeatManager heartbeat_{};

    // Último snapshot de falha enviado (evita repetir o mesmo evento).
    bool lastFaultActive_ = false;
    FaultReason lastFaultReason_ = FaultReason::NONE;
    std::uint16_t lastFaultCode_ = 0;
    std::uint8_t lastFaultSource_ = 0xFF;
};




