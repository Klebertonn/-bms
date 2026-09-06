# BOM inicial - bancada 3S ESP32

**Objetivo:** montar um prototipo de medicao e supervisao com corrente limitada.
**Nao e BOM de producao.** Os itens de potencia devem ser recalculados conforme
corrente, temperatura, encapsulamento e normas do produto final.

## 1. Controle e alimentacao

| Qtd | Componente | Especificacao minima | Funcao |
|---:|---|---|---|
| 1 | ESP32 DevKit V1/WROOM | 3.3 V logic, USB | controlador de bancada |
| 1 | Conversor buck | entrada 9-15 V, saida 5 V, >=1 A | alimentacao USB/5 V |
| 1 | Fusivel de entrada | dimensionado para a fonte, inicialmente 2 A | protecao do prototipo |
| 1 | Chave de emergencia | corte fisico da energia do pack | desligamento independente |
| 1 | Protoboard nao e recomendada para potencia | usar placa perfurada/PCB e bornes | conexoes mecanicas seguras |

## 2. Medicao de tensao dos taps

| Qtd | Componente | Especificacao minima | Funcao |
|---:|---|---|---|
| 3 | Resistor 150 kOhm | 1%, 0.25 W | divisor superior TAP1/TAP2/TAP3 |
| 3 | Resistor 33 kOhm | 1% | divisor inferior |
| 3 | Resistor 1 kOhm | 1% | serie de entrada ADC |
| 3 | Capacitor 100 nF | X7R, 25 V | filtro ADC |
| 6 | Diodo Schottky | baixa fuga, clamp 3V3/GND | protecao de entrada |
| 3 | Ponto de teste | borne ou header identificado | medir taps com multimetro |
| 1 | AFE BMS recomendado | BQ76920/BQ76930 ou equivalente | protecao/medicao real para comparacao |

O AFE nao deve ser omitido se o pack for carregado. O divisor conectado ao
ESP32 nao e uma protecao primaria de bateria.

## 3. Corrente e temperatura

| Qtd | Componente | Especificacao minima | Funcao |
|---:|---|---|---|
| 1 | INA219 | modulo 3.3 V, shunt adequado a corrente | medicao de corrente de bancada |
| 1 | Shunt | valor e potencia conforme corrente; para 2 A, margem termica >=2x | medicao de corrente |
| 1 | NTC 10 kOhm B3950 | encapsulado para contato termico | temperatura do pack |
| 1 | Resistor 10 kOhm | 1% | divisor NTC |
| 1 | Capacitor 100 nF | X7R | filtro NTC |
| 1 | MAX17048 | modulo compativel com tensao/celula | SOC experimental; nao substitui AFE |

## 4. Potencia e atuacao

| Qtd | Componente | Especificacao minima | Funcao |
|---:|---|---|---|
| 1 | Driver de gate carga | logic-level, entrada 3.3 V | comando MOSFET carga |
| 1 | Driver de gate descarga | logic-level, entrada 3.3 V | comando MOSFET descarga |
| 1 | MOSFET carga | VDS > tensao do pack, RDS(on) em VGS real | comutacao controlada |
| 1 | MOSFET descarga | VDS > tensao do pack, SOA validada | comutacao controlada |
| 3 | MOSFET de balanceamento | canal individual, SOA validada | balanceamento passivo |
| 3 | Resistor de balanceamento | potencia calculada, inicialmente baixa corrente | dissipacao de energia |
| 4 | Resistor de gate | 47-220 Ohm | controle de slew/ringing |
| 4 | Resistor pull-down | 10 kOhm | estado OFF durante reset |
| 1 | Rele/contator pequeno | tensao e corrente compatíveis | corte/precharge experimental |
| 1 | Diodo de roda livre | conforme bobina | protecao do rele |

## 5. Comunicacao e observacao

| Qtd | Componente | Especificacao minima | Funcao |
|---:|---|---|---|
| 1 | OLED SSD1306 | I2C, 128x64, 3.3 V | diagnostico local |
| 1 | Transceiver CAN | 3.3 V, compatibilidade TWAI | CAN fisico |
| 1 | Adaptador USB-CAN | CAN 500 kbit/s | teste com PC |
| 1 | Multimetro calibrado | categoria apropriada | tensao/continuidade |
| 1 | Osciloscopio | recomendado | ripple, gate, CAN |
| 1 | Fonte de bancada | limite de corrente ajustavel | energia controlada |
| 1 | Carga eletronica | limite de corrente/potencia | descarga controlada |

## 6. Itens proibidos na primeira montagem

- Pack sem fusivel.
- Celula conectada diretamente a GPIO.
- Protoboard para caminho de corrente.
- Carga sem limite de corrente.
- Carregador sem terminacao correta de 3S.
- MOSFET sem pull-down.
- Operacao sem multimetro e chave de corte acessivel.
