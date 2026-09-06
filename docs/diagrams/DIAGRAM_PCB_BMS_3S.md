# Desenho da futura PCB BMS propria 3S

**Objetivo:** placa rigida para substituir a montagem com ESP32 DevKit,
preservando as interfaces consumidas pelo firmware e adicionando protecao
primaria independente.

## 1. Diagrama de blocos da placa

```text
                       +-----------------------------+
 PACK B- ------------- | GND / referencia de medicao |
 PACK B1 ------------- | AFE BMS: CELL1             |
 PACK B2 ------------- | AFE BMS: CELL2             |
 PACK B+ ------------- | AFE BMS: CELL3             |
                       | 3-5S, OV/UV/OT/OC, ALERT    |
 NTC1..NTC4 ---------- | entradas termicas           |
                       +--------------+--------------+
                                      |
                         I2C/SPI + ALERT + SHUTDOWN
                                      |
+-------------+       +--------------v--------------+       +-------------+
| Shunt       |------->| ESP32-WROOM / MCU supervisor|------>| CAN trans.  |
| Kelvin      | INA226 | App, BLE, log, display      |       | TVS + term. |
+------+------+       +--------------+--------------+       +-------------+
       |                               |
       |                         I2C / UART / USB
       |                               +-------- OLED / service port
       |
       +--> AFE current/protection input

 AFE charge/discharge outputs
       |
       +--> gate driver CHARGE --> MOSFET/contactor CHARGE
       +--> gate driver DISCH  --> MOSFET/contactor DISCHARGE
       +--> gate driver BAL1   --> resistor + MOSFET cell 1
       +--> gate driver BAL2   --> resistor + MOSFET cell 2
       +--> gate driver BAL3   --> resistor + MOSFET cell 3

 PACK B+ --> fuse --> reverse protection --> precharge --> load/charger
```

## 2. Stack fisico recomendado

```text
+-------------------------------------------------------------+
| Zona A: conectores de celula / AFE / NTC                  |
|  B-  B1  B2  B+   CELL INPUT FILTERS   AFE                |
+------------------------------+------------------------------+
| Zona B: potencia             | Zona C: controle           |
| fuse, precharge, shunt,      | ESP32-WROOM, memoria,       |
| MOSFETs/contatores, cobre    | watchdog, I2C, UART, BLE    |
+------------------------------+------------------------------+
| Zona D: comunicacao CAN, TVS, conector service, test points|
+-------------------------------------------------------------+
```

Separar retorno de corrente de potencia do GND analogico do AFE. Unir os
pontos conforme o datasheet do AFE, em um ponto controlado. Manter antena BLE
fora da zona de cobre de potencia e longe do CAN.

## 3. Conectores propostos

```text
J1 PACK:
  1 B-
  2 B1
  3 B2
  4 B+

J2 POWER:
  1 PACK-
  2 LOAD-/CHARGE-
  3 LOAD+/CHARGE+
  4 PRECHARGE

J3 SERVICE:
  1 GND  2 3V3  3 UART_TX  4 UART_RX  5 EN  6 BOOT

J4 CAN:
  1 GND  2 CANH  3 CANL  4 SHIELD/CHASSIS conforme EMC

J5 NTC:
  1 NTC1  2 NTC2  3 NTC3  4 NTC4  5 GND
```

Os conectores reais devem ter trava, polarizacao mecanica, corrente nominal,
separacao e identificacao gravada na serigrafia.

## 4. Relacao com este firmware

| Firmware atual | PCB propria |
|---|---|
| GPIO21/22 I2C | barramento AFE, INA226, OLED ou expansor |
| GPIO26/27/14 | comandos para drivers, nunca gates diretos |
| GPIO32 rele | precharge/contator via driver |
| GPIO5/4 TWAI | transceiver CAN protegido |
| BLE ESP32 | antena e keepout RF na PCB |
| `IADC` | substituir por `IAfeBatteryMonitor` |
| NVS | parametros versionados + CRC + fallback |
| watchdog software | watchdog hardware + reset supervisor |

## 5. Requisitos de seguranca da placa

- AFE deve conseguir desligar potencia sem depender do ESP32.
- Gate drivers devem iniciar em OFF com MCU sem alimentacao.
- Fusivel deve estar no caminho definido pelo estudo de curto-circuito.
- Precharge deve limitar corrente do capacitor da carga.
- TVS, filtro e protecao contra inversao devem ser dimensionados para o pack.
- Shunt deve usar conexao Kelvin e calculo termico.
- Trilhas e conectores devem ser dimensionados pela corrente de pico, nao pela
  corrente media.
- Test points devem permitir medir cada tap sem desmontar a placa.
- Deve existir um ponto de corte fisico e um modo de manutencao seguro.

## 6. Entregaveis CAD antes de fabricar

```text
[ ] esquematico KiCad com ERC limpo
[ ] netlist revisada por segundo engenheiro
[ ] calculo de divisores, filtros, shunt, gates e termica
[ ] BOM com fabricante, substituto e ciclo de vida
[ ] PCB com DRC limpo, regras de cobre e keepout RF
[ ] Gerber, drill, pick-and-place e desenho de montagem
[ ] fixture de teste e procedimento de fim de linha
[ ] firmware HAL do AFE e testes nativos
[ ] DFMEA/PFMEA e matriz de riscos
```

Este desenho e uma arquitetura de entrada. Nao substitui o esquematico CAD,
datasheets, calculos de seguranca ou revisao de engenharia.
