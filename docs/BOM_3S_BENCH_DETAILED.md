# BOM detalhada - bancada BMS 3S + ESP32 DevKit

**Revisao:** 0.3
**Aplicacao:** prototipo supervisionado, pack 3S Li-ion, corrente inicial limitada a 2 A.
**Status:** lista de compra para bancada; nao e BOM de producao.

> Os modelos abaixo sao referencias tecnicas. Confirme disponibilidade, lote,
> encapsulamento, datasheet e procedencia antes da compra. Modulos de marketplace
> sem fabricante identificavel nao devem ser usados como protecao primaria.

## 1. Itens obrigatorios de seguranca e energia

| Qtd | Funcao | Modelo/fabricante recomendado | Especificacao de referencia | Alternativas aceitaveis |
|---:|---|---|---|---|
| 1 | MCU | Espressif ESP32-DevKitC-32E | ESP32-WROOM-32E, logica 3.3 V, USB, ADC1 disponivel | DOIT ESP32 DEVKIT V1 com WROOM-32, somente se os GPIOs estiverem identificados |
| 1 | Fonte de bancada | Korad KA3005P | 0-30 V, 0-5 A, CC/CV, limite de corrente | Rigol DP832, Riden RD6012P com fonte DC adequada |
| 1 | Fusivel do pack | Littelfuse 0218002.MXP | 2 A, 250 V, 5x20 mm, retardamento conforme teste | Schurter 0034.3121, 2 A, com porta-fusivel apropriado |
| 1 | Porta-fusivel | Schurter 0031.8201 | 5x20 mm, montagem em painel/caixa | Porta-fusivel industrial 5x20 mm certificado |
| 1 | Corte de emergencia | Schneider Harmony XB5AS8445 ou equivalente | contato NF para remover energia dos atuadores | Eaton M22-PV/KC11, contato NF |
| 1 | Buck 5 V | Pololu D24V10F5 | entrada ate 36 V, saida 5 V, 1 A | Recom D24V10F5 ou buck industrial 9-15 V para 5 V, >=1 A |
| 1 | Carregador 3S | Usar a KA3005P em CV 12.60 V e limite <=2 A | CC/CV controlado, nunca modo fonte sem limite | Carregador Li-ion 3S certificado, saida 12.60 V CC/CV |
| 1 | Carga eletronica | ZKETECH EBD-A20H | ate 20 A/150 W, usar inicialmente <=2 A | Maynuo M9711, Rigol DL3021A |
| 1 | Pack de teste | 3 celulas Li-ion identicas + holder 3S protegido | 9.0-12.6 V, mesma quimica/capacidade, fusivel externo | Pack comercial 3S com BMS/A F E certificado |

**Configuracao inicial da fonte:** 12.60 V em CV e limite de corrente de 0.10 A
para o primeiro teste. Subir para 0.5 A e 1 A somente depois dos testes sem
pack e de medicao dos taps.

## 2. Protecao primaria recomendada

| Qtd | Funcao | Modelo/fabricante | Especificacao | Observacao |
|---:|---|---|---|---|
| 1 | AFE 3-5 celulas | Texas Instruments BQ76920EVM | monitor 3-5S, protecao de celula, alertas e balanceamento | Usar como referencia/protecao primaria; integrar por I2C/SPI conforme variante |
| 1 | AFE alternativo | Analog Devices LTC6811-1 evaluation board | monitor multicelula de alta precisao, isolamento/daisy-chain | Mais complexo e normalmente excessivo para 3S de bancada |
| 1 | BMS pronto auxiliar | JBD/LLT 3S Li-ion com corrente <=2 A, modelo rastreavel | protecao comercial 3S | Deve ser independente do ESP32 e ter especificacao verificavel |

Nao usar o ESP32, o divisor resistivo ou o firmware como unica protecao de
sobrecarga, descarga profunda ou curto.

## 3. Medicao de taps ADC

### Componentes por canal, quantidade total para 3 taps

| Qtd total | Funcao | Modelo/fabricante | Valor/especificacao |
|---:|---|---|---|
| 3 | resistor superior | Yageo RC0603FR-07150KL | 150 kOhm, 1%, 0.1 W, 0603 |
| 3 | resistor inferior | Yageo RC0603FR-0733KL | 33 kOhm, 1%, 0.1 W, 0603 |
| 3 | resistor serie ADC | Yageo RC0603FR-071KL | 1 kOhm, 1%, 0.1 W, 0603 |
| 3 | filtro ADC | Murata GRM188R71H104KA93D | 100 nF, X7R, 50 V, 0603 |
| 6 | clamp de entrada | Nexperia BAT54S, ou BAT54 + BAT54 | Schottky; validar corrente de clamp e alimentacao |
| 3 | conector de teste | Phoenix Contact 1729128 ou terminal 2.54 mm | ponto de teste do tap e divisor |
| 3 | borne de tap | WAGO 236-402 ou JST-VH B2P-VH | conector polarizado para TAP1/TAP2/TAP3 |

Com 150 kOhm e 33 kOhm, o fator ideal e:

```text
Vadc = Vtap * 33 / (150 + 33) = Vtap * 0.18033
```

O tap de 12.60 V produz aproximadamente 2.272 V no ADC. O valor final deve
ser calibrado contra multimetro e validado no driver. O codigo atual ainda nao
faz a conversao diferencial dos taps; nao conectar um pack real antes dessa
correcao.

## 4. NTC

| Qtd | Funcao | Modelo/fabricante | Especificacao |
|---:|---|---|---|
| 1-3 | sensor temperatura | TDK/EPCOS B57891M0103J000 | NTC 10 kOhm a 25 C, Beta 3988 K, radial | Confirmar Beta real e atualizar `HW_NTC_B_COEFFICIENT` |
| 1 | resistor divisor | Yageo RC0603FR-0710KL | 10 kOhm, 1%, 0603 |
| 1 | filtro | Murata GRM188R71H104KA93D | 100 nF, X7R |
| 1 | conector NTC | JST-PH B2B-PH-K | 2 vias, polarizado | Fixar NTC ao corpo do pack com isolacao |

O firmware usa GPIO39 / ADC1_CH3 para o NTC. Confirmar a placa ESP32 exata e a
ligacao do divisor antes de energizar.

## 5. Medicao de corrente

| Qtd | Funcao | Modelo/fabricante | Especificacao |
|---:|---|---|---|
| 1 | sensor digital | Texas Instruments INA226IDGSR | I2C, shunt bus ate 36 V, medicao de corrente/potencia |
| 1 | shunt | Vishay WSL3637 1L000FEA | 1 mOhm, 1%, potencia conforme encapsulamento e termica |
| 1 | alternativa simples | Adafruit INA219 breakout #904 | I2C, usar somente dentro da faixa do shunt instalado no modulo |
| 1 | conector | Phoenix Contact 1729018 | entrada/saida do caminho de corrente, corrente nominal validada |

O valor `HW_INA219_SHUNT_OHM = 0.001` exige shunt de 1 mOhm real. Nao usar
modulo INA219 com shunt padrao sem recalcular o firmware e a dissipacao.

## 6. OLED, BLE e I2C

| Qtd | Funcao | Modelo/fabricante | Especificacao |
|---:|---|---|---|
| 1 | OLED | Adafruit Monochrome 0.96in 128x64 OLED #938 | SSD1306, I2C, 3.3 V |
| 1 | fuel gauge experimental | SparkFun MAX17048 LiPo Fuel Gauge - Qwiic | I2C, endereco tipico 0x36, uma celula/pack conforme modulo |
| 1 | multiplexador opcional | TI TCA9548A | 8 canais I2C, somente se houver conflito de endereco |
| 3 | pull-up I2C opcional | Yageo RC0603FR-0710KL | 10 kOhm, usar apenas uma rede efetiva por barramento |
| 1 | conector service | JST-PH B6B-PH-K | 3V3, GND, SDA, SCL, UART, EN/BOOT |

Confirmar enderecos: INA219 tipicamente 0x40, MAX17048 0x36 e OLED 0x3C.

## 7. CAN

| Qtd | Funcao | Modelo/fabricante | Especificacao |
|---:|---|---|---|
| 1 | transceiver CAN | TI SN65HVD230D | 3.3 V, CAN 1 Mbps, compativel com TWAI |
| 1 | modulo pronto | Waveshare SN65HVD230 CAN Board | somente se a versao usar transceiver 3.3 V |
| 1 | TVS CAN | Nexperia PESD1CAN | protecao diferencial CANH/CANL |
| 1 | terminacao | resistor Vishay CRCW12061K20FKEA | 120 Ohm, habilitacao por jumper |
| 1 | conector | Phoenix Contact 1757249 | CANH, CANL, GND, shield conforme bancada |
| 1 | interface PC | CANable 2.0 | USB-CAN, SLCAN/CAN, validar firmware do adaptador |

O firmware usa GPIO5 para TX e GPIO4 para RX. CANH/CANL nunca devem ir aos
GPIOs diretamente.

## 8. Atuacao de MOSFET e precharge

| Qtd | Funcao | Modelo/fabricante | Especificacao |
|---:|---|---|---|
| 2 | MOSFET carga/descarga | Infineon IRLZ44N | 55 V, logic-level, TO-220, usar somente prototipo <=2 A com dissipacao |
| 3 | MOSFET balanceamento | Alpha & Omega AO3400A | 30 V, N-MOSFET, SOT-23, validar VGS e corrente |
| 2 | driver de gate | Microchip MCP1407-E/P | driver, alimentacao 4.5-18 V, entrada compatível com 3.3 V |
| 4 | gate resistor | Yageo RC0603FR-07100RL | 100 Ohm, 1% |
| 4 | pull-down gate | Yageo RC0603FR-0710KL | 10 kOhm, 1% |
| 3 | resistor balanceamento | Vishay RH050 100R | 100 Ohm, 5 W, usar corrente limitada e calcular calor |
| 1 | rele precharge | Omron G5LE-1A4 DC5 | bobina 5 V, contatos 10 A a 30 VDC, confirmar carga |
| 1 | diodo bobina | Vishay 1N4007 | retificador 1000 V, usar apenas para bobina de baixa frequencia |
| 1 | driver rele | TI ULN2003A | array Darlington, entrada logica, diodos internos |

O MCP1407 precisa de alimentacao apropriada; nao alimentar seu VDD pelo GPIO.
MOSFETs de potencia e resistores devem ser montados fora da protoboard.

## 9. Montagem e instrumentos

| Qtd | Item | Modelo de referencia | Especificacao |
|---:|---|---|---|
| 1 | multimetro | Keysight U1232A ou Brymen BM869s | CAT III, medicao DC e continuidade |
| 1 | osciloscopio | Rigol DHO804 ou Siglent SDS1104X-E | 4 canais, >=100 MHz recomendado |
| 1 | sonda diferencial | Micsig DP10013 ou equivalente | medir gates/CAN sem curto de terra |
| 1 | fio silicone | H05SS-F/AWG apropriado | corrente e temperatura validadas |
| 1 | placa de montagem | PCB perfurada com bornes, nao protoboard | separacao de potencia e sinal |
| 1 | caixa | Hammond 1591 ou caixa nao condutiva | cobrir partes energizadas |
| 1 | tapete ESD | 3M 8501 ou equivalente | montagem do ESP32 e sensores |

## 10. Criterios de compra

Nao comprar um substituto sem conferir:

- tensao maxima absoluta e faixa de operacao;
- nivel logico em 3.3 V;
- corrente continua e pulso;
- dissipacao e resistencia termica;
- isolamento entre controle e potencia;
- disponibilidade de datasheet e rastreabilidade;
- compatibilidade mecanica do footprint;
- protecao contra inversao, curto e ESD.

## 11. Itens que nao devem ser tratados como equivalentes

- Modulo BMS 3S generico sem esquema nao substitui AFE documentado.
- INA219 com shunt diferente nao substitui INA226 de 1 mOhm sem recalibracao.
- ESP32-C3 nao substitui ESP32-WROOM sem revisar ADC, TWAI, pinos e BLE.
- Fonte CV sem limite de corrente nao substitui carregador CC/CV.
- MOSFET com `RDS(on)` especificado apenas em 10 V nao e automaticamente
  adequado para comando de 3.3 V.
