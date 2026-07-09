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

    PackData pack = battery_.getPackData();

    //-------------------------------------------------
    // Current
    //-------------------------------------------------

    current_.update(pack.totalVoltage);

    CurrentData currentData = current_.getData();

    pack.current = currentData.currentA;

    //-------------------------------------------------
    // Temperature
    //-------------------------------------------------

    temperature_.update();

    TemperatureData temperatureData = temperature_.getData();

    pack.temperature = temperatureData.maxTemperature;

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

    char telemetry[200];

    snprintf(
        telemetry,
        sizeof(telemetry),
        "PACK %.2fV | MIN %.2fV | MAX %.2fV | CURRENT %.2fA | TEMP %.2fC | SOC %d%%",
        pack.totalVoltage,
        pack.minVoltage,
        pack.maxVoltage,
        pack.current,
        pack.temperature,
        pack.soc
    );

    logger_.info(
        EventSource::SYSTEM,
        7000,
        telemetry
    );

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