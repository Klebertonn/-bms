# Manual de montagem da bancada - BMS 3S + ESP32 DevKit
**Revisao:** 1.0
**Aplicacao:** prototipo de bancada supervisionado
**Pack:** 3 celulas Li-ion em serie, 3S, 9.0 V a 12.6 V
**Corrente inicial:** limitar a 0.10 A; somente subir gradualmente ate 2 A

> Este documento e um guia de montagem experimental. Nao e um projeto de
> seguranca certificado. O ESP32 nao pode ser a unica protecao do pack. Use um
> AFE/BMS externo, fusivel, fonte CC/CV, chave de corte e supervisao constante.

## 1. Componentes e funcao
| Item | Componente/modelo | Funcao | Observacao |
|---|---|---|---|
| U1 | Espressif ESP32-DevKitC-32E | Controlador | Logica de 3.3 V; nao aplicar tensao de pack aos GPIOs |
| PS1 | Korad KA3005P | Fonte CC/CV | Primeiro ajuste: 12.60 V e limite de 0.10 A |
| LOAD1 | ZKETECH EBD-A20H | Carga eletronica | Descarga inicial ate 0.10 A |
| PS2 | Pololu D24V10F5 | Buck | Entrada do pack, saida 5 V para modulos/ESP32 |
| F1 | Littelfuse 0218002.MXP | Fusivel | 2 A para a primeira bancada; usar porta-fusivel |
| SW1 | Chave de emergencia NF | Corte fisico | Deve desligar atuadores/caminho de potencia |
| AFE1 | TI BQ76920EVM | Protecao primaria/monitor | Recomendado quando o pack for conectado |
| U2 | INA219 Adafruit #904 | Corrente | Escolha compativel com o codigo atual |
| U2-alt | INA226 TI | Corrente | Alternativa; exige driver INA226 no firmware |
| SH1 | Shunt 1 mOhm Vishay WSL3637 | Corrente | Conferir se o modulo INA219 realmente usa 1 mOhm |
| NTC1 | TDK/EPCOS B57891M0103J000 | Temperatura | 10 kOhm, Beta proximo de 3988 K |
| RNTC | Yageo 10 kOhm 1% | Divisor NTC | GPIO39/ADC1_CH3 |
| OLED1 | Adafruit OLED #938 | Display | SSD1306 I2C 128x64 |
| SOC1 | SparkFun MAX17048 Qwiic | SOC experimental | Endereco tipico 0x36 |
| CAN1 | TI SN65HVD230D | Transceiver CAN | 3.3 V; nunca ligar CANH/CANL ao GPIO |
| CAN-USB | CANable 2.0 | Interface PC | Configurar 500 kbit/s |
| QCHG/QDIS | IRLZ44N ou equivalente | MOSFETs potencia | Somente prototipo <=2 A, com driver e dissipacao |
| QBAL1..3 | AO3400A | Balanceamento | Um canal por celula |
| DRV1/2 | MCP1407 | Driver de gate | VDD apropriado; nao alimentar pelo GPIO |
| K1 | Omron G5LE-1A4 | Rele/precharge | Acionar por driver, nunca direto pelo GPIO |
| D1 | 1N4007 | Diodo de bobina | Paralelo a bobina do rele |
| UDRV | ULN2003A | Driver do rele | Entrada logica, saida para bobina |
## 2. Materiais passivos por canal

Para cada um dos tres taps de tensao:
| Qtd | Componente | Valor |
|---:|---|---:|
| 3 | RTOP1..3 | 150 kOhm, 1% |
| 3 | RBOT1..3 | 33 kOhm, 1% |
| 3 | RSER1..3 | 1 kOhm, 1% |
| 3 | CADC1..3 | 100 nF, X7R, 50 V |
| 6 | Dclamp | Schottky BAT54 ou rede equivalente |
| 3 | TP | Ponto de teste do ADC |

Para o NTC:
| Qtd | Componente | Valor |
|---:|---|---:|
| 1 | RNTC | 10 kOhm, 1% |
| 1 | NTC1 | 10 kOhm a 25 C |
| 1 | CNTC | 100 nF |
| 1 | RSNTC | 1 kOhm opcional em serie |
## 3. Identificacao das celulas e taps

```text
		     PACK 3S

 B- o---[ C1 ]---o B1 o---[ C2 ]---o B2 o---[ C3 ]---o B+
	 celula 1          celula 2          celula 3

 B- = PACK_B_MINUS / referencia / GND do fixture
 B1 = PACK_TAP_1 / entre C1 e C2
 B2 = PACK_TAP_2 / entre C2 e C3
 B+ = PACK_B_PLUS / positivo total do pack
```

Tensoes esperadas medidas contra B-:

| Ponto | Pack descarregado | Pack cheio |
|---|---:|---:|
| B- | 0 V | 0 V |
| B1 | aproximadamente 3.0 V | aproximadamente 4.2 V |
| B2 | aproximadamente 6.0 V | aproximadamente 8.4 V |
| B+ | aproximadamente 9.0 V | aproximadamente 12.6 V |

Nunca trocar B1 com B2. Nunca conectar B1, B2 ou B+ diretamente a um GPIO.
## 4. Esquema geral de conexao

```text
			 +------------------ PACK 3S PROTEGIDO ------------------+
			 |                                                        |
B- ----+--------------------------------------------------------+---- GND
B1 ----+--> divisor TAP1 --> GPIO36 / ADC1_CH0                 |
B2 ----+--> divisor TAP2 --> GPIO34 / ADC1_CH6                 |
B+ --F1+--> divisor TAP3 --> GPIO35 / ADC1_CH7                 |
			 |                                                        |
			 +--> AFE BQ76920EVM (protecao primaria)                 |
			 +--> PS2 buck --> 5 V --> ESP32/VCC da bancada          |
			 +--> LOAD1 por chave/atuador e fusivel                  |

 ESP32 DevKit
	 GPIO21 SDA -------- INA219 + MAX17048 + OLED SDA
	 GPIO22 SCL -------- INA219 + MAX17048 + OLED SCL
	 GPIO39 ADC1_CH3 --- divisor NTC 10k
	 GPIO26 ------------ driver gate MOSFET de carga
	 GPIO27 ------------ driver gate MOSFET de descarga
	 GPIO14 ------------ driver balanceamento
	 GPIO32 ------------ ULN2003A/rele precharge
	 GPIO5 CAN_TX ------ SN65HVD230 TXD
	 GPIO4 CAN_RX ------ SN65HVD230 RXD
```

## 5. Ligacao dos divisores de tensao

Montar tres circuitos iguais. O desenho de cada canal e:

```text
PACK_TAPx o--- RTOP 150k --- RSER 1k ---o--- ESP32 ADC
																				|
																			C 100nF
																				|
																			 GND
																				|
																	RBOT 33k
																				|
																			 GND

Adicionar clamp Schottky no no ADC:
	diodo para 3V3: anodo no ADC, catodo em 3V3
	diodo para GND: anodo em GND, catodo no ADC
```

Ligacoes dos tres canais:

| Canal | Entrada | RTOP | RSER | RBOT/C | GPIO |
|---|---|---|---|---|---|
| TAP1 | PACK_TAP_1/B1 | RTOP1 | RSER1 | RBOT1/CADC1 | GPIO36 / ADC1_CH0 |
| TAP2 | PACK_TAP_2/B2 | RTOP2 | RSER2 | RBOT2/CADC2 | GPIO34 / ADC1_CH6 |
| TAP3 | PACK_B_PLUS/B+ | RTOP3 | RSER3 | RBOT3/CADC3 | GPIO35 / ADC1_CH7 |

O divisor tem fator ideal `33/(150+33) = 0.18033`. Com 12.6 V no TAP3,
o ADC recebe aproximadamente 2.27 V. O firmware precisa converter taps em
celulas: `C1=B1`, `C2=B2-B1`, `C3=B+-B2`. O driver atual ainda precisa dessa
conversao diferencial antes de usar pack real.

## 6. Ligacao do NTC

```text
ESP32 3V3 o--- RNTC 10k ---o--- NTC1 10k --- GND
													 |
												 CNTC 100nF
													 |
													 +---- RSNTC 1k opcional ---- GPIO39 / ADC1_CH3
```

Fixar o NTC ao corpo do pack com fita/isolacao eletrica. Nao deixar o NTC
solto durante o ensaio termico.

## 7. Ligacao I2C

```text
ESP32 3V3 ----+---- VCC INA219
							+---- VCC MAX17048
							+---- VCC OLED

ESP32 GND ----+---- GND INA219
							+---- GND MAX17048
							+---- GND OLED

GPIO21 SDA ---+---- SDA INA219
							+---- SDA MAX17048
							+---- SDA OLED

GPIO22 SCL ---+---- SCL INA219
							+---- SCL MAX17048
							+---- SCL OLED
```

Enderecos esperados: INA219 `0x40`, MAX17048 `0x36`, OLED `0x3C`. Confirmar
com scanner I2C. Usar somente uma combinacao efetiva de pull-ups por SDA/SCL.

## 8. Ligacao de corrente: escolher uma opcao

### Opcao A - INA219, compativel com o codigo atual

```text
PACK_B_PLUS/F1 ---- VIN+ INA219
VIN- INA219 ------- entrada positiva de carga/descarga
INA219 VCC -------- ESP32 3V3
INA219 GND -------- GND comum
INA219 SDA/SCL ---- GPIO21/GPIO22
```

O shunt deve ser o valor configurado no modulo. Se o modulo usar outro shunt,
alterar calibracao e limites antes do teste.

### Opcao B - INA226

Nao instalar INA219 e INA226 no mesmo endereco sem planejamento. INA226 exige
driver proprio no firmware, configuracao de endereco e calibracao do shunt. A
opcao A e a escolha correta para o primeiro teste deste codigo.

## 9. Ligacao dos MOSFETs e drivers

Nunca ligar GPIO diretamente ao gate de potencia:

```text
GPIO26 -- resistor 100R --> IN driver carga
GPIO27 -- resistor 100R --> IN driver descarga
GPIO14 -- resistor 100R --> IN driver balance

Cada entrada de driver:
	entrada --- resistor 10k --- GND
	VDD driver --- alimentacao especificada no datasheet
	GND driver --- GND de controle
	OUT driver --- gate do MOSFET
	source/drain MOSFET --- caminho de potencia com fusivel
```

O estado seguro deve ser OFF com ESP32 desligado, em reset e durante boot.
Testar sem pack e sem carga antes de inserir os MOSFETs.

## 10. Ligacao do rele/precharge

```text
GPIO32 -------- entrada ULN2003A
ULN2003A GND -- ESP32 GND
ULN2003A COM -- +5 V da bobina
ULN2003A OUT -- lado negativo da bobina K1
K1 lado positivo -- +5 V
1N4007 em paralelo com K1:
	catodo no +5 V, anodo no lado OUT/negativo
```

O contacto do rele deve comandar o circuito de precharge, nunca transportar
corrente acima da especificacao. Usar chave de emergencia em serie com a
potencia.

## 11. Ligacao CAN

```text
ESP32 3V3 ------ VCC SN65HVD230
ESP32 GND ------ GND SN65HVD230
GPIO5 ---------- TXD SN65HVD230
GPIO4 ---------- RXD SN65HVD230
SN65 CANH ------ CANH do CANable
SN65 CANL ------ CANL do CANable
GND ------------ GND do CANable
```

Usar terminacao de 120 Ohm somente nas duas extremidades do barramento. Para
uma bancada ponto-a-ponto, habilitar uma terminacao em cada extremidade.

## 12. Sequencia segura de montagem

1. Montar ESP32, buck e GND; nao conectar pack.
2. Medir 5 V e 3.3 V com multimetro.
3. Gravar o firmware e confirmar boot.
4. Montar I2C e confirmar enderecos 0x40, 0x36 e 0x3C.
5. Montar os divisores sem ligar aos taps.
6. Aplicar tensoes de teste limitadas nos divisores e conferir ADC.
7. Montar NTC e confirmar leitura de temperatura.
8. Montar drivers sem MOSFETs; confirmar comandos OFF.
9. Montar CAN e testar com CANable.
10. Inserir AFE/BMS externo, fusivel, corte de emergencia e fonte limitada.
11. Medir B-, B1, B2 e B+ do pack desligado do ESP32.
12. Conectar taps um por vez, na ordem B-, B1, B2, B+.
13. Confirmar valores no multimetro e no serial.
14. Aplicar carga/descarga inicialmente a 0.10 A.
15. Parar imediatamente se houver aquecimento, cheiro, ruido ou tensao anormal.

## 13. Checklist de desligamento

- Desligar carga e carregador.
- Abrir SW1/corte de emergencia.
- Remover alimentacao do pack.
- Esperar capacitores descarregarem.
- Medir ausencia de tensao antes de alterar fios.
- Nunca trocar fios com o pack energizado.

## 14. Bloqueios antes do pack real

- O driver ADC diferencial ainda deve ser implementado e calibrado.
- O BQ76920EVM deve ser configurado e testado como protecao primaria.
- Os valores do INA219/shunt precisam ser confirmados no modulo comprado.
- A montagem deve usar placa perfurada/PCB e bornes, nunca protoboard para potencia.
- O ensaio deve seguir o [plano de bancada](../BENCH_TEST_PLAN_3S.md).
ESP32 DevKitC-32E
Fonte Korad KA3005P
Carga eletrônica ZKETECH EBD-A20H
Buck Pololu D24V10F5
AFE TI BQ76920EVM
Alternativa Analog Devices LTC6811
Resistores Yageo com part numbers
Capacitores Murata
NTC TDK/EPCOS B57891M0103J000
INA226 TI com shunt Vishay de 1 mOhm
INA219 Adafruit como alternativa limitada
OLED Adafruit #938
MAX17048 SparkFun
Transceiver CAN TI SN65HVD230
CANable 2.0
MOSFETs IRLZ44N e AO3400A
Drivers MCP1407
Relé Omron G5LE-1A4
Fusível Littelfuse
Multímetro, osciloscópio, fonte, carga e acessórios
Também corrigi a especificação antiga de pinagem:

CELL1: GPIO36 / ADC1_CH0
CELL2: GPIO34 / ADC1_CH6
CELL3: GPIO35 / ADC1_CH7
NTC: GPIO39 / ADC1_CH3
