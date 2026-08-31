# ICD-007 – Logger API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-008, ICD-006

---

# 1. Objetivo

O Logger é responsável por registrar todos os eventos relevantes do sistema BMS, permitindo diagnóstico, auditoria, rastreabilidade e análise de falhas.

O módulo deverá operar de forma desacoplada dos demais componentes e suportar múltiplos destinos de saída (UART, BLE, CAN, Display e armazenamento persistente).

---

# 2. Responsabilidades

O Logger deverá:

* Registrar eventos do sistema.
* Registrar mensagens de debug.
* Registrar avisos e erros.
* Registrar falhas críticas.
* Armazenar histórico.
* Exportar logs.
* Permitir filtros por módulo e severidade.
* Registrar timestamps.

---

# 3. Níveis de Log

## LogLevel

```cpp id="log001"
enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};
```

---

# 4. Módulos

## LogModule

```cpp id="log002"
enum class LogModule
{
    SYSTEM,
    BATTERY,
    PROTECTION,
    BALANCE,
    COMMUNICATION,
    STORAGE,
    HAL,
    TASK,
    DISPLAY,
    BLE,
    CAN,
    CLI,
    SENSOR,
    WATCHDOG
};
```

---

# 5. Destinos

## LogOutput

```cpp id="log003"
enum class LogOutput
{
    UART,
    BLE,
    CAN,
    OLED,
    STORAGE
};
```

---

# 6. Estruturas

## LogEntry

```cpp id="log004"
struct LogEntry
{
    uint32_t timestamp;
    LogLevel level;
    LogModule module;

    char message[128];
};
```

---

## LoggerStatus

```cpp id="log005"
struct LoggerStatus
{
    bool initialized;

    uint32_t totalLogs;
    uint32_t droppedLogs;

    uint32_t bufferUsage;

    bool storageEnabled;
};
```

---

# 7. Interface Pública

## Inicialização

```cpp id="log006"
void loggerInit();
```

---

## Atualização

```cpp id="log007"
void loggerUpdate();
```

---

## Registrar Mensagem

```cpp id="log008"
void loggerLog(
    LogLevel level,
    LogModule module,
    const char* message);
```

---

## Registrar Mensagem Formatada

```cpp id="log009"
void loggerPrintf(
    LogLevel level,
    LogModule module,
    const char* format,
    ...);
```

---

## Obter Status

```cpp id="log010"
LoggerStatus loggerGetStatus();
```

---

## Limpar Buffer

```cpp id="log011"
void loggerClear();
```

---

## Exportar Logs

```cpp id="log012"
void loggerExport(LogOutput output);
```

---

## Configurar Nível Mínimo

```cpp id="log013"
void loggerSetLevel(LogLevel level);
```

---

## Habilitar Destino

```cpp id="log014"
void loggerEnableOutput(LogOutput output, bool enable);
```

---

# 8. Fluxo de Operação

```text id="log015"
Módulo gera evento
        │
        ▼
loggerLog()
        │
        ▼
Buffer Circular
        │
        ▼
Filtro de nível
        │
        ▼
Destinos habilitados
        │
 ┌──────┼──────────────┐
 ▼      ▼              ▼
UART   BLE         Storage
        │
        ▼
Display OLED (eventos críticos)
```

---

# 9. Integração

Recebe eventos de:

* Battery Manager
* Protection Manager
* Balance Manager
* Communication Manager
* HAL
* Task Manager
* Storage Manager
* Watchdog
* Fault Manager

Fornece informações para:

* CLI
* Dashboard Serial
* BLE
* CAN
* OLED
* Armazenamento Persistente

---

# 10. Formato Padrão dos Logs

Exemplo:

```text id="log016"
[00001234 ms] [INFO] [BATTERY] SOC atualizado: 82%
```

```text id="log017"
[00005421 ms] [WARNING] [BALANCE] Delta entre células acima do limite.
```

```text id="log018"
[00008912 ms] [ERROR] [PROTECTION] Overvoltage detectado na célula 5.
```

```text id="log019"
[00009500 ms] [CRITICAL] [SYSTEM] Desligamento de emergência executado.
```

---

# 11. Tratamento de Erros

Caso o buffer esteja cheio:

* Incrementar `droppedLogs`.
* Preservar mensagens de nível CRITICAL.
* Substituir mensagens TRACE e DEBUG mais antigas.

Caso o armazenamento falhe:

* Continuar operando apenas com buffer em RAM.
* Registrar um evento crítico.

---

# 12. Requisitos

* Thread-safe.
* Compatível com FreeRTOS.
* Baixo uso de RAM.
* Buffer circular.
* Não bloquear tarefas críticas.

---

# 13. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Registrar eventos corretamente.
* Suportar múltiplos destinos simultaneamente.
* Não perder mensagens críticas.
* Funcionar em Mock, Wokwi e ESP32 físico.

---

# 14. Evoluções Futuras

* Exportação em JSON.
* Exportação em CSV.
* Compressão de logs.
* Streaming BLE.
* Streaming CAN.
* Logs remotos via MQTT.
* Integração com dashboards de diagnóstico.

---

# 15. Controle de Versão

| Versão | Data       | Descrição                         |
| ------ | ---------- | --------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Logger API. |

