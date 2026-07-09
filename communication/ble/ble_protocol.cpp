#include "ble_protocol.h"

BLECommandPacket BLEProtocol::decode(const uint8_t* data, uint32_t len)
{
    (void)data;
    (void)len;

    // Stub: implementação real depende do formato definido no seu protocolo.
    return BLECommandPacket{};
}

