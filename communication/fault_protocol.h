#pragma once

#include <cstddef>
#include <cstdint>

#include "../core/fault/fault_event.h"

/*
 * ==========================================================
 * FaultProtocol – Serialização de DTC para comunicação
 * ==========================================================
 * Converte um FaultEvent em um formato serializado (texto/JSON)
 * para transporte em CAN, BLE, MQTT ou Dashboard Web.
 *
 * Esta camada (communication) é CONSUMIDORA dos DTCs.
 * O FaultManager (core) NÃO conhece este módulo.
 * ==========================================================
 */

class FaultProtocol
{
public:
    /* Tamanho máximo de um payload serializado. */
    static constexpr std::size_t PAYLOAD_MAX = 128u;

    /*
     * Serializa um FaultEvent em texto (formato chave=valor),
     * pronto para telemetria/BLE/MQTT.
     * Retorna o número de caracteres escritos (sem o '\0').
     */
    static std::size_t encode(const FaultEvent& event, char* out, std::size_t outLen);

    /*
     * Serializa em formato JSON (para MQTT/Dashboard Web).
     * Retorna o número de caracteres escritos (sem o '\0').
     */
    static std::size_t encodeJson(const FaultEvent& event, char* out, std::size_t outLen);
};
