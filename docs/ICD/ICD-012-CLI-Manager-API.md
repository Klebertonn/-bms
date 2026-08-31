# ICD-012 – CLI Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001 a ICD-011

---

# 1. Objetivo

O CLI Manager fornece uma interface de linha de comando para interação com o firmware do BMS durante desenvolvimento, testes, manutenção e diagnóstico.

O CLI permite consultar informações do sistema, alterar configurações, executar autotestes e acessar estatísticas sem necessidade de recompilar o firmware.

---

# 2. Responsabilidades

O CLI Manager deverá:

* Receber comandos pela interface serial.
* Interpretar parâmetros.
* Validar comandos.
* Executar ações solicitadas.
* Exibir respostas formatadas.
* Registrar operações no Logger.
* Restringir comandos críticos por nível de acesso.

---

# 3. Níveis de Acesso

## CliAccessLevel

```cpp id="cli001"
enum class CliAccessLevel
{
    GUEST,
    OPERATOR,
    ENGINEER,
    ADMIN
};
```

---

# 4. Estruturas

## CliCommand

```cpp id="cli002"
struct CliCommand
{
    const char* name;
    const char* description;
    CliAccessLevel requiredLevel;
};
```

---

## CliStatus

```cpp id="cli003"
struct CliStatus
{
    bool initialized;
    uint32_t commandsExecuted;
    uint32_t invalidCommands;
    CliAccessLevel currentAccessLevel;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="cli004"
bool cliInit();
```

---

## Atualização

```cpp id="cli005"
void cliUpdate();
```

Processa comandos recebidos.

---

## Registrar Comando

```cpp id="cli006"
bool cliRegisterCommand(const CliCommand& command);
```

---

## Executar Comando

```cpp id="cli007"
bool cliExecute(const char* commandLine);
```

---

## Alterar Nível de Acesso

```cpp id="cli008"
bool cliSetAccessLevel(CliAccessLevel level);
```

---

## Obter Status

```cpp id="cli009"
CliStatus cliGetStatus();
```

---

## Autoteste

```cpp id="cli010"
bool cliSelfTest();
```

---

# 6. Comandos Básicos

| Comando       | Descrição                              |
| ------------- | -------------------------------------- |
| help          | Lista todos os comandos disponíveis.   |
| version       | Exibe versão do firmware.              |
| status        | Mostra o estado geral do BMS.          |
| battery       | Exibe dados do pack e das células.     |
| protection    | Exibe proteções ativas.                |
| balance       | Mostra estado do balanceamento.        |
| logger        | Exibe últimos eventos registrados.     |
| faults        | Lista falhas ativas e histórico.       |
| config        | Exibe configurações atuais.            |
| tasks         | Mostra estado das tarefas do FreeRTOS. |
| watchdog      | Exibe status do Watchdog.              |
| reboot        | Reinicia o sistema (ADMIN).            |
| factory-reset | Restaura configurações padrão (ADMIN). |
| self-test     | Executa rotina de autoteste.           |

---

# 7. Fluxo de Operação

```text id="cli011"
Usuário envia comando
        │
        ▼
Recepção Serial
        │
        ▼
Parser
        │
        ▼
Validação
        │
 ┌──────┴────────────┐
 │                   │
 ▼                   ▼
Inválido         Comando válido
 │                   │
 ▼                   ▼
Mensagem de erro  Executar ação
                    │
                    ▼
Resposta ao usuário
```

---

# 8. Integração

O CLI poderá acessar informações de:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Configuration Manager.
* Logger.
* Fault Manager.
* Watchdog Manager.
* Task Manager.
* Communication Manager.

---

# 9. Tratamento de Erros

O CLI deverá:

* Informar comandos inválidos.
* Validar parâmetros antes da execução.
* Impedir acesso a comandos administrativos sem autorização.
* Registrar tentativas inválidas no Logger.

---

# 10. Requisitos

* Baixo consumo de memória.
* Resposta rápida aos comandos.
* Compatível com FreeRTOS.
* Thread-safe.
* Interface extensível para novos comandos.

---

# 11. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Interpretar corretamente os comandos registrados.
* Exibir respostas formatadas.
* Respeitar níveis de acesso.
* Funcionar em Mock, Wokwi e ESP32 físico.

---

# 12. Evoluções Futuras

* Histórico de comandos.
* Autocompletar.
* Ajuda contextual.
* Execução remota via BLE.
* Execução remota via CAN.
* Interface Web baseada nos mesmos comandos.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                              |
| ------ | ---------- | -------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da CLI Manager API. |

