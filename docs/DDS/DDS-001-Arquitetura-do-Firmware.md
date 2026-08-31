# DDS-001 – Arquitetura do Firmware

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Arquitetura Base

**Dependências:** SRS-001 até SRS-010

---

# 1. Objetivo

Este documento define a arquitetura completa do firmware do Battery Management System (BMS), estabelecendo as camadas, módulos, dependências e fluxo de dados.

O objetivo é garantir:

* Alta coesão entre módulos.
* Baixo acoplamento.
* Facilidade de manutenção.
* Portabilidade.
* Testabilidade.
* Escalabilidade.
* Segurança.

---

# 2. Filosofia da Arquitetura

O firmware seguirá os princípios de:

* Separação de responsabilidades.
* Inversão de dependências.
* Arquitetura em camadas.
* Hardware Abstraction Layer (HAL).
* Desenvolvimento orientado a interfaces.
* Modularidade.

Nenhum módulo de negócio deverá depender diretamente das APIs do ESP32.

---

# 3. Arquitetura em Camadas

```text
+------------------------------------------------------+
|                 Aplicação (Application)              |
| State Machine | CLI | Dashboard | OLED | BLE | CAN   |
+------------------------------------------------------+
|                  Serviços (Services)                 |
| Battery | Protection | Balance | Logger | Storage    |
+------------------------------------------------------+
|               Middleware / Managers                  |
| Config | Fault | Event | Communication | Task        |
+------------------------------------------------------+
|             Drivers e HAL (Abstração)               |
| ADC | GPIO | PWM | CAN | BLE | I2C | SPI | UART      |
+------------------------------------------------------+
|                Hardware (ESP32)                      |
+------------------------------------------------------+
```

---

# 4. Estrutura do Projeto

```text
docs/
├── SRS/
├── DDS/
├── ADR/
├── ICD/
├── TEST/

config/
drivers/
hal/
include/
middleware/
services/
src/
tasks/
tests/
tools/
```

---

# 5. Fluxo Principal

```text
Boot
 │
 ▼
Inicialização HAL
 │
 ▼
Configuração
 │
 ▼
Sensores
 │
 ▼
Battery Manager
 │
 ▼
Protection Manager
 │
 ▼
Balance Manager
 │
 ▼
Communication Manager
 │
 ▼
Dashboard / BLE / CAN / OLED
```

---

# 6. Módulos Principais

## HAL

Responsável por abstrair o hardware.

Interfaces previstas:

* ADC
* GPIO
* PWM
* UART
* SPI
* I2C
* CAN (TWAI)
* BLE
* Display
* NVS

---

## Drivers

Implementam o acesso aos componentes físicos.

Exemplos:

* ADS1115
* INA226
* MCP2515 (caso utilizado)
* OLED SSD1306
* Sensores de temperatura

---

## Services

Contêm a lógica de negócio.

### Battery Manager

Responsável por:

* SOC
* SOH
* Energia
* Capacidade restante

### Protection Manager

Responsável por:

* OVP
* UVP
* OCP
* OTP
* UTP
* Curto-circuito

### Balance Manager

Responsável por:

* Seleção da célula.
* Acionamento do balanceamento.
* Monitoramento térmico.
* Timeout.

### Logger

Responsável por:

* Logs.
* Eventos.
* Histórico.
* Dashboard.

### Storage

Responsável por:

* Configurações.
* Histórico.
* Persistência.

---

# 7. Managers

## Configuration Manager

Carrega e valida parâmetros do sistema.

---

## Fault Manager

Centraliza todas as falhas.

Estados:

* Ativa
* Reconhecida
* Recuperada
* Persistente

---

## Event Manager

Registra eventos importantes do sistema.

---

## Communication Manager

Coordena:

* CAN
* BLE
* Serial
* OLED

---

## Task Manager

Gerencia:

* Criação das tarefas.
* Prioridades.
* Filas.
* Watchdog.

---

# 8. Máquina de Estados

Estados principais:

* INIT
* STANDBY
* CHARGING
* DISCHARGING
* BALANCING
* FAULT
* SHUTDOWN

Toda transição deverá ser validada pelo Protection Manager.

---

# 9. Fluxo de Dados

```text
Sensores
     │
     ▼
HAL
     │
     ▼
Drivers
     │
     ▼
Services
     │
     ▼
Managers
     │
     ▼
Application
```

---

# 10. Dependências

As dependências deverão seguir apenas um sentido:

Hardware → HAL → Drivers → Services → Managers → Application

Nenhuma camada poderá acessar diretamente uma camada inferior que não seja sua dependência imediata.

---

# 11. Tratamento de Erros

Todos os módulos deverão retornar códigos de status padronizados.

Os erros críticos deverão ser encaminhados ao Fault Manager, que decidirá a ação apropriada.

---

# 12. Configuração

Todos os parâmetros configuráveis deverão ser centralizados e carregados durante a inicialização.

Perfis previstos:

* MOCK
* WOKWI
* HARDWARE

---

# 13. Requisitos Não Funcionais

* Código em C++17.
* Compilação com PlatformIO.
* Compatível com ESP32.
* Testável em PC.
* Compatível com Wokwi.
* Uso mínimo de memória.
* Inicialização rápida.
* Execução determinística.

---

# 14. Critérios de Aceitação

A arquitetura será considerada aprovada quando:

* Todas as camadas estiverem implementadas.
* Não houver dependências circulares.
* O firmware puder ser compilado para Mock, Wokwi e Hardware sem alterar o código-fonte.
* Os módulos forem independentes e reutilizáveis.

---

# 15. Evolução

A arquitetura deverá permitir a inclusão futura de:

* Wi-Fi.
* MQTT.
* Modbus.
* Atualização OTA.
* Balanceamento ativo.
* Suporte a múltiplos packs.
* Integração com sistemas supervisórios.

---

# 16. Controle de Versão

| Versão | Data       | Descrição                                           |
| ------ | ---------- | --------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Definição da arquitetura detalhada do firmware BMS. |

