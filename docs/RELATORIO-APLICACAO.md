# Relatório Geral da Aplicação — BMS (Battery Management System)

**Projeto:** Firmware BMS
**Versão:** 1.0.0
**Status:** Produção
**Linguagem:** C++ (C++11+)
**Plataformas:** PlatformIO (ESP32 + Native/PC simulada)
**Licença:** MIT

---

## 1. Visão Geral

Este projeto implementa o firmware de controle e proteção de um **pacote de baterias (BMS — Battery Management System)** em **C++**, com arquitetura modular em camadas. O sistema é desenvolvido para **ESP32** (hardware real) e também roda em modo **simulação** (`native`/mocks) no PC, facilitando o desenvolvimento e os testes sem hardware físico.

O foco principal é **segurança, diagnóstico e comunicação industrial**, com proteções de tensão, corrente, temperatura e curto-circuito, gerando **DTCs (Diagnostic Trouble Codes)** persistidos e centralizados em um histórico de falhas.

---

## 2. Funcionalidades Principais

- **State Machine** — gerenciamento de estados do BMS: `BOOT`, `INIT`, `SELF_TEST`, `READY`, `IDLE`, `CHARGING`, `DISCHARGING`, `BALANCING`, `FAULT`, `RECOVERY`, `SHUTDOWN`.
- **Self Test (POST)** — validação de hardware no boot (ADC, EEPROM, FLASH, NVS, sensor de corrente, sensor de temperatura, driver de MOSFET, RAM).
- **Fault Manager** — detecção e gerenciamento de falhas com API DTC.
- **Fault History** — histórico persistido de eventos de falha.
- **Protection Manager** — proteções de sobretensão, subtensão, sobrecorrente, sobre/subtemperatura e curto-circuito.
- **SOC** — estimativa de Estado de Carga (inclui coulomb counter).
- **Temperature Manager** — monitoramento e filtragem de temperatura.
- **Current Manager** — monitoramento e filtragem de corrente.
- **Balance Manager** — balanceamento passivo de células.
- **MOSFET Controller** — controle de MOSFETs de carga/descarga/balanço.
- **Logger & Diagnostics** — log estruturado e eventos.
- **Storage** — persistência (NVS, EEPROM, FLASH, arquivo em native).
- **Scheduler & Heartbeat** — agendamento de tarefas e supervisão de firmware.
- **CAN** — comunicação via TWAI (ESP32) ou MCP2515.
- **BLE** — serviço GATT de monitoramento.
- **Web Dashboard** — visualização de telemetria em tempo real.

---

## 3. Arquitetura em Camadas

A aplicação segue uma **arquitetura em camadas** com separação clara de responsabilidades:

```
┌─────────────────────────────────────────────────────────────┐
│                        APLICAÇÃO (App)                      │
│  src/app/app.cpp  ·  src/main.cpp (native)  ·  main_esp32   │
└───────────────┬─────────────────────────────────────────────┘
┌───────────────▼─────────────────────────────────────────────┐
│                       DOMÍNIO (core/)                       │
│  battery  current  temperature  protection  fault  balance  │
│  soc  state (máq. estados + self test)  mosfet              │
└───────────────┬─────────────────────────────────────────────┘
┌───────────────▼─────────────────────────────────────────────┐
│                  INFRAESTRUTURA (system/)                   │
│  clock  logger  storage  rtos  scheduler  heartbeat         │
└───────────────┬─────────────────────────────────────────────┘
┌───────────────▼─────────────────────────────────────────────┐
│                COMUNICAÇÃO (communication/)                 │
│              CAN (TWAI/MCP2515) · BLE (GATT)                │
└───────────────┬─────────────────────────────────────────────┘
┌───────────────▼─────────────────────────────────────────────┐
│                    HAL (lib/hal/ + src/hal/)                │
│  adc  current  temperature  soc  display  gpio  mosfet      │
│  mocks (native) · drivers ESP32 (esp32dev)                  │
└─────────────────────────────────────────────────────────────┘
```

### 3.1 Responsabilidades por Camada

| Camada | Diretório | Módulos | Responsabilidade |
|--------|-----------|---------|------------------|
| **Aplicação** | `src/app/` | `App`, `main.cpp`, `main_esp32.cpp`, `main_scheduler_demo.cpp` | Orquestra a lógica de negócio, telemetria e injeção de dependências. |
| **Domínio** | `core/` | `battery`, `current`, `temperature`, `protection`, `fault`, `balance`, `soc`, `state`, `mosfet` | Lógica de negócio independente de hardware. |
| **Infraestrutura** | `system/` | `clock`, `logger`, `storage`, `rtos`, `scheduler`, `heartbeat` | Serviços de sistema (relógio, log, persistência, RTOS, agendamento). |
| **Comunicação** | `communication/` | `can`, `ble`, `fault_protocol` | CAN (TWAI/MCP2515) e BLE (GATT). |
| **HAL** | `lib/hal/` + `src/hal/` | `adc`, `current`, `temperature`, `soc`, `display`, `gpio`, `mosfet` | Abstração de hardware e drivers (mocks e drivers ESP32). |

---

## 4. Estrutura do Repositório

```
.
├── core/               # Lógica de negócio (battery, current, temp, protection, fault, balance, soc, state, mosfet)
├── system/             # Infraestrutura (clock, logger, storage, rtos, scheduler, heartbeat)
├── communication/      # CAN e BLE + fault_protocol
├── lib/                # Interfaces HAL e bibliotecas auxiliares
├── src/                # Implementação (app, main, mocks, drivers ESP32)
│   ├── app/            # Camada de aplicação
│   ├── hal/            # Mocks (native) e drivers (esp32)
│   ├── main.cpp        # Ponto de entrada native (PC)
│   ├── main_esp32.cpp  # Ponto de entrada ESP32 (hardware real)
│   └── main_scheduler_demo.cpp  # Demo do scheduler
├── config/             # Configuração do sistema (config.h)
├── docs/               # Documentação (SRS, DDS, ICD, arquitetura, produção)
│   ├── SRS/            # Requisitos de software (10 documentos)
│   ├── DDS/            # Design detalhado (10 documentos)
│   ├── ICD/            # Interfaces (23 documentos)
│   └── diagrams/       # Diagramas
├── tests/              # Testes unitários e de integração (Unity)
├── test/native/        # Testes nativos (test_fault, test_dtc, test_self_test, test_storage)
├── scripts/            # Scripts de automação (geração de relatório)
├── web/                # Dashboard web (telemetria)
├── hardware/           # Artefatos de hardware (esquemas, pinagem, datasheets)
├── firmware/           # Mapeamento lógico do firmware
├── simulation/         # Mapeamento lógico da simulação/mocks
├── tools/              # Ferramentas auxiliares
├── config/config.h     # Constantes de configuração
├── platformio.ini      # Configuração do PlatformIO
├── MIGRATION.md        # Plano de migração da reorganização
└── TODO.md             # Status do desenvolvimento
```

> **Nota de reorganização:** para não quebrar includes e o build, o código-fonte
> (`core/`, `system/`, `communication/`, `lib/`, `src/`, `config/`) permanece
> fisicamente na raiz. As pastas `firmware/`, `simulation/`, `hardware/`, `tests/`,
> `scripts/` e `tools/` organizam a estrutura lógica. Veja `MIGRATION.md`.

---

## 5. Fluxo de Inicialização

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

No código de exemplo (`src/main.cpp`), o loop `App::init()` chama `bmsStateMachine_.update()` três vezes para avançar de `BOOT → INIT → SELF_TEST → READY`.

---

## 6. Fluxo de Dados (Loop Principal)

```
sensores (ADC/I²C)
   → App.update()
      → BatteryManager (tensões, agregados, SOC)
      → CurrentManager (corrente)
      → TemperatureManager (temperaturas)
      → ProtectionManager (proteções → gera DTCs)
      → FaultManager (registro de falhas DTC)
      → BmsStateMachine → BmsStateManager (estado)
      → MosfetController (ação sobre MOSFETs)
      → BalanceManager (balanceamento)
      → Heartbeat (supervisão)
   → Telemetria (print/display/CAN/BLE)
```

### 6.1 Detecção e Persistência de Falha

1. `ProtectionManager::update(pack, fault)` — fonte principal de detecção (`raiseFault`/`clearFault`).
2. `FaultManager` cria um `FaultEvent` (código, severidade, estado, timestamp, ocorrência).
3. `FaultStorageSinkAdapter` persiste o evento DTC no `FaultStorage`.
4. `FaultHistory` registra e mantém o histórico em RAM.
5. `BmsStateManager` transita para `FAULT` se crítico.
6. `MosfetController` aplica ação segura (desliga MOSFETs).

> **Nota — DTC-02:** o `fault_.evaluate(pack)` foi desabilitado como mecanismo primário
> (LEGACY COMPATIBILITY ONLY). A detecção agora é centralizada no `ProtectionManager`,
> que chama `fault_.raiseFault()`/`clearFault()` para gerar os DTCs, evitando duplicação.

---

## 7. Módulos do Domínio (core/)

| Módulo | Arquivos | Responsabilidade |
|--------|----------|------------------|
| **battery/** | `battery_manager`, `battery_cell`, `battery_pack` | Modelo e agregação do pack (células, tensões, corrente, SOC, SOH). |
| **current/** | `current_manager`, `current_filter`, `current_types` | Medição e filtragem de corrente (filtro). |
| **temperature/** | `temperature_manager`, `temperature_filter`, `temperature_types` | Medição e filtragem de temperatura (NTC). |
| **protection/** | `protection_manager`, `protection_types` | Lógica de proteções (over/under voltage, current, temp, short circuit). |
| **fault/** | `fault_manager`, `fault_registry`, `fault_history`, `fault_reason`, `fault_code`, `fault_flags`, `fault_event`, `fault_sink` | Detecção, gerenciamento e histórico de falhas (DTC). |
| **balance/** | `balance_manager`, `balance_types` | Algoritmo de balanceamento passivo. |
| **soc/** | `soc_manager`, `coulomb_counter`, `soc_types` | Estimativa de estado de carga. |
| **state/** | `bms_state_machine`, `bms_state_manager`, `self_test` | Máquina de estados BMS + Self Test (POST). |
| **mosfet/** | `mosfet_controller`, `mosfet_mock` | Controle de MOSFETs (lógica). |

---

## 8. Módulos de Infraestrutura (system/)

| Módulo | Arquivos | Responsabilidade |
|--------|----------|------------------|
| **clock/** | `clock` | Relógio baseado em `millis()`. |
| **logger/** | `logger`, `event_queue`, `event_types`, `event`, `storage_backend`, `log_formatter` | Log estruturado e eventos. |
| **storage/** | `storage_manager`, `storage_crc`, `storage_interface`, `storage_types`, `fault_storage`, `eeprom_driver`, `flash_driver`, `nvs_driver`, `migration_manager` | Persistência (NVS, EEPROM, FLASH, arquivo). |
| **rtos/** | `task_manager`, `queue_manager`, `mutex_manager`, `event_manager`, `watchdog_manager`, `task_priorities`, `task_definitions` | Abstrações FreeRTOS. |
| **scheduler/** | `scheduler`, `scheduler_tasks`, `task` | Agendamento de tarefas periódicas. |
| **heartbeat/** | `heartbeat_manager` | Supervisão de firmware (pulso). |

---

## 9. Comunicação (communication/)

| Módulo | Arquivos | Responsabilidade |
|--------|----------|------------------|
| **can/** | `can_manager`, `can_protocol`, `can_frame`, `can_ids`, `can_filters`, `can_interface`, `twai_driver`, `mcp2515_driver`, `can_mock` | Protocolo CAN, gerenciamento e drivers (TWAI/MCP2515/mock). |
| **ble/** | `ble_manager`, `ble_service`, `ble_protocol`, `ble_commands`, `ble_notifications`, `ble_security` | Serviço BLE GATT, comandos, notificações, segurança. |
| **fault_protocol** | `fault_protocol` | Protocolo de transmissão de falhas. |

---

## 10. HAL (lib/hal/ + src/hal/)

| Módulo | Interface | Sensores/Drivers |
|--------|-----------|------------------|
| **adc** | `adc_interface.h` | ADC interno, ADS1115; mock em `src/hal/adc/mock_adc.cpp`. |
| **current** | `current_interface.h` | INA219, INA226, Hall; mock e driver INA219 ESP32. |
| **temperature** | `temperature_interface.h` | NTC 10k/100k; mock e drivers. |
| **soc** | `soc_sensor_interface.h` | MAX17048; driver ESP32. |
| **display** | — | OLED SSD1306 (I²C). |
| **gpio** | `mosfet_driver_interface.h` | Saídas digitais (MOSFETs, relé). |
| **mosfet** | — | Driver de MOSFET; mock e driver ESP32 (`hal_mosfet_controller`). |

---

## 11. Configuração (config/config.h)

Constantes principais do sistema:

### Parâmetros do Pack
| Parâmetro | Valor |
|-----------|-------|
| `PACK_CELL_COUNT` | 3 células |
| `MAX_CELL_VOLTAGE` | 4.20 V |
| `MIN_CELL_VOLTAGE` | 3.00 V |
| `CELL_OVERVOLTAGE_LIMIT` | 4.25 V |
| `CELL_UNDERVOLTAGE_LIMIT` | 3.00 V |
| `CELL_NOMINAL_VOLTAGE` | 3.70 V |

### Limites de Temperatura
| Parâmetro | Valor |
|-----------|-------|
| `MAX_TEMPERATURE` | 60.0 °C |
| `MIN_TEMPERATURE` | -20.0 °C |
| `MAX_BALANCE_TEMP` | 45.0 °C |

### Limites de Corrente
| Parâmetro | Valor |
|-----------|-------|
| `MAX_CHARGE_CURRENT` | 30.0 A |
| `MAX_DISCHARGE_CURRENT` | 30.0 A |

### Balanceamento
| Parâmetro | Valor |
|-----------|-------|
| `BALANCE_START_DELTA` | 20 mV |
| `BALANCE_STOP_DELTA` | 10 mV |
| `MAX_BALANCE_TIME_MS` | 600000 ms |

### Períodos de Atualização
| Parâmetro | Valor |
|-----------|-------|
| `MAIN_LOOP_PERIOD_MS` | 100 ms |
| `LOGGER_PERIOD_MS` | 1000 ms |
| `TELEMETRY_PERIOD_MS` | 500 ms |

### SOC
| Parâmetro | Valor |
|-----------|-------|
| `PACK_FULL_VOLTAGE` | 12.60 V |
| `PACK_EMPTY_VOLTAGE` | 9.00 V |

### Hardware (ESP32)
| Parâmetro | Valor |
|-----------|-------|
| `HW_I2C_SDA_PIN` | 21 |
| `HW_I2C_SCL_PIN` | 22 |
| `HW_MOSFET_CHARGE_PIN` | 26 |
| `HW_MOSFET_DISCHARGE_PIN` | 27 |
| `HW_MOSFET_BALANCE_PIN` | 14 |
| `HW_RELAY_PIN` | 32 |
| `HW_CAN_TX_PIN` / `HW_CAN_RX_PIN` | 5 / 4 |
| `HW_NTC_ADC_CHANNEL` | 0 (GPIO 36) |
| `HW_OLED_I2C_ADDR` | 0x3C |
| `HW_INA219_I2C_ADDR` | 0x40 |
| `HW_MAX17048_I2C_ADDR` | 0x36 |
| `HW_INA219_SHUNT_OHM` | 0.001 Ω |
| `HW_INA219_MAX_CURRENT_A` | 30.0 A |

---

## 12. Sistema de Falhas (DTC)

### 12.1 FaultManager (API DTC)
- `raiseFault(FaultCode)` — registra uma falha (cria evento, loga, persiste, atualiza histórico).
- `raiseFault(code, severity, state)` — com severidade/estado personalizados.
- `raiseFault(code, severity, state, source, measuredValue, limit)` — com contexto diagnóstico completo.
- `clearFault(FaultCode)` — limpa falha ativa (marca `CLEARED`).
- `hasFault()` / `hasFault(code)` — verifica falhas ativas.
- `getActiveFaults()` / `getActiveFault(index, out)` — obtém eventos ativos.
- `setLogSink()` / `setStorageSink()` — injeção de dependência (Clean Architecture / SOLID / DI).
- Capacidade máxima: **16 falhas ativas** (`MAX_ACTIVE_FAULTS`).

### 12.2 Códigos de Falha (FaultRegistry)
Exemplos de códigos e severidades:
| Código (FaultCode) | Descrição | Severidade |
|--------------------|-----------|------------|
| `BMS_CELL_OVERVOLTAGE` | Cell over voltage | CRITICAL |
| `BMS_CELL_UNDERVOLTAGE` | Cell under voltage | CRITICAL |
| `BMS_OVER_TEMPERATURE` | Over temperature | CRITICAL |
| `BMS_LOW_TEMPERATURE` | Low temperature | WARNING |
| `BMS_OVER_CURRENT` | Over current | CRITICAL |
| `BMS_SHORT_CIRCUIT` | Short circuit | FATAL |
| `BMS_STORAGE_FAILURE` | Storage failure | ERROR |
| `BMS_SENSOR_FAILURE` | Sensor failure | ERROR |
| `BMS_CAN_FAILURE` | CAN failure | ERROR |
| `BMS_BAT_OVERVOLTAGE` | Battery over voltage | — |

### 12.3 ProtectionState
`NORMAL`, `OVER_VOLTAGE`, `UNDER_VOLTAGE`, `OVER_TEMPERATURE`, `UNDER_TEMPERATURE`, `OVER_CURRENT_CHARGE`, `OVER_CURRENT_DISCHARGE`, `SHORT_CIRCUIT`, `SENSOR_FAILURE`, `UNKNOWN`.

---

## 13. Web Dashboard (web/)

Dashboard web responsivo de monitoramento do BMS.

### Funcionalidades
- **SOC** — State of Charge com barra de progresso e indicador de saúde.
- **SOH** — State of Health.
- **Pack** — Tensão total, delta entre células, corrente e potência.
- **Temperatura** — Média, mínima e máxima.
- **Fault** — Falha ativa (nome, código, célula, valor, limite).
- **Histórico** — Tabela de eventos de falha.
- **Estado** — Badge com estado do BMS.
- **Heartbeat** — Indicador de firmware vivo (pulso).
- **Gráficos** — Tensão, corrente, SOC e temperatura em tempo real (Chart.js).

### Estrutura
```
web/
├── index.html          # Página principal
├── css/
│   └── style.css       # Estilos responsivos (dark theme)
└── js/
    ├── data-source.js  # Fonte de dados (demo ou API real)
    ├── charts.js       # Gráficos (Chart.js)
    └── dashboard.js    # Lógica de renderização
```

### Modos de Operação
- **Demo (padrão):** abrir `index.html` no navegador; dados simulados.
- **Produção (hardware real):** alterar `BMS_DATA_SOURCE` para `'api'` e configurar `BMS_API_URL` (ex.: `http://192.168.4.1/api/bms`).

### Tecnologias
- HTML5 / CSS3 (CSS Grid + Flexbox, responsivo).
- JavaScript (vanilla ES6+).
- Chart.js 4.x (CDN).

---

## 14. Testes

Os testes usam o framework **Unity** e estão distribuídos entre `tests/` e `test/native/`.

### Testes Existantes
| Arquivo | Foco |
|---------|------|
| `test/test_unit_scheduler.cpp` | Scheduler |
| `test/test_unit_state_machine.cpp` | Máquina de estados |
| `test/test_unit_can.cpp` | CAN |
| `test/test_unit_ble.cpp` | BLE |
| `test/test_unit_self_test.cpp` | Self Test |
| `test/test_unit_eeprom_flash.cpp` | EEPROM/FLASH |
| `test/test_integration.cpp` | Integração |
| `test/native/test_fault.cpp` | FaultManager (API legada) |
| `test/native/test_fault/test_dtc.cpp` | FaultManager (API DTC + Sprint DTC-02) |
| `test/native/test_self_test.cpp` | Self Test |
| `test/native/test_storage.cpp` | Storage |

### Testes DTC-02 (Sprint)
O `test_dtc.cpp` inclui 8 testes obrigatórios do Sprint DTC-02:
1. Cell overvoltage com source/value corretos.
2. Normalização → CLEARED.
3. Undervoltage com célula correta.
4. Overtemperature.
5. Overcurrent.
6. Múltiplas falhas simultâneas.
7. Overvoltage protege MOSFET de carga.
8. Short circuit (FATAL, MOSFETs off).

### Como Executar
```bash
pio test -e native
```

Para gerar o relatório de testes:
```bash
python scripts/generate_report.py
```

---

## 15. Build / Desenvolvimento

O projeto usa **PlatformIO**.

### Ambiente Native (PC/Mock)
```bash
pio run -e native
```
Executa o firmware em modo simulação (mock), sem hardware físico.

### Ambiente ESP32 (Hardware Real)
```bash
pio run -e esp32dev
```
Compila para ESP32. Requer a plataforma `espressif32` instalada.

### Monitor Serial (ESP32)
```bash
pio run -e esp32dev -t upload && pio device monitor
```

### Configuração do PlatformIO (resumo)
- **`[env:native]`** — `platform = native`, `build_type = debug`, inclui core/system/communication/lib.
  - `lib_deps`: `bms-core`, `bms-system`, `bms-communication`.
  - `lib_extra_dirs`: `core`, `system`, `communication`, `lib`.
  - `extra_scripts`: `pre:scripts/test_build_hook.py` (resolve símbolos nos testes).
- **`[env:esp32dev]`** — `platform = espressif32`, `framework = arduino`, `board = esp32dev`.
  - `lib_deps`: Adafruit INA219, Adafruit SSD1306, Adafruit GFX Library.

---

## 16. Hardware Suportado

| Módulo | Interface | Sensores/Drivers |
|--------|-----------|------------------|
| Tensão de células | ADC | ADC interno, ADS1115 |
| Corrente | I²C | INA219, INA226, Hall |
| Temperatura | ADC | NTC 10k/100k |
| SOC | I²C | MAX17048 |
| Display | I²C | OLED SSD1306 |
| Comunicação | CAN/Serial | TWAI, MCP2515 |
| BLE | — | GATT |
| MOSFETs | GPIO | Carga/Descarga/Balanço |

---

## 17. Documentação Disponível

### Requisitos (docs/SRS/)
- `SRS-001-Visao-Geral.md`, `SRS-002-Arquitetura.md`, `SRS-003-Hardware.md`, `SRS-004-Protecoes.md`, `SRS-005-Comunicacao.md`, `SRS-006-FreeRTOS.md`, `SRS-007-Diagnostico.md`, `SRS-008-Testes.md`, `SRS-009-Producao.md`, `SRS-010-Roadmap.md`.

### Design (docs/DDS/)
- `DDS-001-Arquitetura-do-Firmware.md`, `DDS-002-Hardware-Abstraction-Layer.md`, `DDS-003-Drivers-de-Hardware.md`, `DDS-004-Battery-Manager.md`, `DDS-005-Protection-Manager.md`, `DDS-006-Balance-Manager.md`, `DDS-007-Communication-Manager.md`, `DDS-008-Logger-and-Diagnostics.md`, `DDS-009-Task-Manager-FreeRTOS.md`, `DDS-010-Configuration-Manager.md`.

### Interfaces (docs/ICD/)
- 23 documentos de Interface Contract Description (ICD-001 a ICD-023), cobrindo cada módulo (Battery, Protection, Balance, Communication, HAL, Storage, Logger, Configuration, Task Manager, Fault, Watchdog, CLI, BLE, CAN, Display, OTA, Test, Factory Test, Security, Power, RTOS Integration, Heartbeat, Architecture Evolution).

### Outros
- `README.md`, `ARCHITECTURE.md`, `ARCHITECTURE_REVIEW.md`, `PRODUCTION.md`, `RELEASE.md`, `DTC_DESIGN.md`, `MIGRATION.md`, `CHANGELOG.md`, `SECURITY.md`, `CONTRIBUTING.md`, `LICENSE`.

---

## 18. Estado do Desenvolvimento (TODO.md)

**Sprint DTC-02 — Unificação ProtectionManager → FaultManager → DTC**

| Tarefa | Status |
|--------|--------|
| 1. Remover `fault_.evaluate(pack)` como mecanismo primário (LEGACY) | ✅ Concluído |
| 2. Adicionar detecção de short circuit no ProtectionManager | ✅ Concluído |
| 3. Adicionar 8 testes obrigatórios do Sprint DTC-02 | ✅ Concluído |
| 4. Validar `pio run -e native` BUILD SUCCESS | ✅ Concluído |
| 5. Validar `pio test -e native` (issue de infraestrutura de testes) | ⏳ Pendente |
| 6. Validar execução do `program.exe` (SELF TEST 8/8 OK) | ✅ Concluído |

---

## 19. Changelog (v1.0.0)

### Adicionado
- **Self Test (POST)** integrado à máquina de estados.
- **Interfaces HAL** com suporte a inicialização (`begin()`).
- **Mocks atualizados** para as novas interfaces.
- **Documentação de produção** (ARCHITECTURE, PRODUCTION, RELEASE, README, CHANGELOG, LICENSE, SECURITY, CONTRIBUTING).
- **Estrutura pronta para GitHub** (`.gitignore`).
- **Web Dashboard** — telemetria.
- **Scheduler & Heartbeat**.

### Melhorado
- Padronização de formatação no `src/app/app.h`.
- Documentação de arquitetura e fluxo de dados.

### Corrigido
- Inicialização de sensores de hardware no `main_esp32.cpp` (via `begin()`).

---

## 20. Pontos de Atenção / Observações

1. **Infraestrutura de testes (PENDING):** O PlatformIO native test runner não linka as
   bibliotecas de `lib_extra_dirs` (.a) nos executáveis de teste, causando
   `undefined reference`. O código compila via `pio run` (SUCCESS), mas a validação
   por `pio test` requer correção do ambiente de teste.
2. **Arquitetura reorganizada:** O código-fonte permanece na raiz enquanto as pastas
   `firmware/`, `simulation/`, `hardware/` etc. são apenas mapeamento lógico.
3. **Detecção de falhas centralizada:** O `ProtectionManager` é a fonte principal de
   detecção de falhas; o `FaultManager::evaluate()` está desabilitado para evitar
   duplicação de DTCs.
4. **Política industrial:** O histórico de falhas **não é limpo** automaticamente no
   boot (configurável via `CLEAR_FAULT_HISTORY_ON_BOOT`).

---

## 21. Conclusão

A aplicação é um **BMS firmware completo e bem estruturado**, com arquitetura em camadas
(Clean Architecture / SOLID / Dependency Injection), separando claramente a lógica de
negócio (`core/`), infraestrutura (`system/`), comunicação (`communication/`) e hardware
(HAL). O sistema oferece proteções abrangentes, diagnóstico DTC persistido, telemetria via
CAN/BLE/Web, e suporte a múltiplas plataformas (ESP32 e PC simulado). A documentação é
extensa (SRS, DDS, ICD), e o projeto está em fases de consolidação de testes e validação
industrial.

---
*Relatório gerado automaticamente com base na análise do repositório.*
