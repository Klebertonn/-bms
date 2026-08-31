# ICD-015 – Display Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001 a ICD-014

---

# 1. Objetivo

O Display Manager gerencia toda a interface visual do BMS em displays OLED, LCD ou TFT, fornecendo ao usuário informações em tempo real sobre o estado da bateria, alarmes, comunicação e diagnóstico.

O módulo deve ser independente da tecnologia do display, permitindo futura substituição do hardware sem alterar a lógica do firmware.

---

# 2. Responsabilidades

O Display Manager deverá:

* Inicializar o display.
* Atualizar informações periodicamente.
* Exibir alarmes críticos.
* Alternar entre diferentes telas.
* Exibir mensagens de diagnóstico.
* Integrar-se ao Logger e ao Fault Manager.

---

# 3. Estruturas

## DisplayType

```cpp id="disp001"
enum class DisplayType
{
    OLED_SSD1306,
    LCD_16X2,
    LCD_20X4,
    TFT_ST7789,
    TFT_ILI9341
};
```

---

## DisplayScreen

```cpp id="disp002"
enum class DisplayScreen
{
    HOME,
    BATTERY,
    CELLS,
    TEMPERATURE,
    PROTECTION,
    BALANCING,
    COMMUNICATION,
    FAULTS,
    DIAGNOSTICS,
    SETTINGS
};
```

---

## DisplayStatus

```cpp id="disp003"
struct DisplayStatus
{
    bool initialized;
    DisplayType type;
    DisplayScreen currentScreen;
    bool backlightEnabled;
    uint32_t refreshRateMs;
};
```

---

# 4. Interface Pública

## Inicialização

```cpp id="disp004"
bool displayInit(DisplayType type);
```

---

## Atualização

```cpp id="disp005"
void displayUpdate();
```

---

## Alterar Tela

```cpp id="disp006"
bool displaySetScreen(DisplayScreen screen);
```

---

## Exibir Mensagem

```cpp id="disp007"
void displayShowMessage(const char* message);
```

---

## Exibir Alarme

```cpp id="disp008"
void displayShowAlarm(const char* alarm);
```

---

## Ajustar Brilho

```cpp id="disp009"
bool displaySetBrightness(uint8_t level);
```

---

## Obter Status

```cpp id="disp010"
DisplayStatus displayGetStatus();
```

---

## Executar Autoteste

```cpp id="disp011"
bool displaySelfTest();
```

---

# 5. Telas Padrão

### HOME

* Estado do sistema.
* SOC.
* SOH.
* Tensão do pack.
* Corrente.
* Temperatura.

---

### BATTERY

* Tensão total.
* Corrente.
* Potência.
* Energia acumulada.

---

### CELLS

* Tensão de cada célula.
* Maior tensão.
* Menor tensão.
* Delta entre células.

---

### TEMPERATURE

* Temperatura de cada sensor.
* Temperatura máxima.
* Temperatura mínima.

---

### PROTECTION

* Proteções ativas.
* Estado dos MOSFETs.
* Limites configurados.

---

### BALANCING

* Células em balanceamento.
* Tempo de balanceamento.
* Delta de tensão.

---

### COMMUNICATION

* Estado do BLE.
* Estado do CAN.
* RSSI BLE.
* Frames CAN enviados/recebidos.

---

### FAULTS

* Falhas ativas.
* Histórico resumido.
* Severidade.

---

### DIAGNOSTICS

* Uso de CPU.
* Stack das tarefas.
* Watchdog.
* Memória RAM.
* Espaço em armazenamento.

---

### SETTINGS

* Perfil ativo.
* Versão do firmware.
* Número de série.
* Configurações principais.

---

# 6. Fluxo de Operação

```text id="disp012"
Inicialização
      │
      ▼
Autoteste
      │
      ▼
Tela HOME
      │
      ▼
Atualizações periódicas
      │
 ┌────┴─────────────┐
 │                  │
 ▼                  ▼
Evento normal   Alarme crítico
 │                  │
 ▼                  ▼
Atualizar tela  Exibir alerta
```

---

# 7. Integração

O Display Manager consulta informações de:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Communication Manager.
* Logger.
* Fault Manager.
* Task Manager.
* Watchdog Manager.
* Configuration Manager.

---

# 8. Tratamento de Erros

Caso o display apresente falha:

* Registrar evento no Logger.
* Informar o Fault Manager.
* Continuar operação do BMS sem interface visual.
* Permitir acesso às informações via CLI, BLE e CAN.

---

# 9. Requisitos

* Compatível com diferentes controladores de display.
* Atualização não bloqueante.
* Baixo consumo de memória.
* Compatível com FreeRTOS.
* Interface extensível para novas telas.

---

# 10. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Exibir corretamente todas as telas.
* Atualizar dados em tempo real.
* Apresentar alarmes críticos imediatamente.
* Operar em Mock, Wokwi e ESP32 físico.

---

# 11. Evoluções Futuras

* Interface gráfica colorida.
* Temas configuráveis.
* Gráficos históricos.
* Navegação por encoder rotativo ou touchscreen.
* Suporte multilíngue.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                                  |
| ------ | ---------- | ------------------------------------------ |
| 1.0.0  | 04/07/2026 | Primeira definição da Display Manager API. |

