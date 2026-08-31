# ICD-005 – Hardware Abstraction Layer (HAL) API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-002

---

# 1. Objetivo

A Hardware Abstraction Layer (HAL) fornece uma interface única para acesso ao hardware, isolando os módulos da aplicação dos detalhes específicos da plataforma.

Com esta arquitetura, o mesmo firmware poderá ser executado em:

* PC (Mock)
* Wokwi
* ESP32 físico

Sem alterações na lógica da aplicação.

---

# 2. Responsabilidades

A HAL deverá:

* Inicializar dispositivos.
* Ler sensores.
* Controlar GPIOs.
* Controlar MOSFETs.
* Controlar PWM.
* Controlar ADC.
* Controlar I²C.
* Controlar SPI.
* Controlar UART.
* Controlar CAN.
* Controlar BLE.
* Controlar temporizadores.

---

# 3. Organização

```text id="hal001"
Application
      │
      ▼
Managers
      │
      ▼
HAL API
      │
 ┌────┴───────────────┐
 │                    │
 ▼                    ▼
Mock HAL         Hardware HAL
                      │
                      ▼
                 ESP32 Drivers
```

---

# 4. Estruturas

## HALStatus

```cpp id="hal002"
enum class HALStatus
{
    OK,
    ERROR,
    NOT_INITIALIZED,
    TIMEOUT,
    UNSUPPORTED
};
```

---

## HALPlatform

```cpp id="hal003"
enum class HALPlatform
{
    MOCK,
    WOKWI,
    ESP32
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="hal004"
HALStatus halInit();
```

---

## Plataforma Atual

```cpp id="hal005"
HALPlatform halGetPlatform();
```

---

## Atualização

```cpp id="hal006"
void halUpdate();
```

---

# 6. ADC

```cpp id="hal007"
float halReadCellVoltage(uint8_t cell);
```

```cpp id="hal008"
float halReadPackVoltage();
```

```cpp id="hal009"
float halReadPackCurrent();
```

---

# 7. Temperatura

```cpp id="hal010"
float halReadTemperature(uint8_t sensor);
```

---

# 8. MOSFETs

```cpp id="hal011"
void halSetChargeMOSFET(bool enabled);
```

```cpp id="hal012"
void halSetDischargeMOSFET(bool enabled);
```

```cpp id="hal013"
void halSetBalanceMOSFET(uint8_t cell, bool enabled);
```

---

# 9. Comunicação

## UART

```cpp id="hal014"
HALStatus halSerialWrite(const char* text);
```

---

## CAN

```cpp id="hal015"
HALStatus halCANSend(const uint8_t* data, uint16_t size);
```

---

## BLE

```cpp id="hal016"
HALStatus halBLEUpdate();
```

---

# 10. Display

```cpp id="hal017"
HALStatus halDisplayWrite(const char* text);
```

---

# 11. Temporização

```cpp id="hal018"
uint32_t halMillis();
```

```cpp id="hal019"
void halDelay(uint32_t ms);
```

---

# 12. Armazenamento

```cpp id="hal020"
HALStatus halStorageRead(void* buffer, uint32_t size);
```

```cpp id="hal021"
HALStatus halStorageWrite(const void* buffer, uint32_t size);
```

---

# 13. Fluxo de Operação

```text id="hal022"
Managers
      │
      ▼
HAL API
      │
      ▼
Mock / Wokwi / ESP32
      │
      ▼
Hardware
```

---

# 14. Implementações

O projeto deverá possuir implementações separadas para:

* `hal_mock.cpp`
* `hal_wokwi.cpp`
* `hal_esp32.cpp`

Todas deverão implementar exatamente a mesma interface.

---

# 15. Tratamento de Erros

Toda função deverá retornar um estado (`HALStatus`) ou documentar claramente sua política de tratamento de erros.

Falhas críticas deverão:

* Ser registradas no Logger.
* Notificar o Fault Manager.
* Permitir recuperação quando possível.

---

# 16. Requisitos

* Interface única.
* Independência de plataforma.
* Compatibilidade com FreeRTOS.
* Baixa latência.
* Fácil substituição de drivers.

---

# 17. Critérios de Aceitação

A HAL será considerada estável quando:

* O firmware executar sem alterações em Mock, Wokwi e ESP32.
* Todos os drivers respeitarem a interface definida.
* Os testes unitários e de integração forem aprovados.

---

# 18. Evoluções Futuras

A arquitetura deverá permitir suporte a:

* ESP32-S3.
* ESP32-C6.
* STM32.
* RP2040.
* Linux embarcado.
* Simuladores HIL (Hardware-in-the-Loop).

---

# 19. Controle de Versão

| Versão | Data       | Descrição                         |
| ------ | ---------- | --------------------------------- |
| 1.0.0  | 03/07/2026 | Primeira definição da API da HAL. |

