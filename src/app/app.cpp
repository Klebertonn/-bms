#include "app.h"

#include <cstdio>

static const char* protectionStateToString(ProtectionState state)
{
    switch (state)
    {
        case ProtectionState::NORMAL:
            return "NORMAL";

        case ProtectionState::OVER_VOLTAGE:
            return "OVER_VOLTAGE";

        case ProtectionState::UNDER_VOLTAGE:
            return "UNDER_VOLTAGE";

        case ProtectionState::OVER_TEMPERATURE:
            return "OVER_TEMPERATURE";

        case ProtectionState::UNDER_TEMPERATURE:
            return "UNDER_TEMPERATURE";

        case ProtectionState::OVER_CURRENT_CHARGE:
            return "OVER_CURRENT_CHARGE";

        case ProtectionState::OVER_CURRENT_DISCHARGE:
            return "OVER_CURRENT_DISCHARGE";

        case ProtectionState::SHORT_CIRCUIT:
            return "SHORT_CIRCUIT";

        case ProtectionState::SENSOR_FAILURE:
            return "SENSOR_FAILURE";

        case ProtectionState::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

void App::printTelemetry(const BatteryPack& pack)
{
    printf("\n");
    printf("=============== BMS TELEMETRY ===============\n");

    printf("PACK       : %.2f V\n", pack.totalVoltage);
    printf("AVG CELL   : %.3f V\n", pack.averageVoltage);
    printf("MIN CELL   : %.3f V\n", pack.minVoltage);
    printf("MAX CELL   : %.3f V\n", pack.maxVoltage);
    printf("DELTA      : %.0f mV\n", pack.deltaVoltage * 1000.0f);

    printf("\n");

    printf("CURRENT    : %.2f A\n", pack.current);
    printf("POWER      : %.2f W\n", pack.power);

    printf("\n");

    printf("TEMP       : %.1f °C\n", pack.averageTemperature);

    printf("\n");

    printf("SOC        : %u %%\n", pack.soc);
    printf("SOH        : %u %%\n", pack.soh);

    printf("\n");

    printf("CHARGING   : %s\n", pack.charging ? "YES" : "NO");
    printf("DISCHARGE  : %s\n", pack.discharging ? "YES" : "NO");
    printf("BALANCING  : %s\n", pack.balancing ? "ON" : "OFF");

    printf("\n");

    printf("=============================================\n\n");
}

bool App::init()
{
    logger_.init();

    logger_.info(
        EventSource::SYSTEM,
        1000,
        "Application Started"
    );

    battery_.init();

    logger_.info(
        EventSource::BATTERY,
        3000,
        "Battery Manager Ready"
    );

    protection_.init();

    logger_.info(
        EventSource::PROTECTION,
        2000,
        "Protection Manager Ready"
    );

    balance_.init();

    logger_.info(
        EventSource::SYSTEM,
        2500,
        "Balance Manager Ready"
    );

    temperature_.init();

    logger_.info(
        EventSource::SYSTEM,
        4000,
        "Temperature Manager Ready"
    );

    current_.init();

    logger_.info(
        EventSource::SYSTEM,
        5000,
        "Current Manager Ready"
    );

    // Imprime todos os logs de inicialização
    logger_.flush();

    return true;
}

void App::update()
{
    //-------------------------------------------------
    // Battery
    //-------------------------------------------------

    battery_.update();

    BatteryPack& pack = battery_.getPack();


    //-------------------------------------------------
    // Current
    //-------------------------------------------------

    current_.update(pack.totalVoltage);


    CurrentData currentData = current_.getData();

    battery_.setCurrent(currentData.currentA);
    // Mantém potência do pack consistente com a corrente atual.
    pack.current = currentData.currentA;
    pack.power = pack.totalVoltage * pack.current;



    //-------------------------------------------------
    // Temperature
    //-------------------------------------------------

    temperature_.update();

    TemperatureData temperatureData = temperature_.getData();

    // Atualiza temperaturas no estado compartilhado do BatteryPack
    // Temporariamente, caso ainda não exista temperatura média real no TemperatureData,
    // copiamos maxTemperature para todos os campos.
    pack.averageTemperature = temperatureData.averageTemperature;
    pack.maxTemperature = temperatureData.maxTemperature;
    pack.minTemperature = temperatureData.minTemperature;




    //-------------------------------------------------
    // Protection
    //-------------------------------------------------

    protection_.update(pack);

    //-------------------------------------------------
    // Balance
    //-------------------------------------------------

    balance_.update(
        pack,
        protection_,
        battery_
    );

    //-------------------------------------------------
    // Telemetria
    //-------------------------------------------------



    // Estados do sistema devem refletir o que os MOSFETs/flags realmente permitem.
    // Atualiza após protection_.update(pack);
    pack.charging = protection_.chargeEnabled();
    pack.discharging = protection_.dischargeEnabled();

    // Temporariamente, até BalanceManager expor isBalancing.
    pack.balancing = false;


    printTelemetry(pack);


    //-------------------------------------------------
    // Proteção
    //-------------------------------------------------

    char protectionLog[200];

    snprintf(
        protectionLog,
        sizeof(protectionLog),
        "PROTECTION %s | CHARGE MOSFET %s | DISCHARGE MOSFET %s",
        protectionStateToString(protection_.getState()),
        protection_.chargeEnabled() ? "ON" : "OFF",
        protection_.dischargeEnabled() ? "ON" : "OFF"
    );

    logger_.info(
        EventSource::PROTECTION,
        7001,
        protectionLog
    );

    //-------------------------------------------------
    // Publica todos os eventos pendentes
    //-------------------------------------------------

    logger_.flush();
}