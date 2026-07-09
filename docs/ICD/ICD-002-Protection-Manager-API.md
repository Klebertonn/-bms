# ICD-002 – Protection Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-005

---

# 1. Objetivo

Este documento define a interface pública do Protection Manager, responsável por monitorar continuamente as condições elétricas e térmicas do pack de baterias, detectar condições de risco e executar as ações de proteção necessárias.

A API deverá fornecer uma interface simples, estável e desacoplada para os demais módulos do sistema.

---

# 2. Responsabilidades

O Protection Manager deverá:

* Monitorar sobretensão (OVP).
* Monitorar subtensão (UVP).
* Monitorar sobrecorrente (OCP).
* Monitorar curto-circuito (SCP).
* Monitorar sobretemperatura (OTP).
* Monitorar subtemperatura (UTP).
* Atualizar o estado das proteções.
* Acionar ou bloquear MOSFETs conforme necessário.
* Registrar eventos no Logger.
* Informar o Fault Manager.

---

# 3. Enumerações

## ProtectionType

```cpp
enum class ProtectionType
{
    NONE,

    OVER_VOLTAGE,
    UNDER_VOLTAGE,

    OVER_CURRENT,
    SHORT_CIRCUIT,

    OVER_TEMPERATURE,
    UNDER_TEMPERATURE,

    HARDWARE_FAILURE,
    SENSOR_FAILURE
};
```

---

## ProtectionState

```cpp
enum class ProtectionState
{
    NORMAL,
    WARNING,
    FAULT,
    LATCHED
};
```

---

# 4. Estrutura de Dados

## ProtectionStatus

```cpp
struct ProtectionStatus
{
    ProtectionState state;

    bool ovp;
    bool uvp;
    bool ocp;
    bool scp;
    bool otp;
    bool utp;

    ProtectionType activeFault;

    uint32_t lastFaultTimestamp;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp
void protectionInit();
```

Inicializa o módulo e carrega os limites configurados.

---

## Atualização

```cpp
void protectionUpdate();
```

Executa a verificação completa das proteções.

---

## Obter Estado Geral

```cpp
ProtectionState protectionGetState();
```

Retorna o estado global das proteções.

---

## Obter Estrutura Completa

```cpp
const ProtectionStatus& protectionGetStatus();
```

Retorna a estrutura completa do estado atual.

---

## Verificar Falha Ativa

```cpp
bool protectionHasFault();
```

Retorna `true` caso exista qualquer falha ativa.

---

## Obter Falha Atual

```cpp
ProtectionType protectionGetActiveFault();
```

Retorna a falha de maior prioridade no momento.

---

## Limpar Falhas

```cpp
bool protectionClearFaults();
```

Limpa falhas recuperáveis conforme a política configurada.

---

## Teste das Proteções

```cpp
bool protectionSelfTest();
```

Executa uma rotina de autoteste do módulo.

---

# 6. Fluxo de Operação

```text
batteryUpdate()
        │
        ▼
protectionUpdate()
        │
        ▼
Verifica limites
        │
        ▼
Falha detectada?
        │
 ┌──────┴───────┐
 │              │
Não            Sim
 │              │
 ▼              ▼
Retorna     Aciona proteção
                │
                ▼
Atualiza Fault Manager
                │
                ▼
Logger
                │
                ▼
Communication Manager
```

---

# 7. Integração

O Protection Manager consome dados de:

* Battery Manager.
* HAL.
* Configuration Manager.

Fornece informações para:

* Fault Manager.
* Balance Manager.
* Logger.
* Communication Manager.
* Display.
* CLI.
* BLE.
* CAN.

---

# 8. Tratamento de Erros

Caso um sensor apresente falha:

* Registrar evento.
* Gerar `SENSOR_FAILURE`.
* Informar o Fault Manager.
* Colocar o sistema em modo seguro, quando necessário.

Caso um componente crítico falhe:

* Gerar `HARDWARE_FAILURE`.
* Desabilitar MOSFETs.
* Registrar log crítico.

---

# 9. Requisitos

* Interface thread-safe.
* Compatível com FreeRTOS.
* Execução determinística.
* Tempo máximo de execução compatível com a Safety Task.

---

# 10. Critérios de Aceitação

A API será considerada estável quando:

* Todas as proteções forem verificadas corretamente.
* Os estados forem consistentes.
* Os eventos forem propagados aos demais módulos.
* Os testes unitários forem aprovados.
* O funcionamento for validado em Mock, Wokwi e ESP32 físico.

---

# 11. Evoluções Futuras

A API deverá permitir:

* Novos tipos de proteção.
* Proteções específicas por química de bateria.
* Redundância de sensores.
* Estratégias avançadas de recuperação.
* Integração com sistemas externos de supervisão.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                                        |
| ------ | ---------- | ------------------------------------------------ |
| 1.0.0  | 03/07/2026 | Primeira definição da API do Protection Manager. |

