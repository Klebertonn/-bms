# ICD-020 – Power Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001 a ICD-019

---

# 1. Objetivo

O Power Manager controla o consumo energético do sistema, estados de operação do ESP32 e estratégias de economia de energia, garantindo estabilidade do firmware em condições normais e críticas.

---

# 2. Responsabilidades

O Power Manager deverá:

* Monitorar tensão de alimentação do sistema.
* Gerenciar modos de energia do ESP32.
* Controlar transições entre estados de operação.
* Ativar/desativar periféricos conforme necessidade.
* Reduzir consumo em estado idle.
* Integrar com Watchdog e Fault Manager.
* Garantir recuperação segura após falhas de energia.

---

# 3. Estados de Energia

## PowerState

```cpp id="power001"
enum class PowerState
{
    STARTUP,
    ACTIVE,
    IDLE,
    STANDBY,
    SLEEP,
    DEEP_SLEEP,
    FAULT_POWER
};
```

---

## PowerStatus

```cpp id="power002"
struct PowerStatus
{
    PowerState state;
    float inputVoltage;
    float estimatedConsumption;
    uint32_t uptimeMs;
    bool lowPowerMode;
};
```

---

# 4. Interface Pública

## Inicialização

```cpp id="power003"
bool powerInit();
```

---

## Atualização

```cpp id="power004"
void powerUpdate();
```

---

## Obter Status

```cpp id="power005"
PowerStatus powerGetStatus();
```

---

## Entrar em Modo Baixo Consumo

```cpp id="power006"
bool powerEnterLowPowerMode();
```

---

## Sair do Modo Baixo Consumo

```cpp id="power007"
bool powerExitLowPowerMode();
```

---

## Entrar em Deep Sleep

```cpp id="power008"
void powerEnterDeepSleep(uint64_t sleepTimeMs);
```

---

## Monitorar Alimentação

```cpp id="power009"
bool powerMonitorSupply();
```

---

## Executar Autoteste

```cpp id="power010"
bool powerSelfTest();
```

---

# 5. Estratégias de Economia de Energia

O sistema deverá aplicar automaticamente:

### Idle Mode

* Redução de frequência de sensores.
* Atualização reduzida do display.
* Menor taxa de envio BLE/CAN.

---

### Standby Mode

* Desativação parcial de periféricos.
* Suspensão de tarefas não críticas.

---

### Sleep Mode

* Suspensão de comunicação.
* Monitoramento mínimo de segurança.

---

### Deep Sleep

* Apenas wake-up por interrupção.
* Estado salvo em memória não volátil.

---

# 6. Fluxo de Operação

```text id="power011"
Inicialização
      │
      ▼
Monitoramento contínuo
      │
 ┌────┴─────────────┐
 │                  │
Normal           Baixa energia
 │                  │
 ▼                  ▼
ACTIVE           IDLE/STANDBY
 │                  │
 ▼                  ▼
Operação      Economia de energia
 │                  │
 ▼                  ▼
Eventos críticos → FAULT_POWER
```

---

# 7. Integração

O Power Manager interage com:

* Battery Manager.
* Protection Manager.
* Watchdog Manager.
* Fault Manager.
* Communication Manager (BLE/CAN).
* Display Manager.
* FreeRTOS Task Manager.
* Configuration Manager.

---

# 8. Tratamento de Falhas de Energia

Em caso de instabilidade:

* Registrar evento no Logger.
* Reduzir carga do sistema automaticamente.
* Desativar periféricos não críticos.
* Entrar em modo seguro.
* Notificar interfaces (BLE, CAN, CLI quando possível).

---

# 9. Requisitos

* Compatível com ESP32 sleep modes.
* Transições seguras entre estados.
* Baixo consumo em idle.
* Integração com FreeRTOS.
* Recuperação segura após wake-up.

---

# 10. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Alternar corretamente entre estados de energia.
* Reduzir consumo em modo idle.
* Entrar e sair de sleep sem perda de dados críticos.
* Garantir recuperação estável após wake-up.

---

# 11. Evoluções Futuras

* Gestão inteligente baseada em IA (perfil de consumo).
* Otimização dinâmica de tarefas FreeRTOS.
* Wake-up por BLE ou CAN.
* Perfil de energia configurável por usuário.
* Integração com solar/energia externa.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                                |
| ------ | ---------- | ---------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Power Manager API. |

