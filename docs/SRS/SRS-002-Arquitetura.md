# SRS-002 — Arquitetura do Firmware

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Em desenvolvimento
**Dependência:** SRS-001 – Visão Geral

---

# 1. Objetivo

Definir a arquitetura oficial do firmware do BMS, estabelecendo a organização dos módulos, responsabilidades, fluxo de execução e regras de desenvolvimento.

Esta arquitetura deve permitir:

* Alta escalabilidade.
* Fácil manutenção.
* Independência entre hardware e lógica de negócio.
* Facilidade para testes em Mock, Wokwi e hardware real.
* Reutilização de código.
* Evolução para futuras versões do firmware.

---

# 2. Princípios da Arquitetura

O firmware deverá seguir os seguintes princípios:

* Separação por responsabilidade (Single Responsibility Principle).
* Baixo acoplamento entre módulos.
* Alta coesão.
* Programação orientada a interfaces sempre que possível.
* Nenhuma lógica de negócio acessará diretamente GPIO, ADC, Serial ou periféricos.
* Toda interação com hardware será realizada pela HAL (Hardware Abstraction Layer).

---

# 3. Arquitetura em Camadas

```text
+------------------------------------------------------+
|                   Application Layer                  |
|  State Machine | Diagnostics | Dashboard | CLI       |
+------------------------------------------------------+
|                    Service Layer                     |
| SOC | SOH | Protection | Balancing | Events | Faults |
+------------------------------------------------------+
|                Communication Layer                   |
| BLE | CAN | Serial | Display                         |
+------------------------------------------------------+
|                     HAL Layer                        |
| ADC | GPIO | PWM | I2C | SPI | UART | Timer | NVS    |
+------------------------------------------------------+
|                 Hardware Drivers                     |
| ESP32 | ADS1115 | INA219 | SSD1306 | MCP2515 | NTC   |
+------------------------------------------------------+
```

Cada camada poderá acessar apenas a camada imediatamente inferior.

---

# 4. Estrutura Oficial de Diretórios

```text
BMS/
│
├── docs/
├── include/
│
├── src/
│   ├── app/
│   ├── core/
│   ├── hal/
│   ├── sensors/
│   ├── battery/
│   ├── protection/
│   ├── balancing/
│   ├── communication/
│   ├── diagnostics/
│   ├── storage/
│   ├── display/
│   ├── cli/
│   ├── platform/
│   └── utils/
│
├── lib/
├── test/
└── platformio.ini
```

---

# 5. Responsabilidades dos Módulos

## Core

Responsável pelo ciclo principal do firmware.

* Inicialização.
* Scheduler.
* Máquina de estados.
* Gerenciamento do modo de operação.

---

## HAL

Responsável por abstrair o hardware.

Implementações:

* Mock.
* Wokwi.
* ESP32 real.

A lógica do BMS nunca deverá conhecer detalhes do hardware.

---

## Sensors

Responsável pela aquisição de dados.

Leituras:

* Tensão das células.
* Corrente.
* Temperatura.
* Tensão do pack.

---

## Battery

Responsável pelos cálculos elétricos.

* SOC.
* SOH.
* SOP.
* Energia.
* Capacidade restante.
* Ciclos.

---

## Protection

Implementa todas as proteções.

* OVP.
* UVP.
* OCP.
* OTP.
* UTP.
* Curto-circuito (preparado para futura implementação).

---

## Balancing

Controla o balanceamento.

Responsabilidades:

* Seleção das células.
* Estratégia.
* PWM.
* Tempo máximo.
* Critérios de desligamento.

---

## Communication

Responsável por:

* CAN.
* BLE.
* Serial.
* Dashboard.
* CLI.

---

## Diagnostics

Responsável por:

* Logger.
* Fault Manager.
* Event Manager.
* Performance Monitor.
* Health Monitor.

---

## Storage

Responsável por:

* NVS.
* EEPROM (quando aplicável).
* Configurações.
* Histórico.
* Calibração.

---

## Display

Responsável por:

* OLED.
* Menus.
* Alarmes.
* Dashboard local.

---

## CLI

Console de diagnóstico via Serial.

Exemplos de comandos:

status

cells
temps
faults
config
help
reboot
reset

---

# 6. Fluxo Geral

```text
Boot

↓

HAL Init

↓

Storage Init

↓

Logger Init

↓

Sensors Init

↓

Protection Init

↓

Communication Init

↓

Display Init

↓

State Machine Init

↓

Scheduler

↓

Loop Principal
```

---

# 7. Máquina de Estados

Estados oficiais:

BOOT

INIT

NORMAL

CHARGING

DISCHARGING

BALANCING

WARNING

FAULT

SHUTDOWN

EMERGENCY

As transições deverão ocorrer somente pela State Machine.

---

# 8. FreeRTOS

O firmware será preparado para utilizar tarefas independentes.

TaskSensors

TaskProtection

TaskSOC

TaskBalancing

TaskCAN

TaskBLE

TaskDisplay

TaskLogger

TaskCLI

TaskStorage

TaskDiagnostics

Nenhuma tarefa poderá utilizar delay() bloqueante.

---

# 9. Fluxo de Dados

```text
Sensores

↓

HAL

↓

Filtro

↓

Battery Model

↓

Protection

↓

State Machine

↓

Fault Manager

↓

Logger

↓

CAN

↓

BLE

↓

OLED

↓

Dashboard
```

---

# 10. Dependências Permitidas

Exemplo:

Protection

↓

Battery

↓

Sensors

↓

HAL

Nunca será permitido:

Protection → Display

Battery → Serial

CAN → ADC

---

# 11. Modos de Execução

## Mock

Simulação completa no computador.

---

## Wokwi

ESP32 virtual.

---

## Hardware

ESP32 físico.

A troca deverá ocorrer por configuração, sem modificar a lógica principal.

---

# 12. Configuração Centralizada

Todos os parâmetros do firmware deverão estar em um único módulo.

Exemplos:

OVP

UVP

OCP

OTP

UTP

Número de células

Intervalo de leitura

Nome BLE

Velocidade CAN

Taxa de atualização do display

---

# 13. Tratamento de Erros

Nenhum módulo poderá desligar diretamente o sistema.

Fluxo obrigatório:

Erro

↓

Fault Manager

↓

Logger

↓

State Machine

↓

Ação

---

# 14. Regras de Desenvolvimento

* C++17.
* PlatformIO.
* Código modular.
* Interface (.h) separada da implementação (.cpp).
* Uso de `enum class`.
* Uso de `constexpr`.
* Evitar variáveis globais.
* Comentários voltados à intenção do código.
* Compilação sem erros e sem warnings relevantes.

---

# 15. Critérios de Aceitação

A arquitetura será considerada aprovada quando:

* Todos os módulos estiverem isolados.
* A HAL permitir troca entre Mock, Wokwi e Hardware Real.
* O código compilar para ESP32.
* O firmware operar sem dependências circulares.
* Os módulos puderem ser testados individualmente.
* A documentação estiver sincronizada com a implementação.

---

# 16. Evolução Prevista

A arquitetura deverá permitir a inclusão futura de:

* Wi-Fi
* OTA
* MQTT
* Modbus
* Ethernet
* Armazenamento em cartão SD
* Aplicativo móvel
* Interface Web
* Atualização remota de firmware
* Suporte a novos microcontroladores sem reescrever a lógica do BMS

---

# 17. Controle de Versão

| Versão | Data       | Descrição                                     |
| ------ | ---------- | --------------------------------------------- |
| 1.0.0  | 03/07/2026 | Definição inicial da arquitetura do firmware. |


