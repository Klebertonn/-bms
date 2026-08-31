#include "fault_protocol.h"

#include <cstdio>

#include "../core/fault/fault_registry.h"

/*
 * ==========================================================
 * FaultProtocol – Serialização de DTC para comunicação
 * ==========================================================
 * Formato texto (chave=valor), pronto para CAN/BLE/MQTT/Web.
 * Formato JSON, para MQTT/Dashboard Web.
 *
 * Esta camada (communication) é apenas CONSUMIDORA dos DTCs.
 * ==========================================================
 */

std::size_t FaultProtocol::encode(const FaultEvent& event, char* out, std::size_t outLen)
{
    if (out == nullptr || outLen == 0u)
    {
        return 0u;
    }

    const char* codeStr  = FaultRegistry::codeToString(event.code);
    const char* stateStr = faultStateToString(event.state);
    const char* sevStr   = faultSeverityToString(event.severity);

    const int written = std::snprintf(
        out, outLen,
        "DTC=%s;STATE=%s;SEV=%s;OCC=%lu;TS=%lu",
        codeStr, stateStr, sevStr,
        static_cast<unsigned long>(event.occurrence),
        static_cast<unsigned long>(event.timestamp));

    if (written < 0)
    {
        out[0] = '\0';
        return 0u;
    }

    return static_cast<std::size_t>(written);
}

std::size_t FaultProtocol::encodeJson(const FaultEvent& event, char* out, std::size_t outLen)
{
    if (out == nullptr || outLen == 0u)
    {
        return 0u;
    }

    const char* codeStr  = FaultRegistry::codeToString(event.code);
    const char* stateStr = faultStateToString(event.state);
    const char* sevStr   = faultSeverityToString(event.severity);

    const int written = std::snprintf(
        out, outLen,
        "{\"dtc\":\"%s\",\"state\":\"%s\",\"severity\":\"%s\","
        "\"occurrence\":%lu,\"timestampMs\":%lu}",
        codeStr, stateStr, sevStr,
        static_cast<unsigned long>(event.occurrence),
        static_cast<unsigned long>(event.timestamp));

    if (written < 0)
    {
        out[0] = '\0';
        return 0u;
    }

    return static_cast<std::size_t>(written);
}
