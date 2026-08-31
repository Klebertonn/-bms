# ICD-016 – OTA Update API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-004, ICD-006, ICD-007, ICD-008, ICD-010, ICD-011

---

# 1. Objetivo

O OTA Update Manager é responsável por realizar atualizações seguras do firmware do BMS sem a necessidade de conexão física, garantindo integridade, autenticidade e possibilidade de recuperação em caso de falha.

---

# 2. Responsabilidades

O OTA Manager deverá:

* Verificar disponibilidade de novas versões.
* Baixar pacotes de firmware.
* Validar integridade (checksum).
* Validar autenticidade (assinatura digital).
* Gravar a nova imagem.
* Reiniciar o sistema de forma segura.
* Executar rollback automático caso a atualização falhe.
* Registrar todo o processo no Logger.

---

# 3. Estados

## OtaState

```cpp id="ota001"
enum class OtaState
{
    IDLE,
    CHECKING,
    DOWNLOADING,
    VALIDATING,
    INSTALLING,
    REBOOTING,
    SUCCESS,
    FAILED,
    ROLLBACK
};
```

---

## OtaStatus

```cpp id="ota002"
struct OtaStatus
{
    OtaState state;
    uint8_t progress;
    bool updateAvailable;
    char currentVersion[16];
    char availableVersion[16];
};
```

---

# 4. Interface Pública

## Inicialização

```cpp id="ota003"
bool otaInit();
```

---

## Verificar Atualizações

```cpp id="ota004"
bool otaCheckForUpdates();
```

---

## Iniciar Atualização

```cpp id="ota005"
bool otaStart();
```

---

## Cancelar Atualização

```cpp id="ota006"
bool otaCancel();
```

---

## Obter Status

```cpp id="ota007"
OtaStatus otaGetStatus();
```

---

## Validar Firmware

```cpp id="ota008"
bool otaValidateImage();
```

---

## Executar Rollback

```cpp id="ota009"
bool otaRollback();
```

---

## Executar Autoteste

```cpp id="ota010"
bool otaSelfTest();
```

---

# 5. Fluxo de Atualização

```text id="ota011"
Inicialização
      │
      ▼
Verificar servidor
      │
      ▼
Nova versão disponível?
      │
 ┌────┴─────┐
 │          │
Não        Sim
 │          │
 ▼          ▼
Continuar   Download
 operação      │
               ▼
      Verificar integridade
               │
               ▼
      Validar assinatura
               │
               ▼
      Instalar firmware
               │
               ▼
          Reiniciar
               │
               ▼
        Autoteste inicial
               │
 ┌─────────────┴─────────────┐
 │                           │
 ▼                           ▼
Sucesso                 Falha
 │                           │
 ▼                           ▼
Executar firmware      Rollback
```

---

# 6. Segurança

O OTA deverá:

* Validar assinatura digital do firmware.
* Verificar checksum.
* Impedir downgrade não autorizado.
* Registrar todas as etapas no Logger.
* Bloquear atualizações durante falhas críticas do BMS.

---

# 7. Integração

Interage com:

* Communication Manager.
* Storage Manager.
* Configuration Manager.
* Fault Manager.
* Logger.
* Watchdog Manager.

---

# 8. Tratamento de Erros

Em caso de:

* Falha de download.
* Imagem corrompida.
* Assinatura inválida.
* Interrupção de energia.

O sistema deverá:

* Registrar evento.
* Restaurar a versão anterior (rollback).
* Notificar o usuário via CLI, BLE, CAN e Display.

---

# 9. Requisitos

* Compatível com ESP32 OTA.
* Atualização segura.
* Baixo consumo de memória.
* Rollback automático.
* Processo não bloqueante.

---

# 10. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Atualizar o firmware corretamente.
* Detectar imagens inválidas.
* Executar rollback quando necessário.
* Registrar todas as etapas do processo.

---

# 11. Evoluções Futuras

* Atualização diferencial (delta OTA).
* Atualização via BLE.
* Atualização via CAN Gateway.
* Agendamento automático de atualizações.
* Servidor próprio de distribuição de firmware.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                             |
| ------ | ---------- | ------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da OTA Update API. |

