# SRS-003 – Hardware

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Prototipo 3S especificado; PCB propria em fase de arquitetura
**Dependências:** SRS-001 – Visão Geral, SRS-002 – Arquitetura

---

# 1. Objetivo

Este documento especifica a arquitetura de hardware suportada pelo firmware do BMS.

Define os componentes compatíveis, interfaces elétricas, abstração de hardware (HAL) e os requisitos mínimos para operação em simulação e hardware real.

---

# 2. Plataforma Principal

## Microcontrolador

Plataforma principal:

* ESP32 DevKit V1

Compatibilidade futura:

* ESP32-WROOM
* ESP32-S3
* ESP32-C3 (com adaptações)
* Outras placas ESP32 compatíveis com Arduino Framework

---

# 3. Modos de Execução

O firmware deverá operar em três modos:

### Mock (PC)

Objetivo:

* Desenvolvimento.
* Testes unitários.
* Validação de lógica.

Não utiliza hardware físico.

---

### Wokwi

Objetivo:

* Simulação embarcada.
* Testes rápidos de integração.
* Demonstração da aplicação.

Utiliza periféricos simulados.

---

### Hardware Real

Objetivo:

* Testes elétricos.
* Validação de sensores.
* Ensaios em bancada.
* Operação do sistema físico.

---

# 4. Sensores Suportados

> Para o prototipo 3S, o ADC interno com divisor e somente uma etapa de
> bancada. A protecao primaria e a medicao de producao devem usar AFE BMS
> dedicado. Consulte `docs/HARDWARE_3S_ESP32_PROTOTYPE.md` e
> `docs/BMS_OWN_PCB_ROADMAP.md`.

## Tensão das células

Interfaces previstas:

* ADC interno (protótipos)
* ADS1115
* ADC dedicado para BMS (expansão futura)

---

## Corrente

Sensores suportados:

* INA219
* INA226
* Sensor Hall (configurável)

A HAL deverá permitir trocar o sensor sem alterar a lógica do firmware.

---

## Temperatura

Sensores previstos:

* NTC 10k
* NTC 100k
* Sensores digitais compatíveis com futuras expansões

Número de sensores configurável.

---

# 5. Comunicação

## CAN (TWAI)

Objetivos:

* Envio periódico de telemetria.
* Envio de alarmes.
* Diagnóstico.

O firmware deverá utilizar a interface TWAI do ESP32 ou controlador CAN externo, conforme a versão do hardware.

---

## Bluetooth Low Energy (BLE)

Objetivos:

* Monitoramento.
* Configuração.
* Diagnóstico.

Os serviços BLE deverão permitir leitura dos principais dados do sistema.

---

## Serial

Usada para:

* Debug.
* Dashboard.
* CLI.
* Atualizações de diagnóstico.

---

# 6. Display

Display principal:

OLED SSD1306 (I²C)

Informações exibidas:

* Estado do sistema.
* SOC.
* SOH.
* Tensão do pack.
* Corrente.
* Temperatura.
* Alarmes.
* Estado do balanceamento.

---

# 7. Balanceamento

O firmware deverá suportar:

* Balanceamento passivo.
* Controle individual por célula.
* Expansão para balanceamento ativo.

Cada canal deverá ser controlado pela HAL.

---

# 8. MOSFETs

Controle independente para:

* Carga.
* Descarga.

Estados possíveis:

* ON
* OFF
* Protegido
* Emergência

---

# 9. Memória

Persistência utilizando:

* NVS (ESP32)

Itens armazenados:

* Configurações.
* Calibrações.
* Histórico de falhas.
* Contador de ciclos.
* Dados de manutenção.

---

# 10. Watchdog

O firmware deverá utilizar:

* Watchdog de software.
* Watchdog de hardware (quando disponível).

Objetivos:

* Detectar travamentos.
* Reinicializar o sistema de forma segura.
* Registrar a causa do reinício.

---

# 11. Alimentação

O firmware deverá operar corretamente dentro das especificações do hardware escolhido.

A lógica de proteção deverá impedir a operação em condições elétricas inseguras.

---

# 12. Hardware Abstraction Layer (HAL)

A HAL deverá fornecer interfaces para:

* ADC
* GPIO
* PWM
* I²C
* SPI
* UART
* TWAI (CAN)
* BLE
* Timer
* NVS

A lógica do BMS nunca deverá acessar diretamente periféricos do ESP32.

---

# 13. Compatibilidade

O projeto deverá manter compatibilidade com:

* PlatformIO
* Arduino Framework
* ESP32 DevKit
* Wokwi
* Simulação Mock

---

# 14. Requisitos de Expansão

A arquitetura deverá permitir a inclusão futura de:

* Cartão SD
* Wi-Fi
* Ethernet
* GPS
* RTC
* Buzzer
* LEDs de status
* Teclado de navegação
* Display TFT
* Sensores adicionais

Sem alterações significativas na lógica principal.

---

# 15. Critérios de Aceitação

O hardware será considerado suportado quando:

* Inicializar corretamente.
* For reconhecido pela HAL.
* Permitir leituras consistentes.
* Permitir testes em Mock, Wokwi e hardware real.
* Não exigir alterações na lógica do firmware para troca de sensores compatíveis.

---

# 16. Controle de Versão

| Versão | Data       | Descrição                                              |
| ------ | ---------- | ------------------------------------------------------ |
| 1.0.0  | 03/07/2026 | Definição inicial da plataforma de hardware suportada. |

