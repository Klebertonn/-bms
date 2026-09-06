# BMS — Battery Management System

[![PlatformIO](https://img.shields.io/badge/PlatformIO-esp32%2Fnative-blue.svg)](https://platformio.org)
[![C++](https://img.shields.io/badge/C%2B%2B-11%2B-yellow.svg)](https://isocpp.org)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Release](https://img.shields.io/badge/Release-1.0-brightgreen.svg)](CHANGELOG.md)

Firmware de **Battery Management System (BMS)** em **C++** com arquitetura modular, desenvolvido para **ESP32** e **PlatformIO**, com suporte a simulação em PC (`native`/mock).

---

## Visão Geral

Este projeto implementa o firmware de controle e proteção de um pacote de baterias (BMS), com foco em segurança, diagnóstico e comunicação industrial. A arquitetura é dividida em camadas que separam a **lógica de negócio** (`core/`) da **infraestrutura** (`system/`) e do **hardware** (`lib/hal/` + `src/hal/`).

### Funcionalidades

- **State Machine** — gerenciamento de estados do BMS (`BOOT`, `INIT`, `SELF_TEST`, `READY`, `IDLE`, `CHARGING`, `DISCHARGING`, `BALANCING`, `FAULT`, `RECOVERY`, `SHUTDOWN`).
- **Self Test (POST)** — validação de hardware no boot (ADC, EEPROM, FLASH, NVS, Current Sensor, Temperature Sensor, MOSFET Driver, RAM).
- **Fault Manager** — detecção e gerenciamento de falhas.
- **Fault History** — histórico persistido de eventos de falha.
- **Protection Manager** — proteções de sobretensão, subtensão, sobrecorrente, sobre/subtemperatura e curto-circuito.
- **SOC** — estimativa de estado de carga.
- **Temperature Manager** — monitoramento de temperatura.
- **Current Manager** — monitoramento de corrente.
- **Balance Manager** — balanceamento de células.
- **MOSFET Controller** — controle de MOSFETs de carga/descarga/balanço.
- **Logger & Diagnostics** — log estruturado e eventos.
- **Storage** — persistência (NVS, EEPROM, FLASH, arquivo em native).
- **Self Test integrado** — validação de hardware no boot.
- **Scheduler & Heartbeat** — agendamento de tarefas e supervisão.
- **CAN** — comunicação via TWAI (ESP32) ou MCP2515.
- **BLE** — serviço GATT de monitoramento.
- **Web Dashboard** — visualização de telemetria.

---

## Estrutura do Repositório

```
.
├── core/               # Lógica de negócio (battery, current, temp, protection, fault, balance, soc, state, mosfet)
├── system/             # Infraestrutura (clock, logger, storage, rtos, scheduler, heartbeat)
├── communication/      # CAN e BLE
├── lib/                # Interfaces HAL e bibliotecas auxiliares
├── src/                # Implementação (app, main, mocks, drivers ESP32)
│   ├── app/            # Camada de aplicação
│   ├── hal/            # Mocks e drivers de hardware
│   └── main.cpp        # Ponto de entrada native (PC)
│   └── main_esp32.cpp  # Ponto de entrada ESP32 (hardware real)
├── config/             # Configuração do sistema (limites, pinos)
├── docs/               # Documentação (SRS, DDS, ICD, arquitetura, produção)
├── tests/              # Testes unitários e de integração (Unity)
├── scripts/            # Scripts de automação (geração de relatório)
├── web/                # Dashboard web
├── hardware/           # Artefatos de hardware (esquemas, pinagem, datasheets)
├── firmware/           # Mapeamento lógico do firmware (código na raiz)
├── simulation/         # Mapeamento lógico da simulação/mocks
├── tools/              # Ferramentas auxiliares
├── MIGRATION.md        # Plano de migração da reorganização
├── platformio.ini      # Configuração do PlatformIO
└── TODO.md             # Status do desenvolvimento
```

> **Nota de reorganização:** para não quebrar includes e o build, o código-fonte
> (`core/`, `system/`, `communication/`, `lib/`, `src/`, `config/`) permanece
> fisicamente na raiz. As pastas `firmware/`, `simulation/`, `hardware/`, `tests/`,
> `scripts/` e `tools/` organizam a estrutura lógica. Veja `MIGRATION.md`.

---

## Build / Desenvolvimento

O projeto usa **PlatformIO**.

### Ambiente Native (PC / Mock)

```bash
pio run -e native
```

Executa o firmware em modo simulação (mock), sem hardware físico.

### Ambiente ESP32 (Hardware Real)

```bash
pio run -e esp32dev
```

Compila para o ESP32. Requer a plataforma `espressif32` instalada.

### Monitor Serial (ESP32)

```bash
pio run -e esp32dev -t upload && pio device monitor
```

### Endurance (Native)

```bash
pio run -e native
.pio/build/native/program.exe --endurance --duration-ms 300000
```

Use `--duration-ms 0` (ou omita a duração) para manter a execução até `Ctrl+C`.
O relatório é salvo em `reports/endurance_latest.txt`.

---

## Testes

```bash
pio test -e native
```

Os testes usam o framework **Unity** (em `tests/`). Para gerar o relatório:

```bash
python scripts/generate_report.py
```

---

## Hardware Suportado

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

## Configuração

As constantes de configuração (limites de tensão, corrente, temperatura, pinos, períodos) estão em [`config/config.h`](config/config.h).

---

## Documentação

- [Arquitetura](docs/ARCHITECTURE.md)
- [Checklist Industrial / Produção](docs/PRODUCTION.md)
- [Release](docs/RELEASE.md)
- [Requisitos (SRS)](docs/SRS/)
- [Design (DDS)](docs/DDS/)
- [Interfaces (ICD)](docs/ICD/)
- [Especificacao eletrica 3S / ESP32 DevKit](docs/HARDWARE_3S_ESP32_PROTOTYPE.md)
- [BOM de bancada 3S](docs/BOM_3S_BENCH.md)
- [BOM detalhada com modelos e marcas](docs/BOM_3S_BENCH_DETAILED.md)
- [Lista de compra completa da bancada](docs/LISTA_COMPRA_BANCADA_3S.md)
- [Plano de teste de bancada](docs/BENCH_TEST_PLAN_3S.md)
- [Roadmap da PCB BMS propria](docs/BMS_OWN_PCB_ROADMAP.md)
- [Desenho da bancada 3S + ESP32 DevKit](docs/DIAGRAM_BANCADA_3S_ESP32.md)
- [Desenho da futura PCB BMS 3S](docs/DIAGRAM_PCB_BMS_3S.md)
- [Netlist de montagem da bancada](docs/NETLIST_3S_BENCH.md)

---

## Licença

Este projeto é distribuído sob a licença **MIT**. Veja o arquivo [`LICENSE`](LICENSE).

## Segurança

Para reportar vulnerabilidades, consulte [`SECURITY.md`](SECURITY.md).

## Contribuição

Veja [`CONTRIBUTING.md`](CONTRIBUTING.md) para guia de contribuição e [`CHANGELOG.md`](CHANGELOG.md) para histórico de versões.
