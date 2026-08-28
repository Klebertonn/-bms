# Guia de Execucao Local e Instalacao no ESP32

Este guia descreve como executar o BMS no PC, abrir o dashboard local e gravar o firmware em uma placa ESP32.

> **Importante:** o projeto esta configurado atualmente para `PACK_CELL_COUNT = 3`, ou seja, um pack 3S. Nao conecte uma unica celula diretamente sem antes ajustar a configuracao e validar o circuito de medicao.

## 1. Pre-requisitos

- VS Code
- Extensao PlatformIO IDE
- Cabo USB de dados
- Placa ESP32 compativel com `esp32dev`
- Driver USB da placa, se o Windows nao reconhecer a porta serial
- PlatformIO instalado pela extensao ou pelo CLI

Abra o terminal na raiz do projeto:

```powershell
cd "C:\Users\Klebe\OneDrive\Desktop\BMSPC2\-bms"
```

## 2. Executar no PC

O ambiente `native` executa a logica em modo simulado, sem hardware fisico.

### Compilar

```powershell
pio run -e native
```

### Executar os testes

```powershell
pio test -e native
```

O ultimo resultado validado foi:

- 34 casos de teste executados
- 34 casos aprovados
- 0 falhas

## 3. Abrir o dashboard local

O dashboard possui um modo demo com dados simulados.

Abra este arquivo no navegador:

```text
web/index.html
```

Tambem e possivel clicar com o botao direito em `web/index.html` no VS Code e escolher **Open with Default Browser**.

O modo demo nao exige ESP32. Ele serve para apresentacao e verificacao visual local.

## 4. Compilar o firmware para ESP32

Com a placa ainda sem bateria conectada, compile o firmware real:

```powershell
pio run -e esp32dev
```

O ambiente `esp32dev` usa:

- Framework Arduino
- Placa `esp32dev`
- Monitor serial em `115200` baud
- INA219 para corrente
- MAX17048 para SOC
- OLED SSD1306
- CAN TWAI
- BLE

## 5. Gravar o firmware

1. Conecte o ESP32 ao PC por USB.
2. Aguarde o Windows reconhecer a porta COM.
3. No terminal, execute:

```powershell
pio run -e esp32dev -t upload
```

Para gravar e abrir o monitor serial:

```powershell
pio run -e esp32dev -t upload; pio device monitor -b 115200
```

No VS Code, as mesmas tarefas podem ser executadas pelos botoes de **Build**, **Upload** e **Monitor** na barra do PlatformIO.

Se o upload ficar aguardando a placa:

1. Mantenha pressionado o botao `BOOT`.
2. Inicie o upload.
3. Solte `BOOT` quando a gravacao comecar.

## 6. Verificar o boot

Com o monitor serial aberto, pressione `EN` ou reinicie a placa. O firmware deve mostrar mensagens semelhantes a:

```text
[BMS] ESP32 boot...
[HW] INA219 OK
[HW] MAX17048 OK
[HW] OLED OK
[HW] CAN OK
[HW] BLE OK
[BMS] READY
```

Mensagens `FAIL` indicam que o periferico nao foi detectado, esta sem alimentacao, possui endereco I2C diferente ou ainda nao esta conectado. Nao ignore falhas de sensores antes de testar a bateria.

## 7. Bancada inicial sem pack real

A ordem recomendada e:

1. Gravar o ESP32 sem bateria conectada.
2. Confirmar o boot pelo monitor serial.
3. Testar I2C com INA219, OLED e MAX17048.
4. Testar temperatura com o NTC.
5. Confirmar que os MOSFETs iniciam desligados.
6. Usar uma fonte de laboratorio com limite de corrente.
7. Medir as tensoes com multimetro antes de habilitar carga ou descarga.
8. Somente depois conectar uma celula ou pack de teste protegido.

## 8. Seguranca eletrica obrigatoria

- Nunca conecte uma celula de litio diretamente a um GPIO do ESP32.
- O ADC atual usa leitura direta e esta com `SCALE_FACTOR = 1.0f`.
- Uma celula de 4,2 V pode exceder a faixa segura do ADC, dependendo do circuito e da atenuacao.
- Use divisor de tensao ou circuito dedicado de monitoramento, dimensionado e medido com multimetro.
- Use fusivel, limitacao de corrente, cabos adequados e suporte para a celula.
- Nao carregue uma celula sem carregador apropriado e supervisao.
- Nao deixe a bancada ligada sem supervisao.
- Para uma unica celula, altere e valide `PACK_CELL_COUNT`, os limites de pack, o SOC e o mapeamento do ADC antes de testar.

## 9. Checklist rapido

- [ ] `pio run -e native` passou
- [ ] `pio test -e native` passou
- [ ] Dashboard `web/index.html` abriu no navegador
- [ ] `pio run -e esp32dev` compilou
- [ ] Firmware foi gravado no ESP32
- [ ] Monitor serial abriu em `115200`
- [ ] Boot foi confirmado sem bateria
- [ ] Sensores foram detectados
- [ ] ADC e divisor foram validados
- [ ] MOSFETs foram confirmados desligados
- [ ] Fonte limitada e fusivel foram instalados

Somente avance para um pack real depois que todos os itens de hardware e seguranca estiverem validados.
