# ICD-011 – Watchdog Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-009, ICD-010, ICD-007

---

# 1. Objetivo

O Watchdog Manager monitora continuamente a execução das tarefas do firmware para detectar travamentos, loops infinitos, estouro de tempo de execução e falhas críticas.

Ele complementa o watchdog de hardware do ESP32 com um watchdog lógico de software, oferecendo diagnósticos detalhados e ações de recuperação.

---

# 2. Responsabilidades

O Watchdog Manager deverá:

* Monitorar todas as tarefas registradas.
* Receber "heartbeats" das tarefas.
* Detectar timeout de execução.
* Detectar tarefas travadas.
* Integrar-se ao watchdog de hardware.
* Registrar eventos no Logger.
* Reportar falhas ao Fault Manager.
* Reiniciar tarefas quando possível.
* Solicitar reinicialização segura do sistema em falhas fatais.

---

# 3. Enumerações

## WatchdogState

```cpp id="wdg001"
enum class WatchdogState
{
    STOPPED,
    RUNNING,
    WARNING,
    ERROR
};
```

---

## WatchdogAction

```cpp id="wdg002"
enum class WatchdogAction
{
    NONE,
    LOG_ONLY,
    RESTART_TASK,
    SAFE_SHUTDOWN,
    SYSTEM_RESET
};
```

---

# 4. Estruturas

## WatchdogTask

```cpp id="wdg003"
struct WatchdogTask
{
    TaskId taskId;
    uint32_t timeoutMs;
    uint32_t lastHeartbeat;
    bool enabled;
};
```

---

## WatchdogStatus

```cpp id="wdg004"
struct WatchdogStatus
{
    WatchdogState state;
    uint32_t monitoredTasks;
    uint32_t timeoutEvents;
    bool hardwareWatchdogEnabled;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="wdg005"
bool watchdogInit();
```

---

## Atualização

```cpp id="wdg006"
void watchdogUpdate();
```

Executa a verificação periódica das tarefas.

---

## Registrar Tarefa

```cpp id="wdg007"
bool watchdogRegisterTask(TaskId id, uint32_t timeoutMs);
```

---

## Remover Tarefa

```cpp id="wdg008"
bool watchdogUnregisterTask(TaskId id);
```

---

## Heartbeat

```cpp id="wdg009"
void watchdogKick(TaskId id);
```

Chamado periodicamente por cada tarefa para indicar que está ativa.

---

## Obter Estado

```cpp id="wdg010"
WatchdogState watchdogGetState();
```

---

## Obter Status

```cpp id="wdg011"
WatchdogStatus watchdogGetStatus();
```

---

## Executar Autoteste

```cpp id="wdg012"
bool watchdogSelfTest();
```

---

# 6. Fluxo de Operação

```text id="wdg013"
Scheduler inicia
        │
        ▼
Tarefas executam
        │
        ▼
watchdogKick()
        │
        ▼
Watchdog verifica timeouts
        │
 ┌──────┴─────────────┐
 │                    │
 ▼                    ▼
Heartbeat OK      Timeout detectado
 │                    │
 ▼                    ▼
Continua       Fault Manager
                    │
                    ▼
Logger
                    │
                    ▼
Ação configurada
```

---

# 7. Integração

Recebe informações de:

* Task Manager.
* FreeRTOS Scheduler.

Interage com:

* Fault Manager.
* Logger.
* HAL.
* Storage Manager.

---

# 8. Política de Recuperação

| Situação          | Ação                                               |
| ----------------- | -------------------------------------------------- |
| Atraso pequeno    | Registrar WARNING.                                 |
| Timeout isolado   | Reiniciar tarefa, se suportado.                    |
| Timeout repetido  | Registrar CRITICAL e limitar operação.             |
| Falha persistente | Solicitar desligamento seguro ou reset do sistema. |

---

# 9. Tratamento de Erros

Caso o próprio Watchdog apresente falha:

* Registrar evento crítico.
* Notificar o Fault Manager.
* Ativar o watchdog de hardware do ESP32 como mecanismo de proteção final.

---

# 10. Requisitos

* Thread-safe.
* Compatível com FreeRTOS.
* Baixa latência.
* Não bloquear tarefas.
* Compatível com watchdog de hardware.

---

# 11. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Detectar corretamente tarefas travadas.
* Registrar eventos de timeout.
* Executar a política de recuperação configurada.
* Operar em Mock, Wokwi e ESP32 físico.

---

# 12. Evoluções Futuras

* Monitoramento por núcleo (ESP32 Dual Core).
* Estatísticas de tempo de resposta.
* Ajuste dinâmico de timeouts.
* Integração com sistemas de telemetria.
* Diagnóstico preditivo de travamentos.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                   |
| ------ | ---------- | ------------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Watchdog Manager API. |

