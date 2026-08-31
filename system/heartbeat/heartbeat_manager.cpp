#include "heartbeat_manager.h"

#include <cstdio>

#include "../../system/clock/clock.h"

#include "../../communication/can/can_interface.h"
#include "../../communication/can/can_ids.h"
#include "../../communication/can/can_protocol.h"

#include "../../communication/ble/ble_notifications.h"

// Driver CAN global (definido no mock/driver real)
extern ICANDriver& can;

void HeartbeatManager::init()
{
    data_.alive = true;
    data_.state = "UNKNOWN";
    data_.uptimeMs = 0;
    data_.soc = 0;
    data_.temperatureC = 0.0f;

    lastBeatMs_ = 0;
}

void HeartbeatManager::update()
{
    const std::uint32_t now = static_cast<std::uint32_t>(Clock::millis());

    // Atualiza uptime continuamente
    data_.uptimeMs = now;

    // Emite heartbeat a cada 1000 ms
    if ((now - lastBeatMs_) >= HEARTBEAT_PERIOD_MS)
    {
        send();
    }
}

void HeartbeatManager::send()
{
    data_.alive = true;
    data_.uptimeMs = static_cast<std::uint32_t>(Clock::millis());

    sendCAN();
    sendBLE();
    sendUART();

    lastBeatMs_ = data_.uptimeMs;
}

void HeartbeatManager::sendCAN()
{
    // Frame CAN 0x100 (HEARTBEAT) com dados compactados:
    //   byte 0: alive (0xAA)
    //   byte 1: estado (mapeado para valor simples)
    //   byte 2: SOC (%)
    //   byte 3: temperatura (inteiro °C)
    //   bytes 4-7: uptime (uint32 little-endian)
    CANFrame frame;
    frame.id = CANID::HEARTBEAT;
    frame.dlc = 8;

    frame.data[0] = data_.alive ? 0xAA : 0x00;
    frame.data[1] = 0x01; // estado genérico (expansão futura: mapear enum)
    frame.data[2] = data_.soc;
    frame.data[3] = static_cast<std::uint8_t>(data_.temperatureC);

    const std::uint32_t uptime = data_.uptimeMs;
    frame.data[4] = static_cast<std::uint8_t>(uptime & 0xFFu);
    frame.data[5] = static_cast<std::uint8_t>((uptime >> 8) & 0xFFu);
    frame.data[6] = static_cast<std::uint8_t>((uptime >> 16) & 0xFFu);
    frame.data[7] = static_cast<std::uint8_t>((uptime >> 24) & 0xFFu);

    (void)can.send(frame);
}

void HeartbeatManager::sendBLE()
{
    // Hook para publicação BLE.
    // A arquitetura BLE atual é stub (BLEManager::publishTelemetry vazio).
    // Quando o serviço BLE real for implementado, publicar aqui a notificação
    // BLENotification::PACK_STATUS (ou uma nova HEARTBEAT) com os dados.
    //
    // Exemplo futuro:
    //   bleService.notify(BLENotification::PACK_STATUS, data_);
    (void)BLENotification::PACK_STATUS;
}

void HeartbeatManager::sendUART()
{
    // Dashboard / UART: linha formatada com todos os campos do heartbeat.
    printf(
        "[HEARTBEAT] ALIVE=%s STATE=%s UPTIME=%u ms SOC=%u%% TEMP=%.1f C\n",
        data_.alive ? "YES" : "NO",
        data_.state,
        static_cast<unsigned>(data_.uptimeMs),
        static_cast<unsigned>(data_.soc),
        data_.temperatureC);
}
