# ICD-010 – Fault Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001, ICD-002, ICD-003, ICD-006, ICD-007

---

# 1. Objetivo

O Fault Manager centraliza o gerenciamento de todas as falhas do sistema BMS.

Seu papel é receber eventos dos módulos, classificá-los, definir a severidade, registrar o histórico, executar ações corretivas e disponibilizar o estado global de falhas para o restante do sistema.

---

# 2. Responsabilidades

O Fault Manager deverá:

* Receber falhas de todos os módulos.
* Classificar severidade.
* Registrar timestamp.
* Manter histórico.
* Eliminar duplicidade de eventos.
* Acionar procedimentos de segurança.
* Informar Logger.
* Informar Communication Manager.
* Informar Display.
* Informar CLI.

---

# 3. Classificação

## FaultSeverity

```cpp id="fault001"
enum class FaultSeverity
{
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    FATAL
};
```

---

## FaultSource

```cpp id="fault002"
enum class FaultSource
{
    SYSTEM,
    BATTERY,
    PROTECTION,
    BALANCE,
    HAL,
    STORAGE,
    LOGGER,
    COMMUNICATION,
    WATCHDOG,
    DISPLAY,
    SENSOR
};
```

---

## FaultCode

```cpp id="fault003"
enum class FaultCode
{
    NONE,

    OVER_VOLTAGE,
    UNDER_VOLTAGE,

    OVER_CURRENT,
    SHORT_CIRCUIT,

    OVER_TEMPERATURE,
    UNDER_TEMPERATURE,

    SENSOR_FAILURE,

    ADC_FAILURE,

    STORAGE_FAILURE,

    COMMUNICATION_FAILURE,

    WATCHDOG_TIMEOUT,

    STACK_OVERFLOW,

    UNKNOWN
};
```

---

# 4. Estruturas

## FaultRecord

```cpp id="fault004"
struct FaultRecord
{
    uint32_t timestamp;

    FaultSource source;

    FaultCode code;

    FaultSeverity severity;

    bool active;

    char description[128];
};
```

---

## FaultStatus

```cpp id="fault005"
struct FaultStatus
{
    bool hasFault;

    FaultSeverity highestSeverity;

    uint32_t activeFaults;

    uint32_t totalFaults;

    bool emergencyShutdown;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="fault006"
bool faultInit();
```

---

## Registrar Falha

```cpp id="fault007"
bool faultRaise(
    FaultSource source,
    FaultCode code,
    FaultSeverity severity,
    const char* description);
```

---

## Limpar Falha

```cpp id="fault008"
bool faultClear(FaultCode code);
```

---

## Limpar Todas

```cpp id="fault009"
void faultClearAll();
```

---

## Obter Status

```cpp id="fault010"
FaultStatus faultGetStatus();
```

---

## Obter Histórico

```cpp id="fault011"
bool faultGetRecord(uint32_t index, FaultRecord* record);
```

---

## Verificar Emergência

```cpp id="fault012"
bool faultEmergencyShutdown();
```

---

## Executar Autoteste

```cpp id="fault013"
bool faultSelfTest();
```

---

# 6. Fluxo de Operação

```text id="fault014"
Módulo detecta falha
        │
        ▼
faultRaise()
        │
        ▼
Classificar severidade
        │
        ▼
Registrar no Logger
        │
        ▼
Salvar no Storage
        │
        ▼
Atualizar estado global
        │
 ┌──────┴─────────────┐
 │                    │
 ▼                    ▼
Falha recuperável   Falha crítica
 │                    │
 ▼                    ▼
Monitorar       Desligamento seguro
```

---

# 7. Integração

Recebe eventos de:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* HAL.
* Storage Manager.
* Logger.
* Watchdog.
* Communication Manager.

Fornece informações para:

* Display Manager.
* CLI Manager.
* BLE Service.
* CAN Protocol.
* Dashboard Serial.

---

# 8. Política de Severidade

| Severidade | Ação                                 |
| ---------- | ------------------------------------ |
| INFO       | Apenas registrar.                    |
| WARNING    | Registrar e monitorar.               |
| ERROR      | Registrar e notificar.               |
| CRITICAL   | Acionar proteção e limitar operação. |
| FATAL      | Desligamento seguro imediato.        |

---

# 9. Tratamento de Erros

O Fault Manager deverá:

* Evitar registros duplicados.
* Priorizar falhas críticas.
* Garantir integridade do histórico.
* Operar mesmo com falhas de armazenamento (mantendo histórico em RAM).

---

# 10. Requisitos

* Thread-safe.
* Compatível com FreeRTOS.
* Não bloquear tarefas críticas.
* Baixa latência.
* Histórico persistente.

---

# 11. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Todas as falhas forem registradas corretamente.
* A severidade for classificada conforme definido.
* O histórico puder ser consultado.
* O desligamento seguro ocorrer em falhas fatais.
* Funcionar corretamente em Mock, Wokwi e ESP32 físico.

---

# 12. Evoluções Futuras

* Agrupamento inteligente de falhas.
* Diagnóstico preditivo.
* Estatísticas de confiabilidade.
* Exportação de relatórios.
* Integração com sistemas remotos de manutenção.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                |
| ------ | ---------- | ---------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Fault Manager API. |

