# ICD-013 – BLE Service API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001 a ICD-012

---

# 1. Objetivo

O BLE Service Manager disponibiliza os dados do BMS através do Bluetooth Low Energy (BLE), permitindo monitoramento, configuração e diagnóstico por aplicativos móveis e ferramentas de manutenção.

---

# 2. Responsabilidades

O BLE Service deverá:

* Inicializar a pilha BLE.
* Publicar serviços GATT.
* Disponibilizar características para leitura e escrita.
* Enviar notificações em tempo real.
* Permitir configuração segura do sistema.
* Integrar-se ao Logger e ao Fault Manager.

---

# 3. Estruturas

## BleConnectionStatus

```cpp id="ble001"
enum class BleConnectionStatus
{
    DISCONNECTED,
    ADVERTISING,
    CONNECTED
};
```

---

## BleStatus

```cpp id="ble002"
struct BleStatus
{
    BleConnectionStatus connection;
    uint32_t connectedClients;
    bool notificationsEnabled;
    int8_t rssi;
};
```

---

# 4. Serviços GATT

## Battery Service

Características:

* Pack Voltage
* Pack Current
* SOC
* SOH
* Temperature
* Cycle Count

---

## Cell Service

Características:

* Cell 1 Voltage
* Cell 2 Voltage
* ...
* Cell N Voltage
* Delta Voltage

---

## Protection Service

Características:

* Over Voltage
* Under Voltage
* Over Current
* Over Temperature
* Under Temperature
* Short Circuit

---

## Configuration Service

Características:

* Limites de tensão.
* Limites de corrente.
* Limites de temperatura.
* Parâmetros de balanceamento.

Escrita permitida apenas para usuários autenticados.

---

## Diagnostic Service

Características:

* Faults
* Logger
* Task Status
* Watchdog Status
* Firmware Version

---

# 5. Interface Pública

## Inicialização

```cpp id="ble003"
bool bleInit();
```

---

## Atualização

```cpp id="ble004"
void bleUpdate();
```

---

## Iniciar Advertising

```cpp id="ble005"
bool bleStartAdvertising();
```

---

## Parar Advertising

```cpp id="ble006"
bool bleStopAdvertising();
```

---

## Enviar Notificação

```cpp id="ble007"
bool bleNotify(const char* characteristic);
```

---

## Obter Status

```cpp id="ble008"
BleStatus bleGetStatus();
```

---

## Encerrar Conexão

```cpp id="ble009"
bool bleDisconnect();
```

---

# 6. Fluxo de Operação

```text id="ble010"
Inicialização
      │
      ▼
Advertising
      │
      ▼
Aplicativo conecta
      │
      ▼
Autenticação
      │
      ▼
Leitura de dados
      │
      ▼
Notificações em tempo real
      │
      ▼
Desconexão
```

---

# 7. Segurança

O BLE deverá suportar:

* Emparelhamento seguro.
* Autenticação para escrita.
* Criptografia das configurações sensíveis.
* Tempo limite de sessão.
* Registro de acessos no Logger.

---

# 8. Integração

Interage com:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Configuration Manager.
* Fault Manager.
* Logger.
* Task Manager.
* Display Manager.

---

# 9. Requisitos

* Compatível com ESP32 BLE.
* Baixo consumo de energia.
* Notificações periódicas configuráveis.
* Múltiplas características GATT.
* Thread-safe.

---

# 10. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Um aplicativo conseguir conectar ao ESP32.
* Dados do pack e das células forem atualizados em tempo real.
* Configurações autorizadas puderem ser alteradas.
* Alarmes forem enviados imediatamente por notificação BLE.

---

# 11. Evoluções Futuras

* Atualização OTA via BLE.
* Sincronização com aplicativo em nuvem.
* Exportação de logs.
* Dashboard gráfico.
* Perfis de usuário.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                              |
| ------ | ---------- | -------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da BLE Service API. |

