# Netlist de montagem - bancada 3S ESP32

**Revisao:** 0.2
**Uso:** guia de fios, bornes e pontos de teste para a bancada.
**Corrente maxima da primeira montagem:** 2 A com fusivel e fonte limitada.

## 1. Bornes do pack

| Rede | Origem | Destino | Teste esperado |
|---|---|---|---|
| `PACK_B_MINUS` | negativo da celula 1 | B-, GND comum do fixture | 0 V referencia |
| `PACK_TAP_1` | entre C1 e C2 | divisor TAP1 / AFE CELL1 | 3.0-4.2 V vs B- |
| `PACK_TAP_2` | entre C2 e C3 | divisor TAP2 / AFE CELL2 | 6.0-8.4 V vs B- |
| `PACK_B_PLUS` | positivo da celula 3 | fusivel, divisor TAP3, AFE CELL3 | 9.0-12.6 V vs B- |

Nunca trocar `PACK_TAP_1`, `PACK_TAP_2` e `PACK_B_PLUS`. Usar conectores
polarizados e etiquetas gravadas.

## 2. Rede de medicao dos taps

| Rede | RTOP | RSERIE | RBOT | C filtro | ESP32 |
|---|---:|---:|---:|---:|---|
| `ADC_TAP1` | 150 kOhm | 1 kOhm | 33 kOhm | 100 nF | GPIO36 / ADC1_CH0 |
| `ADC_TAP2` | 150 kOhm | 1 kOhm | 33 kOhm | 100 nF | GPIO34 / ADC1_CH6 |
| `ADC_TAP3` | 150 kOhm | 1 kOhm | 33 kOhm | 100 nF | GPIO35 / ADC1_CH7 |
| `ADC_NTC1` | 10 kOhm | 1 kOhm opcional | NTC 10 kOhm | 100 nF | GPIO39 / ADC1_CH3 |

Cada `ADC_TAPx` deve possuir clamp para 3V3/GND, ponto de teste e trilha
separada de potencia. O valor do divisor deve ser confirmado por medicao.

## 3. Barramento I2C

```text
ESP32 GPIO21 / I2C_SDA ----+---- INA219 SDA
                           +---- MAX17048 SDA
                           +---- OLED SDA

ESP32 GPIO22 / I2C_SCL ----+---- INA219 SCL
                           +---- MAX17048 SCL
                           +---- OLED SCL

ESP32 3V3 ------------------+---- VCC dos modulos 3.3 V
ESP32 GND ------------------+---- GND dos modulos
```

Confirmar que existe somente uma terminacao de pull-up efetiva por linha ou
calcular a resistencia equivalente. Conferir enderecos I2C antes de ligar os
modulos juntos.

## 4. Controle de potencia

| Rede | GPIO | Elementos | Estado seguro |
|---|---:|---|---|
| `GATE_CMD_CHARGE` | 26 | resistor 100 Ohm -> driver -> MOSFET carga | OFF |
| `GATE_CMD_DISCHARGE` | 27 | resistor 100 Ohm -> driver -> MOSFET descarga | OFF |
| `GATE_CMD_BALANCE` | 14 | resistor 100 Ohm -> driver -> MOSFET balance | OFF |
| `PRECHARGE_CMD` | 32 | driver de rele/precharge | OFF |

Cada comando deve ter pull-down de 10 kOhm no lado da entrada do driver. O
ESP32 nunca deve ser o caminho de corrente da bobina ou do gate de potencia.

## 5. CAN e servico

```text
ESP32 GPIO5 / CAN_TX ----> TXD transceiver CAN 3.3 V
ESP32 GPIO4 / CAN_RX <---- RXD transceiver CAN 3.3 V
ESP32 3V3 -----------------> VCC transceiver
GND -----------------------> GND transceiver
CANH/CANL ------------------> conector J_CAN + terminacao configuravel 120R
```

Adicionar TVS CAN, resistencia de terminacao selecionavel e ponto de teste
CANH/CANL. Nao conectar CANH/CANL diretamente ao ESP32.

## 6. Pontos de teste obrigatorios

```text
TP1  PACK_B_MINUS / GND
TP2  PACK_TAP_1
TP3  PACK_TAP_2
TP4  PACK_B_PLUS
TP5  ADC_TAP1
TP6  ADC_TAP2
TP7  ADC_TAP3
TP8  ADC_NTC1
TP9  3V3
TP10 5V_USB
TP11 CANH
TP12 CANL
TP13 GATE_CMD_CHARGE
TP14 GATE_CMD_DISCHARGE
TP15 GATE_CMD_BALANCE
```

## 7. Sequencia de montagem

1. Montar ESP32, buck e GND sem pack.
2. Medir 3V3 e 5V.
3. Montar I2C e confirmar enderecos.
4. Montar divisores sem conectar aos taps.
5. Aplicar tensoes de teste limitadas nos divisores.
6. Confirmar ADC e NTC no monitor serial.
7. Montar drivers sem MOSFET de potencia.
8. Confirmar todos os comandos em OFF no boot/reset.
9. Inserir fusivel, fixture de corrente e AFE/BMS externo.
10. Conectar o pack protegido somente após checklist assinado.

## 8. Limite de validade

Esta netlist nao substitui um esquematico CAD. Nao usar para producao, carga
sem supervisao, pack de alta corrente ou declaracao de conformidade.
