# Lista de compra - bancada BMS 3S + ESP32 DevKit

**Revisao:** 1.0
**Uso:** prototipo de bancada supervisionado
**Pack:** 3S Li-ion, 9.0 V a 12.6 V
**Corrente inicial:** 0.10 A; limite maximo da primeira bancada: 2 A

> Confirme datasheet, procedencia e disponibilidade antes da compra. O ESP32,
> divisores ADC e firmware nao substituem um AFE/BMS de protecao primaria.

## A. Compra obrigatoria para iniciar a bancada

| Qtd | Item | Fabricante/modelo | Especificacao minima | Uso |
|---:|---|---|---|---|
| 1 | Placa MCU | Espressif ESP32-DevKitC-32E | ESP32-WROOM-32E, 3.3 V, USB | Controlador |
| 1 | Fonte CC/CV | Korad KA3005P | 0-30 V, 0-5 A, limite de corrente | Alimentacao/teste |
| 1 | Carga eletronica | ZKETECH EBD-A20H | Usar inicialmente <=0.10 A | Descarga controlada |
| 1 | Buck | Pololu D24V10F5 | Entrada ate 36 V, saida 5 V, 1 A | Alimentar bancada |
| 1 | Fusivel | Littelfuse 0218002.MXP | 2 A, 5x20 mm | Protecao do pack |
| 1 | Porta-fusivel | Schurter 0031.8201 | Para fusivel 5x20 mm | Suporte F1 |
| 1 | Chave de emergencia | Schneider Harmony XB5AS8445 | Contato NF | Corte fisico |
| 1 | Multimetro | Brymen BM869s ou Keysight U1232A | DC, continuidade, categoria apropriada | Medicao |
| 1 | Placa de montagem | PCB perfurada + bornes | Nao usar protoboard em potencia | Montagem |
| 1 | Pack de teste | 3 celulas Li-ion identicas com holder | 3S, mesma quimica/capacidade | Fonte de energia |
| 1 | Carregador/BMS auxiliar | BMS 3S rastreavel ou carregador 3S CC/CV | 12.60 V, corrente limitada | Protecao primaria |
| 1 | Caixa | Hammond 1591 ou equivalente | Nao condutiva, ventilada | Protecao mecanica |

## B. Protecao primaria recomendada

| Qtd | Item | Modelo | Especificacao | Observacao |
|---:|---|---|---|---|
| 1 | AFE BMS | Texas Instruments BQ76920EVM | Monitor/protecao 3-5S | Recomendado para conectar pack |
| 1 | Alternativa AFE | Analog Devices LTC6811-1 evaluation board | Monitor multicelula | Mais complexo; fase 2 |
| 1 | Fixture/BMS auxiliar | JBD/LLT 3S rastreavel | Protecao 3S | Nao usar modelo generico sem esquema |

## C. Medicao de tensao dos taps

Comprar para **cada um dos 3 canais**:

| Qtd total | Item | Fabricante/modelo | Especificacao |
|---:|---|---|---|
| 3 | Resistor RTOP | Yageo RC0603FR-07150KL | 150 kOhm, 1%, 0.1 W |
| 3 | Resistor RBOT | Yageo RC0603FR-0733KL | 33 kOhm, 1%, 0.1 W |
| 3 | Resistor serie | Yageo RC0603FR-071KL | 1 kOhm, 1%, 0.1 W |
| 3 | Capacitor ADC | Murata GRM188R71H104KA93D | 100 nF, X7R, 50 V |
| 6 | Diodo clamp | Nexperia BAT54S | Schottky para clamp de entrada |
| 3 | Conector tap | JST-VH B2P-VH ou WAGO 236-402 | Polarizado, 2 vias |
| 3 | Ponto de teste | Phoenix Contact 1729128 ou header | Acesso a TAP/ADC |

**Canais do firmware:**

- TAP1/B1: GPIO36 / ADC1_CH0
- TAP2/B2: GPIO34 / ADC1_CH6
- TAP3/B+: GPIO35 / ADC1_CH7
- NTC: GPIO39 / ADC1_CH3

## D. Temperatura

| Qtd | Item | Fabricante/modelo | Especificacao |
|---:|---|---|---|
| 1-3 | NTC | TDK/EPCOS B57891M0103J000 | 10 kOhm, Beta proximo de 3988 K |
| 1 | Resistor divisor | Yageo RC0603FR-0710KL | 10 kOhm, 1% |
| 1 | Capacitor filtro | Murata GRM188R71H104KA93D | 100 nF, X7R |
| 1 | Conector NTC | JST-PH B2B-PH-K | 2 vias polarizado |
| 1 | Fixacao termica | fita Kapton + isolante apropriado | Fixar NTC ao pack |

## E. Medicao de corrente

### Opcao recomendada para este firmware

| Qtd | Item | Fabricante/modelo | Especificacao |
|---:|---|---|---|
| 1 | Modulo corrente | Adafruit INA219 Breakout #904 | I2C, verificar shunt instalado |
| 1 | Shunt | Vishay WSL3637 1L000FEA | 1 mOhm, 1%, potencia termica validada |
| 1 | Bornes corrente | Phoenix Contact 1729018 | Corrente nominal compatível |

### Opcao futura

| Qtd | Item | Fabricante/modelo | Especificacao |
|---:|---|---|---|
| 1 | Monitor corrente | Texas Instruments INA226IDGSR | I2C, bus ate 36 V |
| 1 | Shunt | Vishay WSL3637 1L000FEA | 1 mOhm, Kelvin |

> INA226 exige driver e calibracao próprios. Para o primeiro teste, comprar
> INA219, que corresponde ao driver atualmente usado pelo projeto.

## F. I2C, display e SOC

| Qtd | Item | Fabricante/modelo | Especificacao |
|---:|---|---|---|
| 1 | OLED | Adafruit Monochrome 0.96in OLED #938 | SSD1306, 128x64, I2C, 3.3 V |
| 1 | Fuel gauge | SparkFun MAX17048 LiPo Fuel Gauge Qwiic | I2C, endereco tipico 0x36 |
| 1 | Conector service | JST-PH B6B-PH-K | 3V3, GND, SDA, SCL, UART, EN/BOOT |
| 2-3 | Pull-up I2C | Yageo RC0603FR-0710KL | 10 kOhm, instalar somente se necessario |
| 1 | Multiplexador opcional | TI TCA9548A | Somente se houver conflito I2C |

**Enderecos esperados:** INA219 `0x40`, MAX17048 `0x36`, OLED `0x3C`.

## G. MOSFETs, drivers e precharge

| Qtd | Item | Fabricante/modelo | Especificacao |
|---:|---|---|---|
| 2 | MOSFET carga/descarga | Infineon IRLZ44N | 55 V, logic-level, usar somente <=2 A com dissipacao |
| 3 | MOSFET balanceamento | Alpha & Omega AO3400A | 30 V, logic-level, um por celula |
| 2 | Driver gate | Microchip MCP1407-E/P | Driver de gate, VDD conforme datasheet |
| 3 | Resistor gate | Yageo RC0603FR-07100RL | 100 Ohm, 1% |
| 4 | Pull-down gate | Yageo RC0603FR-0710KL | 10 kOhm, 1% |
| 3 | Resistor balanceamento | Vishay RH050 100R | 100 Ohm, 5 W, validar aquecimento |
| 1 | Rele precharge | Omron G5LE-1A4 DC5 | Bobina 5 V, contato validado |
| 1 | Driver rele | TI ULN2003A | Driver para bobina |
| 1 | Diodo bobina | Vishay 1N4007 | Catodo no +5 V, anodo no retorno |
| 1 | Resistor precharge | Valor definido pelo ensaio da carga | Limitar corrente de inrush |

Nunca conectar GPIO diretamente a gate, bobina ou carga de potencia.

## H. CAN

| Qtd | Item | Fabricante/modelo | Especificacao |
|---:|---|---|---|
| 1 | Transceiver | Texas Instruments SN65HVD230D | CAN, 3.3 V, compatível TWAI |
| 1 | Modulo opcional | Waveshare SN65HVD230 CAN Board | Confirmar VCC 3.3 V |
| 1 | TVS CAN | Nexperia PESD1CAN | Protecao CANH/CANL |
| 1 | Terminacao | Vishay CRCW12061K20FKEA | 120 Ohm, jumper selecionavel |
| 1 | USB-CAN | CANable 2.0 | 500 kbit/s, teste com PC |
| 1 | Conector CAN | Phoenix Contact 1757249 | CANH, CANL, GND |

## I. Instrumentacao e montagem

| Qtd | Item | Modelo sugerido | Especificacao |
|---:|---|---|---|
| 1 | Osciloscopio | Rigol DHO804 ou Siglent SDS1104X-E | 4 canais, >=100 MHz recomendado |
| 1 | Sonda diferencial | Micsig DP10013 ou equivalente | Gates/CAN sem curto de terra |
| 1 | Cabos potencia | fio silicone AWG apropriado | Dimensionar pela corrente |
| 1 | Bornes | Phoenix Contact ou WAGO | Polarizados e identificados |
| 1 | Tapete ESD | 3M 8501 ou equivalente | Montagem eletrônica |
| 1 | Etiquetas | termorretratil/identificacao | B-, B1, B2, B+, GND, sinais |
| 1 | Termometro | Fluke 62 MAX ou equivalente | Verificar aquecimento |

## J. Nao comprar para a primeira montagem

- Protoboard para caminho de corrente.
- Modulo BMS generico sem esquema ou datasheet.
- MOSFET especificado apenas para VGS de 10 V.
- Fonte sem limite de corrente para carregar Li-ion.
- INA226 esperando que funcione com o driver INA219 atual.
- ESP32-C3 como substituto sem revisar pinos, ADC, CAN e BLE.
- Pack sem fusivel, AFE/BMS auxiliar e chave de corte.

## K. Ordem de compra recomendada

### Pedido 1 - controle e segurança

ESP32, fonte, buck, fusivel/porta-fusivel, chave de emergencia, multimetro,
placa perfurada, bornes, caixa e AFE/BMS auxiliar.

### Pedido 2 - medicao

Resistores, capacitores, clamps, NTC, INA219, shunt, OLED e MAX17048.

### Pedido 3 - atuacao e comunicacao

Drivers, MOSFETs, resistores de gate, rele, ULN2003A, diodo, SN65HVD230,
CANable e carga eletronica.

### Pedido 4 - evolucao

INA226, LTC6811, fixture de teste, osciloscopio/sonda diferencial e materiais
para a futura PCB propria.

## L. Verificacao antes de finalizar a compra

- [ ] Confirmar disponibilidade e datasheet de cada part number.
- [ ] Confirmar que todos os modulos trabalham com logica de 3.3 V.
- [ ] Confirmar shunt real do INA219 comprado.
- [ ] Confirmar pinagem da variante ESP32-DevKit adquirida.
- [ ] Confirmar que o BQ76920EVM sera usado como protecao primaria.
- [ ] Confirmar corrente maxima de cada borne, fio, fusivel, MOSFET e rele.
- [ ] Confirmar que o firmware diferencial dos taps sera implementado antes do pack.
- [ ] Comprar uma unidade extra de resistores, capacitores, conectores e NTCs.
