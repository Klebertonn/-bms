# ICD-014 – CAN Protocol API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001 a ICD-013

---

# 1. Objetivo

O CAN Protocol Manager é responsável pela comunicação do BMS através do barramento CAN, permitindo troca de informações com inversores, carregadores, controladores de motor, gateways e sistemas de supervisão.

O módulo deverá suportar transmissão periódica de dados, recepção de comandos, tratamento de erros e integração com os demais módulos do firmware.

---

# 2. Responsabilidades

O CAN Protocol deverá:

* Inicializar a interface CAN.
* Configurar bitrate.
* Enviar mensagens periódicas.
* Receber comandos.
* Validar identificadores (IDs).
* Monitorar erros do barramento.
* Integrar-se ao Logger e ao Fault Manager.
* Permitir expansão para diferentes perfis de comunicação.

---

# 3. Estruturas

## CanState

```cpp id="can001"
enum class CanState
{
    STOPPED,
    INITIALIZING,
    RUNNING,
    ERROR_PASSIVE,
    BUS_OFF
};
```

---

## CanFrame

```cpp id="can002"
struct CanFrame
{
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
    bool extendedId;
    bool remoteFrame;
};
```

---

## CanStatus

```cpp id="can003"
struct CanStatus
{
    CanState state;
    uint32_t txFrames;
    uint32_t rxFrames;
    uint32_t txErrors;
    uint32_t rxErrors;
    bool busOff;
};
```

---

# 4. Interface Pública

## Inicialização

```cpp id="can004"
bool canInit(uint32_t bitrate);
```

---

## Atualização

```cpp id="can005"
void canUpdate();
```

---

## Enviar Frame

```cpp id="can006"
bool canSend(const CanFrame& frame);
```

---

## Receber Frame

```cpp id="can007"
bool canReceive(CanFrame& frame);
```

---

## Registrar Callback

```cpp id="can008"
bool canRegisterCallback(uint32_t id, void (*callback)(const CanFrame&));
```

---

## Obter Status

```cpp id="can009"
CanStatus canGetStatus();
```

---

## Reiniciar Interface

```cpp id="can010"
bool canRestart();
```

---

## Executar Autoteste

```cpp id="can011"
bool canSelfTest();
```

---

# 5. Mensagens Periódicas

O firmware deverá transmitir periodicamente:

| ID    | Conteúdo                | Período |
| ----- | ----------------------- | ------- |
| 0x100 | Tensão total do pack    | 100 ms  |
| 0x101 | Corrente do pack        | 100 ms  |
| 0x102 | SOC e SOH               | 500 ms  |
| 0x103 | Temperaturas            | 500 ms  |
| 0x104 | Estado dos MOSFETs      | 500 ms  |
| 0x105 | Alarmes ativos          | 100 ms  |
| 0x106 | Estado do balanceamento | 500 ms  |
| 0x107 | Versão do firmware      | 5 s     |

---

# 6. Comandos Recebidos

O firmware deverá aceitar, mediante validação:

* Solicitação de status.
* Leitura de configurações.
* Atualização de parâmetros.
* Reinicialização controlada.
* Execução de autotestes.
* Limpeza de falhas.
* Sincronização de tempo.

---

# 7. Fluxo de Operação

```text id="can012"
Inicialização
      │
      ▼
Configuração do controlador CAN
      │
      ▼
Estado RUNNING
      │
 ┌────┴──────────────┐
 │                   │
 ▼                   ▼
Transmitir        Receber
mensagens         comandos
 │                   │
 ▼                   ▼
Logger         Validação
 │                   │
 ▼                   ▼
Fault Manager   Execução
```

---

# 8. Tratamento de Erros

O módulo deverá detectar:

* Bus-Off.
* Error Passive.
* Perda de arbitragem.
* Timeout de transmissão.
* Frames inválidos.
* DLC incorreto.

Em caso de erro crítico:

* Registrar evento.
* Informar o Fault Manager.
* Tentar recuperação automática quando apropriado.

---

# 9. Integração

O CAN Protocol interage com:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Configuration Manager.
* Logger.
* Fault Manager.
* Task Manager.
* Watchdog Manager.

---

# 10. Requisitos

* Compatível com ESP32 e controlador CAN utilizado.
* Thread-safe.
* Baixa latência.
* Filtragem por identificadores.
* Buffer de transmissão e recepção.

---

# 11. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Transmitir mensagens periódicas corretamente.
* Receber e interpretar comandos válidos.
* Detectar e registrar erros do barramento.
* Operar em conjunto com os demais módulos sem perda de desempenho.

---

# 12. Evoluções Futuras

* Suporte a CAN FD.
* Perfis configuráveis de mensagens.
* Diagnóstico remoto.
* Integração com gateways industriais.
* Atualização OTA via gateway CAN.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                               |
| ------ | ---------- | --------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da CAN Protocol API. |

