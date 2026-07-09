# ICD-009 – Task Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-009, ICD-001 a ICD-008

---

# 1. Objetivo

O Task Manager é responsável por criar, iniciar, monitorar e controlar todas as tarefas do firmware utilizando o FreeRTOS.

Seu objetivo é garantir execução determinística, isolamento entre módulos e resposta em tempo real para funções críticas do BMS.

---

# 2. Responsabilidades

O Task Manager deverá:

* Criar todas as tarefas do sistema.
* Definir prioridades.
* Controlar períodos de execução.
* Monitorar uso de CPU.
* Monitorar uso de stack.
* Reiniciar tarefas quando possível.
* Detectar travamentos.
* Integrar-se ao Watchdog.

---

# 3. Arquitetura

```text id="task001"
                    FreeRTOS Scheduler
                           │
 ┌───────────────┬──────────┼───────────┬──────────────┐
 │               │          │           │              │
 ▼               ▼          ▼           ▼              ▼
Safety Task  Battery Task Balance Task Comm Task Logger Task
 │               │          │           │              │
 ▼               ▼          ▼           ▼              ▼
Protection     Battery     Balance     CAN/BLE      Storage
```

---

# 4. Enumerações

## TaskId

```cpp id="task002"
enum class TaskId
{
    SAFETY,
    BATTERY,
    BALANCE,
    COMMUNICATION,
    LOGGER,
    DISPLAY,
    STORAGE,
    CLI,
    WATCHDOG
};
```

---

## TaskState

```cpp id="task003"
enum class TaskState
{
    CREATED,
    RUNNING,
    SUSPENDED,
    BLOCKED,
    ERROR,
    STOPPED
};
```

---

# 5. Estruturas

## TaskInfo

```cpp id="task004"
struct TaskInfo
{
    TaskId id;

    TaskState state;

    uint8_t priority;

    uint16_t stackSize;

    uint16_t stackUsed;

    uint32_t executionTimeUs;

    uint32_t executions;

    bool alive;
};
```

---

# 6. Interface Pública

## Inicialização

```cpp id="task005"
bool taskManagerInit();
```

---

## Iniciar Scheduler

```cpp id="task006"
void taskManagerStart();
```

---

## Criar Tarefa

```cpp id="task007"
bool taskCreate(TaskId id);
```

---

## Remover Tarefa

```cpp id="task008"
bool taskDelete(TaskId id);
```

---

## Suspender

```cpp id="task009"
bool taskSuspend(TaskId id);
```

---

## Retomar

```cpp id="task010"
bool taskResume(TaskId id);
```

---

## Obter Estado

```cpp id="task011"
TaskState taskGetState(TaskId id);
```

---

## Obter Informações

```cpp id="task012"
TaskInfo taskGetInfo(TaskId id);
```

---

## Atualizar Estatísticas

```cpp id="task013"
void taskUpdateStatistics();
```

---

## Executar Autoteste

```cpp id="task014"
bool taskSelfTest();
```

---

# 7. Prioridades Recomendadas

| Tarefa        | Prioridade |     Período |
| ------------- | ---------: | ----------: |
| Safety        |         10 |        5 ms |
| Battery       |          9 |       10 ms |
| Balance       |          8 |       20 ms |
| Communication |          6 |       20 ms |
| Logger        |          5 |      100 ms |
| Display       |          4 |      250 ms |
| Storage       |          3 |      500 ms |
| CLI           |          2 | Sob demanda |
| Watchdog      |         10 |      100 ms |

---

# 8. Sincronização

O Task Manager deverá utilizar:

* Mutexes para recursos compartilhados.
* Filas para troca de mensagens.
* Semáforos binários para sincronização.
* Event Groups para estados globais.

Nenhum módulo deverá compartilhar variáveis globais sem proteção.

---

# 9. Fluxo de Inicialização

```text id="task015"
Boot
 │
 ▼
HAL
 │
 ▼
Configuration
 │
 ▼
Storage
 │
 ▼
Logger
 │
 ▼
Battery
 │
 ▼
Protection
 │
 ▼
Balance
 │
 ▼
Communication
 │
 ▼
Display
 │
 ▼
Scheduler Start
```

---

# 10. Monitoramento

O Task Manager deverá acompanhar:

* Uso de CPU.
* Tempo de execução.
* Tempo máximo de execução.
* Stack restante.
* Número de execuções.
* Tarefas travadas.

Essas informações deverão estar disponíveis para:

* CLI.
* BLE.
* CAN.
* Dashboard Serial.

---

# 11. Tratamento de Erros

Caso uma tarefa:

* Estoure a stack.
* Trave.
* Pare de responder.
* Exceda o tempo máximo.

O sistema deverá:

1. Registrar evento.
2. Notificar o Watchdog.
3. Atualizar o Fault Manager.
4. Reiniciar a tarefa, quando possível.
5. Executar desligamento seguro em falhas críticas.

---

# 12. Requisitos

* Compatível com FreeRTOS.
* Thread-safe.
* Baixa latência.
* Escalável.
* Determinístico.

---

# 13. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Todas as tarefas forem criadas corretamente.
* O scheduler operar de forma estável.
* O uso de CPU e stack puder ser monitorado.
* O firmware funcionar em Mock, Wokwi e ESP32 físico.

---

# 14. Evoluções Futuras

* Escalonamento dinâmico de prioridades.
* Afinidade por núcleo (ESP32 Dual Core).
* Estatísticas detalhadas de desempenho.
* Perfis de economia de energia.
* Balanceamento automático de carga entre núcleos.

---

# 15. Controle de Versão

| Versão | Data       | Descrição                                                   |
| ------ | ---------- | ----------------------------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Task Manager API baseada em FreeRTOS. |

