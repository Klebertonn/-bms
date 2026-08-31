# Arquitetura do Firmware BMS

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Produção

---

## Visão Geral

O firmware segue uma **arquitetura em camadas**, com separação clara entre:

1. **Aplicação** — orquestra a lógica de negócio (`App`).
2. **Domínio (`core/`)** — lógica de negócio independente de hardware.
3. **Infraestrutura (`system/`)** — serviços de sistema (clock, logger, storage, RTOS, scheduler).
4. **Comunicação (`communication/`)** — CAN e BLE.
5. **HAL (`lib/hal/` + `src/hal/`)** — abstração de hardware e drivers.

```
┌─────────────────────────────────────────────────────────────┐
│                        APLICAÇÃO (App)                      │
│  src/app/app.cpp  ·  src/main.cpp (native)  ·  main_esp32   │
└───────────────┬─────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────┐
│                       DOMÍNIO (core/)                       │
│  battery  current  temperature  protection  fault  balance  │
│  soc  state (máq. estados + self test)  mosfet              │
└───────────────┬─────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────┐
│                  INFRAESTRUTURA (system/)                   │
│  clock  logger  storage  rtos  scheduler  heartbeat         │
│  ·        ·        ·        ·       ·          ·            │
└───────────────┬─────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────┐
│                COMUNICAÇÃO (communication/)                 │
│              CAN (TWAI/MCP2515) · BLE (GATT)                │
└───────────────┬─────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────┐
│                    HAL (lib/hal/ + src/hal/)                │
│  adc  current  temperature  soc  display  gpio  mosfet      │
│  mocks (native) · drivers ESP32 (esp32dev)                  │
└─────────────────────────────────────────────────────────────┘
```

---

## Fluxo de Inicialização

```
BOOT → INIT → SELF_TEST → READY (ou FAULT)
```

1. **BOOT** — inicialização do hardware (HAL).
2. **INIT** — inicialização dos módulos de domínio e infraestrutura.
3. **SELF_TEST** — execução do Power-On Self Test (POST):
   - ADC, EEPROM, FLASH, NVS, Current Sensor, Temperature Sensor, MOSFET Driver, RAM.
   - **PASS** → `READY`.
   - **FAIL** → `FAULT`.
4. **READY** — sistema operacional, aguardando carga/descarga.

---

## Fluxo de Dados (Loop Principal)

```
sensores (ADC/I²C)
   → App.update()
      → BatteryManager (tensões, agregados, SOC)
      → CurrentManager (corrente)
      → TemperatureManager (temperaturas)
      → FaultManager (detecção de falhas)
      → ProtectionManager (proteções)
      → BmsStateMachine → BmsStateManager (estado)
      → MosfetController (ação sobre MOSFETs)
      → BalanceManager (balanceamento)
   → Telemetria (print/display/CAN/BLE)
```

---

## Responsabilidades por Camada

### core/ (Domínio)
| Módulo | Responsabilidade |
|--------|------------------|
| `battery/` | Modelo e agregação do pack (células, tensões, SOC). |
| `current/` | Medição e filtragem de corrente. |
| `temperature/` | Medição e filtragem de temperatura. |
| `protection/` | Lógica de proteções (over/under voltage, current, temp). |
| `fault/` | Detecção, gerenciamento e histórico de falhas. |
| `balance/` | Algoritmo de balanceamento passivo. |
| `soc/` | Estimativa de estado de carga. |
| `state/` | Máquina de estados BMS + Self Test (POST). |
| `mosfet/` | Controle de MOSFETs (lógica). |

### system/ (Infraestrutura)
| Módulo | Responsabilidade |
|--------|------------------|
| `clock/` | Relógio (millis). |
| `logger/` | Log estruturado e eventos. |
| `storage/` | Persistência (NVS, EEPROM, FLASH, arquivo). |
| `rtos/` | Abstrações FreeRTOS (task, queue, mutex, event, watchdog). |
| `scheduler/` | Agendamento de tarefas periódicas. |
| `heartbeat/` | Supervisão de heartbeat. |

### communication/
| Módulo | Responsabilidade |
|--------|------------------|
| `can/` | Protocolo CAN, gerenciamento e drivers (TWAI/MCP2515/mock). |
| `ble/` | Serviço BLE GATT, comandos, notificações, segurança. |

### lib/hal/ + src/hal/
| Módulo | Responsabilidade |
|--------|------------------|
| `adc` | Leitura de tensão das células. |
| `current` | Leitura de corrente (INA219/INA226/Hall). |
| `temperature` | Leitura de temperatura (NTC). |
| `soc` | Leitura de SOC (MAX17048). |
| `display` | Display OLED SSD1306. |
| `gpio` | Saídas digitais (MOSFETs, relé). |
| `mosfet` | Driver de MOSFET. |

---

## Principais Fluxos

### Detecção de Falha
1. `FaultManager::evaluate(pack)` — analisa tensões, correntes e temperaturas.
2. Gera flag de falha (`FaultFlag`) e detalhes (`FaultInfo`).
3. `ProtectionManager` reage à falha.
4. `BmsStateManager` transita para `FAULT` se crítico.
5. `MosfetController` aplica ação segura.

### Persistência de Falha
1. `FaultStorage` carrega o histórico no boot.
2. Ao detectar uma falha **nova** (edge detection), `FaultHistory` registra.
3. `FaultStorage::append()` persiste o evento.

---

## Decisões de Design

- **HAL** isola o hardware da lógica; a troca de sensores não altera o `core/`.
- **Dependency Injection** no MOSFET (`IMosfetDriver*`) permite usar mock (native) ou driver real (ESP32).
- **Build por target** (`native`/`esp32dev`) via `build_src_filter` no `platformio.ini`.
- **Self Test** integrado à máquina de estados garante que o hardware funcione antes do `READY`.

---

## Referências

- [Visão Geral (SRS-001)](SRS/SRS-001-Visao-Geral.md)
- [Arquitetura (SRS-002)](SRS/SRS-002-Arquitetura.md)
- [Hardware (SRS-003)](SRS/SRS-003-Hardware.md)
- [DDS de Arquitetura](DDS/DDS-001-Arquitetura-do-Firmware.md)
- [HAL (DDS-002)](DDS/DDS-002-Hardware-Abstraction-Layer.md)
