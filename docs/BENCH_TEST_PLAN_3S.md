# Plano de teste de bancada - BMS 3S ESP32

**Fase:** 1 - prototipo supervisionado
**Pre-condicao:** revisar `HARDWARE_3S_ESP32_PROTOTYPE.md` e `BOM_3S_BENCH.md`
**Resultado esperado:** evidencias para decidir a passagem para a fase 2

## Regras de seguranca

1. Trabalhar em bancada nao inflamavel e ventilada.
2. Usar fonte com limite de corrente e fusivel no pack.
3. Ter chave de corte fisico ao alcance.
4. Medir cada tap com multimetro antes de conectar ao ESP32.
5. Nunca carregar celulas sem carregador 3S apropriado ou AFE/BMS de protecao.
6. Interromper o teste se houver aquecimento, cheiro, inchaco, ruido eletrico
   anormal ou tensao fora do previsto.
7. Nao deixar o pack energizado sem supervisao.

## Instrumentos e registro

Registrar em cada ensaio:

- data, operador, identificacao das celulas e firmware;
- tensao B-, B1, B2 e B+ medida no multimetro;
- tensao reportada pelo ESP32 para cada celula;
- corrente da fonte, corrente do INA219 e corrente da carga;
- temperaturas NTC e ambiente;
- estado dos MOSFETs, DTC, watchdog e motivo de reset;
- fotos da montagem e arquivo serial bruto.

## Etapa A - inspeção sem energia

- [ ] Conferir polaridade e continuidade do pack.
- [ ] Conferir fusivel, chave de corte e bornes.
- [ ] Confirmar que nenhum tap chega diretamente ao GPIO.
- [ ] Confirmar divisores, resistores serie, capacitores e clamps.
- [ ] Confirmar pull-downs dos drivers e MOSFETs em OFF.
- [ ] Conferir ausencia de curto entre 3V3, 5V e GND.
- [ ] Fotografar a montagem antes de energizar.

**Aceite:** nenhum curto, nenhuma polaridade invertida e comando de potencia em
OFF sem alimentacao do ESP32.

## Etapa B - ESP32 sem pack

1. Gravar `pio run -e esp32dev -t upload`.
2. Abrir `pio device monitor -b 115200`.
3. Confirmar causa de reset, POST e estado seguro.
4. Confirmar INA219, MAX17048, OLED, CAN e BLE conforme o hardware instalado.
5. Confirmar que carga, descarga e balanceamento permanecem OFF.
6. Reiniciar pelo botao EN e guardar o log.

**Aceite:** boot estavel, sem periferico critico inesperadamente em FAIL e saídas
em estado seguro.

## Etapa C - fonte sem celula

- Alimentar apenas o buck e o ESP32.
- Aplicar tensoes simuladas aos divisores com fonte limitada, nunca excedendo
  a tensao de entrada calculada.
- Medir a saida de cada divisor e comparar com o valor previsto.
- Calibrar offset/gain somente depois de registrar pelo menos 5 pontos por canal.

**Aceite inicial:** erro de medicao definido pelo projeto e documentado. Para a
fase de prototipo, usar como meta preliminar <=2% antes da calibracao e <=1%
apos calibracao. A meta final deve ser definida pelo AFE e pelo produto.

## Etapa D - pack 3S em repouso

1. Usar pack protegido e balanceado, com tensao total entre 9.0 V e 12.6 V.
2. Medir B-, B1, B2 e B+ no multimetro.
3. Conectar os taps ao front-end desligado.
4. Energizar o ESP32 por USB/fonte separada.
5. Comparar cada celula calculada pelo firmware com o multimetro.
6. Confirmar plausibilidade: `Vtap3 >= Vtap2 >= Vtap1 >= 0`.
7. Confirmar que o historico e os DTCs nao crescem sem evento.

**Aceite:** sem aquecimento, sem reset, erro dentro da meta e nenhum comando de
potencia ativo por engano.

## Etapa E - corrente e temperatura

- Aplicar carga limitada inicialmente a 0.1 A, depois 0.5 A e 1 A.
- Comparar INA219 com multimetro/shunt ou carga eletronica.
- Fixar o NTC no ponto termico definido e comparar com termometro.
- Confirmar corrente positiva em carga e negativa em descarga, conforme contrato.
- Registrar temperatura do shunt, MOSFET e cabos.

**Aceite:** erro e aquecimento dentro dos limites definidos no plano de produto.

## Etapa F - protecoes com fonte/carga controladas

Executar uma condicao por vez, com limite de corrente e plano de abortar:

| Cenario | Metodo | Aceite |
|---|---|---|
| Sobretensao | tensao simulada acima de 4.20 V | DTC, carga OFF, log |
| Subtensao | tensao simulada abaixo de 3.00 V | DTC, descarga OFF, log |
| Temperatura alta | simulacao/resistor NTC controlado | DTC e saídas seguras |
| Sobrecorrente | carga acima do limite configurado | DTC e saídas seguras |
| Curto | somente com fixture e limitacao apropriados | corte independente e sem dano |
| Sensor aberto | desconectar pelo fixture | `SENSOR_FAILURE`, estado seguro |

Nao provocar curto diretamente em um pack sem fixture de teste projetado.

## Etapa G - watchdog e reset

- Confirmar heartbeat normal por pelo menos 10 minutos.
- Em firmware de teste controlado, bloquear o loop por mais de 2 s.
- Confirmar reset pelo TWDT e mensagem `RESET REASON`.
- Repetir power-cycle e brownout controlado, se a fonte possuir esse recurso.
- Confirmar que as saídas retornam a OFF depois do reset.

## Etapa H - comunicacao

- CAN: validar 500 kbit/s, heartbeat `0x100`, pack `0x101`, DLC, endianess e
  recepcao de comando invalido.
- BLE: descobrir servicos, ler tensao, corrente, temperatura, SOC/SOH e DTC.
- OLED: conferir unidade, faixa, estado e alarme sem sobreposicao.

## Gate de passagem para fase 2

Somente passar para projeto de PCB própria quando:

- todos os itens A-H tiverem evidencias anexadas;
- nenhum reset inexplicado ocorrer;
- medicao de tensão tiver erro conhecido e aceitável;
- protecoes tiverem sido testadas sem depender apenas do firmware;
- componentes de potencia nao excederem temperatura definida;
- esquematico, BOM e logs forem revisados por outra pessoa;
- riscos abertos tiverem dono e plano de mitigacao.
