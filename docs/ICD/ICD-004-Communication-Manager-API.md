# ICD-004 – Communication Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-007

---

# 1. Objetivo

Este documento define a interface pública do Communication Manager, responsável por centralizar toda a comunicação entre o BMS e dispositivos externos.

A API deverá abstrair os detalhes de implementação de cada interface, permitindo que os demais módulos utilizem um único ponto de acesso.

---

# 2. Responsabilidades

O Communication Manager deverá:

* Inicializar todas as interfaces de comunicação.
* Enviar e receber mensagens.
* Gerenciar filas de transmissão e recepção.
* Padronizar formatos de mensagens.
* Notificar eventos importantes.
* Monitorar o estado das interfaces.

---

# 3. Interfaces Suportadas

Versão atual:

* UART (Serial)
* CLI
* Dashboard Serial
* CAN (TWAI)
* Bluetooth Low Energy (BLE)
* Display OLED

Versões futuras:

* Wi-Fi
* MQTT
* Ethernet
* Modbus RTU
* Modbus TCP
* USB CDC

---

# 4. Enumerações

## CommunicationInterface

```cpp id="comm001"
enum class CommunicationInterface
{
    UART,
    CLI,
    CAN,
    BLE,
    OLED
};
```

---

## CommunicationState

```cpp id="comm002"
enum class CommunicationState
{
    STOPPED,
    INITIALIZING,
    READY,
    BUSY,
    ERROR
};
```

---

## MessagePriority

```cpp id="comm003"
enum class MessagePriority
{
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};
```

---

# 5. Estruturas de Dados

## CommunicationMessage

```cpp id="comm004"
struct CommunicationMessage
{
    uint32_t id;
    uint32_t timestamp;

    CommunicationInterface interface;

    MessagePriority priority;

    uint16_t length;

    uint8_t payload[256];
};
```

---

## CommunicationStatus

```cpp id="comm005"
struct CommunicationStatus
{
    CommunicationState state;

    bool uartConnected;
    bool canConnected;
    bool bleConnected;
    bool oledAvailable;

    uint32_t txMessages;
    uint32_t rxMessages;

    uint32_t txErrors;
    uint32_t rxErrors;
};
```

---

# 6. Interface Pública

## Inicialização

```cpp id="comm006"
void communicationInit();
```

---

## Atualização

```cpp id="comm007"
void communicationUpdate();
```

Processa filas de transmissão e recepção.

---

## Enviar Mensagem

```cpp id="comm008"
bool communicationSend(const CommunicationMessage& message);
```

---

## Receber Mensagem

```cpp id="comm009"
bool communicationReceive(CommunicationMessage& message);
```

---

## Obter Estado

```cpp id="comm010"
CommunicationState communicationGetState();
```

---

## Obter Status

```cpp id="comm011"
const CommunicationStatus& communicationGetStatus();
```

---

## Verificar Interface

```cpp id="comm012"
bool communicationIsConnected(CommunicationInterface interface);
```

---

## Reinicializar Interface

```cpp id="comm013"
bool communicationRestart(CommunicationInterface interface);
```

---

## Executar Autoteste

```cpp id="comm014"
bool communicationSelfTest();
```

---

# 7. Fluxo de Operação

```text id="comm015"
Inicialização
      │
      ▼
Configuração das interfaces
      │
      ▼
Recepção de mensagens
      │
      ▼
Fila de processamento
      │
      ▼
Módulos internos
      │
      ▼
Fila de transmissão
      │
      ▼
Envio para UART / CAN / BLE / OLED
```

---

# 8. Integração

Recebe dados de:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Logger.
* Task Manager.

Disponibiliza informações para:

* Aplicativos BLE.
* Rede CAN.
* Terminal Serial.
* Display OLED.
* Ferramentas de diagnóstico.

---

# 9. Tratamento de Erros

Em caso de erro:

* Registrar no Logger.
* Incrementar contadores de erro.
* Tentar reconexão automática.
* Informar o Fault Manager, se necessário.

---

# 10. Requisitos

* Interface thread-safe.
* Comunicação não bloqueante.
* Compatível com FreeRTOS.
* Suporte a filas de mensagens.
* Baixa latência.

---

# 11. Critérios de Aceitação

A API será considerada estável quando:

* Todas as interfaces forem inicializadas corretamente.
* Mensagens forem transmitidas e recebidas sem perdas.
* Reconexões funcionarem conforme especificado.
* O sistema operar em Mock, Wokwi e ESP32 físico.

---

# 12. Evoluções Futuras

A API deverá permitir:

* Novos protocolos de comunicação.
* Criptografia de mensagens.
* Compressão de dados.
* Atualização OTA.
* Sincronização com serviços em nuvem.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                           |
| ------ | ---------- | --------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Primeira definição da API do Communication Manager. |

