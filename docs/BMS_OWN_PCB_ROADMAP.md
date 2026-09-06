# Arquitetura futura - PCB BMS propria 3S

**Fase 2:** projeto de produto
**Objetivo:** substituir o prototipo ESP32 DevKit por uma placa BMS integrada,
com protecao primaria independente e controlador substituivel.

## 1. Principios de projeto

O desenho de blocos da placa esta em [`DIAGRAM_PCB_BMS_3S.md`](DIAGRAM_PCB_BMS_3S.md).

1. O ESP32 e supervisor, comunicacao e interface; nao deve ser a unica barreira
   de seguranca.
2. O AFE BMS deve medir celulas e executar protecoes de sobrecarga, subtensao,
   sobrecorrente e temperatura mesmo se o firmware parar.
3. O caminho de potencia deve ter fusivel, desconexao e precharge dimensionados.
4. Falha de MCU, perda de I2C/SPI, brownout ou reset deve levar a estado seguro.
5. Medicao, potencia, RF e conectores devem ser separados fisicamente na PCB.
6. Cada valor critico deve ter tolerancia, derating e metodo de teste definido.

## 2. Blocos da placa

```text
Pack 3S
  |
  +-- fusivel e protecao contra inversao
  |
  +-- AFE BMS 3-5S -- taps de celula -- NTCs -- protecao primaria
  |        |
  |        +-- drivers de charge/discharge/balance
  |        +-- interrupt/alert para ESP32
  |
  +-- shunt ou sensor Hall -- medicao de corrente
  |
  +-- buck isolado/nosso GND definido -- 5 V / 3V3
           |
           +-- ESP32-WROOM ou MCU equivalente
           +-- CAN transceiver protegido
           +-- BLE/USB/service UART
           +-- memoria de eventos e parametros
           +-- debug/test points
```

## 3. Recomendacao de componentes por funcao

| Funcao | Prototipo | PCB propria - direcao |
|---|---|---|
| MCU | ESP32 DevKit | ESP32-WROOM com USB-UART e boot controlado |
| AFE 3S | divisor ADC experimental | BQ76920/BQ76930 ou AFE equivalente |
| Corrente | INA219 modulo | shunt Kelvin + INA226 ou sensor Hall |
| Temperatura | NTC 10k | 2-4 NTCs em entradas do AFE |
| Protecao | firmware + driver | AFE + driver de gate + hardware shutdown |
| Balanceamento | MOSFET/resistor | canais calculados termicamente por celula |
| Potencia | fios/modulos | cobre dimensionado, fusivel, precharge, conectores |
| CAN | transceiver externo | transceiver automotivo protegido e terminacao configuravel |
| Memoria | NVS | NVS + area de parametros versionada, CRC e fallback |

A escolha final depende da corrente, quimica, capacidade, ambiente e requisito
regulatorio do produto. Nao congelar part numbers sem esses dados.

## 4. Interfaces que o firmware deve preservar

- `IADC` deve evoluir para uma interface de pack/AFE que entregue tensoes
  individuais ja validadas e flags de qualidade.
- Corrente deve informar valor, sentido, validade e saturacao.
- Temperatura deve informar valor, sensor aberto/curto e validade.
- MOSFET driver deve ter feedback de estado quando possivel.
- AFE deve expor fault flags, alertas e estado de comunicacao.
- CAN/BLE devem usar payload versionado e diagnostico de compatibilidade.

A camada `core/` deve permanecer independente do part number. Somente `src/hal`
e a composicao da aplicacao devem conhecer o componente escolhido.

## 5. Requisitos preliminares da PCB

- Pack: 3S, faixa de operacao 9.0-12.6 V.
- Corrente nominal e pico: definir antes do esquematico; o prototipo atual e
  limitado a 2 A.
- Balanceamento: definir corrente de balanceamento e dissipacao por canal.
- Temperatura: limitar componentes de potencia por calculo e medicao.
- Creepage/clearance: definir conforme tensao, ambiente e norma aplicavel.
- Conectores: impedir inversao mecanica e separar taps de potencia.
- Test points: B-, B1, B2, B+, 3V3, GND, SDA, SCL, CANH, CANL, gates e shunt.
- Test fixture: pogo pins para programacao, leitura de taps, CAN e teste de
  entradas/saidas.
- EMC: retorno de corrente controlado, plano de GND, filtros, TVS e layout RF.

## 6. Entregaveis da fase 2

- [ ] Requisitos de produto: tensao, corrente, capacidade, quimica, ambiente.
- [ ] Diagrama de blocos aprovado.
- [ ] Esquematico eletrico revisado.
- [ ] Calculos de divisor, shunt, fusivel, MOSFET, trilhas e termica.
- [ ] BOM com fabricante, part number alternativo e ciclo de vida.
- [ ] PCB layout revisado por regras DRC/ERC.
- [ ] Gerbers, pick-and-place, desenho de montagem e pontos de teste.
- [ ] Firmware HAL para o AFE escolhido.
- [ ] Fixture de teste de fabricacao.
- [ ] Plano de bring-up e teste de producao.
- [ ] FMEA/DFMEA e matriz de riscos.
- [ ] Plano de certificacao aplicavel ao mercado alvo.

## 7. Estrategia de migracao

1. Validar o plano de bancada com ESP32 DevKit e AFE externo.
2. Medir erros reais e congelar requisitos de medicao.
3. Implementar `IAfeBatteryMonitor` no firmware mantendo o mock atual.
4. Criar testes nativos para tensoes, flags e falhas de comunicacao do AFE.
5. Montar primeira PCB com pontos de teste abundantes e sem compactacao extrema.
6. Fazer bring-up sem pack, depois com fonte limitada e depois com fixture.
7. Comparar PCB propria contra instrumento calibrado e prototipo de referencia.
8. Executar validacao ambiental, EMC, abuso e certificacao antes da producao.

## 8. Criterio de competitividade

Uma placa propria so deve ser considerada pronta para mercado quando entregar,
alem de funcionar:

- protecao primaria independente do ESP32;
- medicao rastreavel e calibrada;
- comportamento seguro em falhas de sensor/comunicacao/energia;
- diagnostico reproduzivel e historico persistente;
- BOM com disponibilidade e alternativas;
- teste de fim de linha automatizado;
- documentacao de revisao, lote e firmware;
- evidencias de normas e ensaios do mercado alvo.
