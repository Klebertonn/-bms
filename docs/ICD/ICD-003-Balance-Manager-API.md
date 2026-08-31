# ICD-003 – Balance Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-006

---

# 1. Objetivo

Este documento define a interface pública do Balance Manager, responsável pelo controle do balanceamento das células do pack de baterias.

O módulo deverá operar de forma independente, utilizando informações fornecidas pelo Battery Manager e respeitando todas as restrições impostas pelo Protection Manager.

---

# 2. Responsabilidades

O Balance Manager deverá:

* Identificar células desbalanceadas.
* Selecionar as células a serem balanceadas.
* Acionar e desligar MOSFETs de balanceamento.
* Monitorar temperatura durante o balanceamento.
* Registrar eventos.
* Informar seu estado aos demais módulos.

---

# 3. Enumerações

## BalanceMode

```cpp id="bal001"
enum class BalanceMode
{
    DISABLED,
    PASSIVE,
    ACTIVE
};
```

---

## BalanceStrategy

```cpp id="bal002"
enum class BalanceStrategy
{
    SEQUENTIAL,
    ROUND_ROBIN,
    SIMULTANEOUS
};
```

---

## BalanceState

```cpp id="bal003"
enum class BalanceState
{
    IDLE,
    WAITING,
    BALANCING,
    PAUSED,
    COMPLETED,
    FAULT
};
```

---

# 4. Estruturas de Dados

## BalanceChannel

```cpp id="bal004"
struct BalanceChannel
{
    bool enabled;
    float cellVoltage;
    float temperature;
    uint32_t activeTimeMs;
};
```

---

## BalanceStatus

```cpp id="bal005"
struct BalanceStatus
{
    BalanceState state;
    BalanceMode mode;
    BalanceStrategy strategy;

    float voltageDelta;

    uint8_t activeCells;
    uint8_t currentCell;

    bool running;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="bal006"
void balanceInit();
```

---

## Atualização

```cpp id="bal007"
void balanceUpdate();
```

Executa um ciclo de balanceamento.

---

## Iniciar Balanceamento

```cpp id="bal008"
bool balanceStart();
```

---

## Parar Balanceamento

```cpp id="bal009"
void balanceStop();
```

---

## Obter Estado

```cpp id="bal010"
BalanceState balanceGetState();
```

---

## Obter Status Completo

```cpp id="bal011"
const BalanceStatus& balanceGetStatus();
```

---

## Obter Canal

```cpp id="bal012"
BalanceChannel balanceGetChannel(uint8_t cell);
```

---

## Verificar Execução

```cpp id="bal013"
bool balanceIsRunning();
```

---

## Selecionar Estratégia

```cpp id="bal014"
void balanceSetStrategy(BalanceStrategy strategy);
```

---

## Executar Autoteste

```cpp id="bal015"
bool balanceSelfTest();
```

---

# 6. Fluxo de Operação

```text id="bal016"
batteryUpdate()
        │
        ▼
Calcula delta entre células
        │
        ▼
Protection Manager autoriza?
        │
 ┌──────┴───────┐
 │              │
Não            Sim
 │              │
 ▼              ▼
Aguardar    Selecionar células
                │
                ▼
Acionar MOSFETs
                │
                ▼
Monitorar temperatura
                │
                ▼
Atualizar Logger
```

---

# 7. Integração

O Balance Manager recebe informações de:

* Battery Manager.
* Protection Manager.
* Configuration Manager.

Fornece dados para:

* Communication Manager.
* Logger.
* Display.
* CLI.
* BLE.
* CAN.

---

# 8. Tratamento de Erros

Caso ocorra:

* Sobretemperatura.
* Falha de MOSFET.
* Falha de sensor.
* Delta inválido.

O módulo deverá:

* Encerrar imediatamente o balanceamento.
* Registrar o evento.
* Informar o Protection Manager e o Fault Manager.

---

# 9. Requisitos

* Operação thread-safe.
* Compatibilidade com FreeRTOS.
* Tempo determinístico.
* Baixo consumo de CPU.

---

# 10. Critérios de Aceitação

A API será considerada estável quando:

* O balanceamento iniciar e encerrar conforme os critérios definidos.
* Os canais forem controlados corretamente.
* O estado refletir a condição real do sistema.
* Os testes forem aprovados em Mock, Wokwi e ESP32 físico.

---

# 11. Evoluções Futuras

A API deverá permitir:

* Balanceamento ativo.
* Controle por PWM.
* Balanceamento adaptativo.
* Algoritmos baseados em SOH.
* Estratégias específicas para diferentes químicas de bateria.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                                     |
| ------ | ---------- | --------------------------------------------- |
| 1.0.0  | 03/07/2026 | Primeira definição da API do Balance Manager. |

