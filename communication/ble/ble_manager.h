#pragma once

#include "ble_service.h"
#include "../../core/battery/battery_pack.h"
#include "../../core/fault/fault_reason.h"

/*
 * ==========================================================
 * BLE Manager — Camada BLE industrial
 * ==========================================================
 * Gerencia os serviços GATT do BMS (Battery, Fault, Temperature,
 * Current, Config, Logs), publicando a telemetria nas
 * características legíveis. Não altera a arquitetura existente.
 * ==========================================================
 */
class BLEManager
{
public:
    // Inicializa os serviços GATT.
    void init();

    // Deve ser chamado no loop principal.
    void update();

    bool connected() const;

    // Setters de dados (chamados pela telemetria/App).
    void setPack(const BatteryPack& pack) { pack_ = pack; }
    void setFault(const FaultInfo& fi) { fault_ = fi; }
    void setState(uint8_t state) { state_ = state; }
    void setTemperature(float avg, float maxV, float minV)
    {
        tempAvg_ = avg;
        tempMax_ = maxV;
        tempMin_ = minV;
    }
    void setCurrent(float currentA, float shuntMv)
    {
        currentA_ = currentA;
        shuntMv_ = shuntMv;
    }
    void setLog(uint32_t timestamp, uint16_t code)
    {
        logTimestamp_ = timestamp;
        logCode_ = code;
        setLog_ = true;
    }
    void setConfig(float maxV, float minV, float maxT, float maxI)
    {
        cfgMaxV_ = maxV;
        cfgMinV_ = minV;
        cfgMaxT_ = maxT;
        cfgMaxI_ = maxI;
    }

    // Exposição dos serviços para acesso externo (diagnóstico).
    BLEServiceManager& serviceManager() { return serviceManager_; }

private:
    // Cria os serviços GATT.
    void createServices();

    void processCommands();

    // Publica a telemetria nas características.
    void publishTelemetry();

    bool isConnected = false;

    // Dados a publicar
    BatteryPack pack_{};
    FaultInfo fault_{};
    uint8_t state_ = 0;
    float tempAvg_ = 0.0f, tempMax_ = 0.0f, tempMin_ = 0.0f;
    float currentA_ = 0.0f, shuntMv_ = 0.0f;
    float cfgMaxV_ = 0.0f, cfgMinV_ = 0.0f, cfgMaxT_ = 0.0f, cfgMaxI_ = 0.0f;
    uint32_t logTimestamp_ = 0;
    uint16_t logCode_ = 0;
    bool setLog_ = false;

    BLEServiceManager serviceManager_{};
};

