# ICD-001 – Battery Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-003

---

# 1. Objetivo

Este documento define a interface pública do Battery Manager, responsável pelo gerenciamento do estado da bateria, cálculo de SOC, SOH, SOE e disponibilização das informações aos demais módulos.

---

# 2. Responsabilidades

O Battery Manager deverá:

* Ler informações provenientes da HAL.
* Atualizar o estado interno da bateria.
* Calcular SOC.
* Calcular SOH.
* Calcular SOE.
* Calcular tensão do pack.
* Calcular corrente do pack.
* Calcular potência.
* Disponibilizar os dados para os demais módulos.

---

# 3. Estruturas de Dados

## BatteryCell

```cpp
struct BatteryCell
{
    float voltage;
    float temperature;
    bool balancing;
};
```

---

## BatteryPack

```cpp
struct BatteryPack
{
    float packVoltage;
    float packCurrent;
    float packPower;

    float soc;
    float soh;
    float soe;

    uint8_t cellCount;

    BatteryCell cells[MAX_CELLS];
};
```

---

# 4. Enumerações

## BatteryState

```cpp
enum class BatteryState
{
    OFF,
    IDLE,
    CHARGING,
    DISCHARGING,
    BALANCING,
    FAULT
};
```

---

# 5. Interface Pública

## Inicialização

```cpp
void batteryInit();
```

Inicializa o módulo.

---

## Atualização

```cpp
void batteryUpdate();
```

Atualiza todas as medições.

---

## Obter Estado

```cpp
BatteryState batteryGetState();
```

---

## Obter Estrutura Completa

```cpp
const BatteryPack& batteryGetData();
```

---

## Obter SOC

```cpp
float batteryGetSOC();
```

---

## Obter SOH

```cpp
float batteryGetSOH();
```

---

## Obter SOE

```cpp
float batteryGetSOE();
```

---

## Obter Corrente

```cpp
float batteryGetCurrent();
```

---

## Obter Potência

```cpp
float batteryGetPower();
```

---

## Obter Tensão

```cpp
float batteryGetVoltage();
```

---

## Obter Célula

```cpp
BatteryCell batteryGetCell(uint8_t index);
```

---

# 6. Fluxo de Operação

```text
batteryInit()

↓

batteryUpdate()

↓

HAL

↓

Atualiza BatteryPack

↓

Protection Manager

↓

Balance Manager

↓

Communication Manager

↓

Logger

↓

Display
```

---

# 7. Dependências

O Battery Manager utiliza:

* HAL
* Configuration Manager
* Logger

---

# 8. Módulos Consumidores

Os seguintes módulos utilizam esta API:

* Protection Manager
* Balance Manager
* Communication Manager
* Display
* Logger
* CLI
* BLE
* CAN

---

# 9. Tratamento de Erros

Em caso de erro de leitura:

* Registrar evento no Logger.
* Atualizar Fault Manager.
* Retornar último valor válido quando possível.

---

# 10. Requisitos

* Interface thread-safe.
* Compatível com FreeRTOS.
* Não bloquear outras tarefas.
* Tempo de execução determinístico.

---

# 11. Critérios de Aceitação

A API será considerada estável quando:

* Todos os dados puderem ser consultados pelos módulos consumidores.
* Não houver dependências circulares.
* Os testes unitários forem aprovados.
* O funcionamento for validado em Mock, Wokwi e ESP32 físico.

---

# 12. Evoluções Futuras

A API deverá permitir:

* Suporte a múltiplos packs.
* Diferentes químicas de bateria.
* Packs em paralelo.
* Packs em série.
* Baterias modulares.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                     |
| ------ | ---------- | --------------------------------------------- |
| 1.0.0  | 03/07/2026 | Primeira definição da API do Battery Manager. |

