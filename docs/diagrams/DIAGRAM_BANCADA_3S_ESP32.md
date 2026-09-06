# Desenho de bancada - pack 3S + ESP32 DevKit

**Uso:** montagem experimental supervisionada, corrente limitada a 2 A.
**Nao e circuito de protecao primaria de bateria.**

## 1. Vista funcional

```text
             PACK 3S PROTEGIDO

   B+ o--------------------[F1 fusivel]-----+---- carga/descarga
      |                                      |
     C3                                      +-- entrada B+ do AFE/BMS externo
      |
   B2 o-------------------------------------+
      |                                      +-- TAP3 / medicao
     C2
      |
   B1 o-------------------------------------+
      |                                      +-- TAP2 / medicao
     C1
      |
   B- o--------------------[K1 corte]-------+---- GND comum da bancada
                                            |
                                            +-- TAP1 / medicao

   TAP1              TAP2              TAP3
    |                 |                 |
 [150k/33k]       [150k/33k]       [150k/33k]
     |                 |                 |
 [1k + 100nF]     [1k + 100nF]     [1k + 100nF]
    |                 |                 |
 GPIO36/ADC1_CH0  GPIO34/ADC1_CH6  GPIO35/ADC1_CH7

                       +----------------------+
 USB 5 V -------------| ESP32 DevKit        |
 GND comum -----------| GND                  |
                       | GPIO39 ADC1_CH3 <- NTC 10k
                       | GPIO21 SDA ---------+-- INA219
                       | GPIO22 SCL ---------+-- MAX17048
                       |                     +-- OLED SSD1306
                       | GPIO26 -------------> driver MOSFET carga
                       | GPIO27 -------------> driver MOSFET descarga
                       | GPIO14 -------------> driver balanceamento
                       | GPIO32 -------------> driver K1/precharge
                       | GPIO5 TX -----------> transceiver CAN
                       | GPIO4 RX <----------- transceiver CAN
                       +----------------------+
```

## 2. Vista dos divisores de entrada

Repetir tres vezes, sempre com protecao e ponto de teste:

```text
TAP do pack o---RTOP 150k---Rserie 1k---o------> ADC ESP32
                                     |    |
                                    100nF Rbot 33k
                                     |    |
                                    GND  GND

ADC protection: clamp Schottky para 3V3/GND ou rede aprovada.
```

A tensao do tap deve ser calculada no firmware por diferenca:

```text
cell1 = tap1
cell2 = tap2 - tap1
cell3 = tap3 - tap2
```

**Importante:** o driver atual `readCell(index)` ainda trata cada ADC como se
fosse uma tensao de celula individual. Antes de usar um pack 3S real, deve ser
implementado um driver de taps que faca as tres diferencas e aplique o ganho do
divisor. O desenho nao autoriza conectar este circuito ao pack sem essa revisao.

## 3. Circuito NTC

```text
3V3 o---Rserie 10k 1%---o---NTC 10k B3950---GND
                         |
                       100nF
                         |
                         +---- GPIO39 / ADC1_CH3
```

O canal foi reservado em `config/config.h` para nao colidir com os taps. GPIO34,
GPIO35, GPIO36 e GPIO39 sao entradas; confirme a variante exata da placa antes
da montagem.

## 4. Saida de potencia

```text
ESP32 GPIO26 --100R--+--> IN driver gate carga
                     +--10k--> GND
ESP32 GPIO27 --100R--+--> IN driver gate descarga
                     +--10k--> GND
ESP32 GPIO14 --100R--+--> IN driver balanceamento
                     +--10k--> GND

Driver OUT --> gate MOSFET
MOSFET source/drain --> caminho de potencia com fusivel
```

O driver deve garantir OFF durante reset, boot, perda de 3V3 e cabo USB
removido. Nao conectar GPIO diretamente ao gate de potencia.

## 5. Instrumentos na bancada

```text
Fonte limitada ---- fusivel ---- pack/fixture 3S
Carga eletronica ---------------- caminho de descarga
Multimetro ---------------------- B-, B1, B2, B+
USB ----------------------------- ESP32
USB-CAN ------------------------- transceiver CAN
PC/BLE -------------------------- diagnostico
Chave K1 ------------------------ corte fisico de emergencia
```

O AFE/BMS externo deve permanecer como protecao primaria durante os testes.
