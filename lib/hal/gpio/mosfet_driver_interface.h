#pragma once

#include "core/state/bms_state_manager.h"

/*
 * ==========================================================
 * Interface HAL — Driver de MOSFET
 * ==========================================================
 * Abstrai o controle dos MOSFETs de carga, descarga e
 * balanceamento. É uma porta (port) da Arquitetura Hexagonal:
 * o Core (App) depende desta interface, e a implementação
 * concreta é injetada na inicialização:
 *   - Mock  (native)  →  MockMosfetDriver (flags)
 *   - Real  (esp32)   →  HalMosfetController (GPIO)
 *
 * Esta interface NÃO contém nada específico de ESP32.
 * ==========================================================
 */
class IMosfetDriver
{
public:
    virtual ~IMosfetDriver() = default;

    // Inicializa os MOSFETs em estado seguro (tudo OFF).
    virtual void init() = 0;

    // Aplica o estado industrial (BmsState) nos MOSFETs.
    virtual void applyState(BmsState state) = 0;

    // Estado atual (espelho do hardware / flags).
    virtual bool chargeEnabled() const = 0;
    virtual bool dischargeEnabled() const = 0;
    virtual bool balanceEnabled() const = 0;
};
