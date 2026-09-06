# Especificacao eletrica - BMS 3S com ESP32 DevKit

**Revisao:** 0.1 - prototipo de bancada
**Status:** somente bancada supervisionada
**Pack alvo:** 3 celulas Li-ion em serie (3S)
**Tensao:** 9.0 V a 12.6 V para celulas de 3.0 V a 4.2 V
**Corrente de projeto do prototipo:** limitada a 2 A

> Este documento nao transforma o ESP32 DevKit em um BMS certificado. O
> prototipo deve usar fonte/carga com limite de corrente, fusivel e supervisao.
> A protecao primaria contra sobrecarga, descarga profunda e curto deve ser um
> carregador/BMS comercial apropriado ou um AFE dedicado. O firmware e a camada
> de supervisao e comando nesta fase.

## 1. Arquitetura de bancada

O desenho detalhado esta em [`DIAGRAM_BANCADA_3S_ESP32.md`](DIAGRAM_BANCADA_3S_ESP32.md).

```text
                 +------------------+
Pack B- -------->| GND comum        |
Pack B1 -------->| divisor TAP1 ----|--> ESP32 ADC1_CH0 / GPIO36
Pack B2 -------->| divisor TAP2 ----|--> ESP32 ADC1_CH6 / GPIO34
Pack B+ -------->| divisor TAP3 ----|--> ESP32 ADC1_CH7 / GPIO35
                 |                  |
                 | ESP32 DevKit    |-- GPIO26 -> driver MOSFET carga
                 |                  |-- GPIO27 -> driver MOSFET descarga
                 |                  |-- GPIO14 -> driver balanceamento
                 |                  |-- GPIO32 -> rele/precharge
                 |                  |-- GPIO21/22 -> barramento I2C
                 +------------------+

I2C: INA219 + MAX17048 + OLED SSD1306
CAN: GPIO5 TX / GPIO4 RX -> transceiver 3.3 V -> barramento CAN
NTC: divisor NTC 10k -> ADC1_CH3 / GPIO39
```

O desenho acima e um diagrama funcional. Antes de fabricar ou energizar, ele
precisa virar esquematico CAD com netlist, valores, footprints, creepage,
clearance e revisao eletrica.

## 2. Regra critica de medicao

Nao conectar B1, B2 ou B+ diretamente a GPIO. As tensoes sao referenciadas ao
negativo do pack e podem exceder 3.3 V. Para o prototipo, medir cada tap do pack
por divisor independente e calcular no firmware:

```text
Vcell1 = Vtap1
Vcell2 = Vtap2 - Vtap1
Vcell3 = Vtap3 - Vtap2
```

Cada entrada deve ter resistor serie, resistor para GND, capacitor de filtro,
protecao contra sobretensao e ponto de teste. O firmware deve rejeitar tensoes
negativas, saltos impossiveis e combinacoes em que Vtap2 < Vtap1 ou Vtap3 < Vtap2.

### Divisor inicial recomendado para bancada

| Item | Valor inicial | Observacao |
|---|---:|---|
| Resistor superior de cada tap | 150 kOhm, 1%, 0.25 W | Tensao maxima do tap: 12.6 V |
| Resistor inferior de cada tap | 33 kOhm, 1% | Fator aproximado: 0.1803 |
| Capacitor no ADC | 100 nF, X7R | Filtro RC local |
| Resistor serie adicional | 1 kOhm | Limita corrente de falha |
| Clamp | diodos Schottky para 3V3/GND ou TVS adequado | Validar corrente de clamp |

Com 12.6 V no tap, o divisor produz aproximadamente 2.27 V. O valor final
somente deve ser aceito depois de medir o tap e o ADC com multimetro calibrado.
Os resistores nao substituem fusivel, AFE ou protecao de celula.

## 3. Alimentacao

- Alimentar o ESP32 por USB ou fonte regulada 5 V durante a primeira etapa.
- Nao alimentar o ESP32 diretamente pelo pack sem conversor buck regulado.
- Usar conversor buck 9-12.6 V para 5 V, corrente minima de 1 A, com fusivel de
  entrada e protecao contra inversao.
- Usar 3.3 V somente de regulador apropriado para sensores de baixa corrente;
  nao retirar corrente de carga dos pinos GPIO.
- Unir GND do ESP32 ao B- somente no prototipo comum nao isolado e com a fonte
  de bancada configurada para isso.

## 4. Saidas de potencia

GPIO do ESP32 nao deve comandar gate de MOSFET de potencia diretamente.
Usar driver de gate com resistor de entrada, resistor pull-down, limitacao de
VGS e estado definido durante reset. Para bancada de ate 2 A:

- driver low-side ou high-side apropriado ao MOSFET escolhido;
- resistor de gate de 47-220 Ohm;
- pull-down de 10 kOhm;
- MOSFET logic-level com RDS(on) especificado em VGS real;
- fusivel em serie com o caminho de corrente;
- diodo/TVS para cargas indutivas e rele;
- contato de emergencia que remova a energia independentemente do firmware.

O comando de carga, descarga e balanceamento deve iniciar em OFF por hardware.

## 5. Interfaces e pinagem do prototipo

| Funcao | ESP32 DevKit | Hardware |
|---|---:|---|
| I2C SDA | GPIO21 | INA219, MAX17048, OLED |
| I2C SCL | GPIO22 | INA219, MAX17048, OLED |
| MOSFET carga | GPIO26 | entrada do driver de gate |
| MOSFET descarga | GPIO27 | entrada do driver de gate |
| Balanceamento | GPIO14 | driver do canal de balanceamento |
| Rele/precharge | GPIO32 | driver de rele |
| CAN TX | GPIO5 | transceiver TWAI 3.3 V |
| CAN RX | GPIO4 | transceiver TWAI 3.3 V |
| NTC | conforme `HW_NTC_ADC_CHANNEL` | divisor NTC |
| ADC tap 1/2/3 | ADC1 dedicado | divisores dos taps |

O firmware reserva ADC1_CH0/6/7 para os taps e ADC1_CH3/GPIO39 para o NTC.
Confirmar o mapeamento da variante exata do ESP32 antes da montagem.

## 6. Limites desta fase

- Pack 3S apenas.
- Corrente limitada a 2 A.
- Sem carga automatica de lítio sem carregador apropriado.
- Sem operacao sem supervisao.
- Sem declaracao de seguranca funcional ou certificacao.
- O ADC do DevKit serve para observacao/calibracao; o desligamento de emergencia
  deve existir independentemente no carregador/BMS de bancada.
