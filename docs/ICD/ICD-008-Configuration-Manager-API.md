# ICD-008 – Configuration Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-010, ICD-006

---

# 1. Objetivo

O Configuration Manager centraliza todos os parâmetros configuráveis do BMS, fornecendo uma interface única para leitura, atualização, validação e persistência das configurações do sistema.

As configurações podem ser carregadas da memória não volátil, alteradas via CLI, BLE ou CAN e restauradas para os valores padrão quando necessário.

---

# 2. Responsabilidades

O Configuration Manager deverá:

* Carregar configurações na inicialização.
* Validar todos os parâmetros.
* Disponibilizar configurações para os demais módulos.
* Atualizar configurações em tempo de execução.
* Persistir alterações no Storage Manager.
* Restaurar configurações padrão.
* Controlar a versão da estrutura de configuração.

---

# 3. Estruturas de Dados

## FirmwareProfile

```cpp id="cfg001"
enum class FirmwareProfile
{
    MOCK,
    WOKWI,
    ESP32
};
```

---

## BatteryConfig

```cpp id="cfg002"
struct BatteryConfig
{
    uint8_t cellCount;

    float maxCellVoltage;
    float minCellVoltage;

    float maxChargeCurrent;
    float maxDischargeCurrent;

    float maxTemperature;
    float minTemperature;

    float balanceStartDelta;
    float balanceStopDelta;
};
```

---

## CommunicationConfig

```cpp id="cfg003"
struct CommunicationConfig
{
    uint32_t serialBaudrate;
    uint32_t canBitrate;

    char bleDeviceName[32];

    bool uartEnabled;
    bool bleEnabled;
    bool canEnabled;
    bool oledEnabled;
};
```

---

## LoggerConfig

```cpp id="cfg004"
struct LoggerConfig
{
    uint8_t minimumLevel;
    bool persistentStorage;
    bool exportUART;
    bool exportBLE;
    bool exportCAN;
};
```

---

## SystemConfig

```cpp id="cfg005"
struct SystemConfig
{
    uint32_t version;

    FirmwareProfile profile;

    BatteryConfig battery;

    CommunicationConfig communication;

    LoggerConfig logger;
};
```

---

# 4. Interface Pública

## Inicialização

```cpp id="cfg006"
bool configInit();
```

---

## Carregar Configuração

```cpp id="cfg007"
bool configLoad();
```

---

## Salvar Configuração

```cpp id="cfg008"
bool configSave();
```

---

## Restaurar Configuração Padrão

```cpp id="cfg009"
bool configFactoryReset();
```

---

## Obter Configuração

```cpp id="cfg010"
const SystemConfig& configGet();
```

---

## Atualizar Configuração

```cpp id="cfg011"
bool configSet(const SystemConfig& config);
```

---

## Validar Configuração

```cpp id="cfg012"
bool configValidate();
```

---

## Obter Perfil

```cpp id="cfg013"
FirmwareProfile configGetProfile();
```

---

# 5. Fluxo de Operação

```text id="cfg014"
Inicialização
      │
      ▼
configInit()
      │
      ▼
Carregar Storage
      │
      ▼
Validar parâmetros
      │
 ┌────┴─────┐
 │          │
Erro       OK
 │          │
 ▼          ▼
Factory   Sistema inicia
Reset
```

---

# 6. Integração

Fornece configurações para:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Communication Manager.
* Logger.
* HAL.
* Task Manager.

Recebe comandos de:

* CLI.
* BLE.
* CAN.
* Aplicações de configuração.

---

# 7. Tratamento de Erros

Caso uma configuração seja inválida:

* Rejeitar atualização.
* Registrar evento no Logger.
* Restaurar último valor válido.
* Informar o Fault Manager, quando aplicável.

---

# 8. Requisitos

* Interface thread-safe.
* Persistência via Storage Manager.
* Compatível com FreeRTOS.
* Suporte a versionamento de configuração.
* Validação completa antes da aplicação.

---

# 9. Critérios de Aceitação

O módulo será considerado aprovado quando:

* As configurações forem carregadas corretamente.
* Alterações forem persistidas.
* Configurações inválidas forem rejeitadas.
* O mesmo firmware funcionar em Mock, Wokwi e ESP32 apenas alterando o perfil.

---

# 10. Evoluções Futuras

* Perfis personalizados.
* Importação/exportação em JSON.
* Configuração remota via BLE.
* Configuração remota via CAN.
* Interface Web.
* Atualização OTA das configurações.

---

# 11. Controle de Versão

| Versão | Data       | Descrição                                        |
| ------ | ---------- | ------------------------------------------------ |
| 1.0.0  | 04/07/2026 | Primeira definição da Configuration Manager API. |

