# DTC – Diagnostic Trouble Code Design

**Projeto:** BMS Firmware ESP32
**Versão:** 1.0.0
**Status:** Proposta de Design (documento de arquitetura)
**Tipo:** Documentação e arquitetura – sem alteração de código
**Dependências:** `core/fault/*`, `system/logger/*`, `system/storage/*`, `core/state/self_test`, `docs/ICD/ICD-010-Fault-Manager-API.md`

---

## Sumário

1. [Conceito do sistema DTC](#1-conceito-do-sistema-dtc)
2. [Modelo de código de falha](#2-modelo-de-código-de-falha)
3. [Categorias](#3-categorias)
4. [Estados da falha](#4-estados-da-falha)
5. [Fluxo](#5-fluxo)
6. [Persistência](#6-persistência)
7. [Integração futura](#7-integração-futura)
8. [Regras](#8-regras)
9. [Estrutura de código proposta](#9-estrutura-de-código-proposta)
10. [Glossário](#10-glossário)
11. [Controle de versão](#11-controle-de-versão)

---

# 1. Conceito do sistema DTC

O sistema DTC (Diagnostic Trouble Code) é a evolução do atual `FaultManager` para um
modelo padronizado de diagnóstico, no qual **cada falha possui um código único,
legível e persistente**, acompanhado de metadados de engenharia (estado, severidade,
contador de ocorrência, origem, valor medido e limite).

Enquanto o `FaultManager` atual apenas **acumula um bitmask de falhas** (`FaultFlag`)
e deriva um único `FaultInfo` com prioridade fixa, o modelo DTC:

- **Identifica de forma única** cada condição de falha (código BMS-XXX-NNN).
- **Registra o ciclo de vida** de cada falha (estado ACTIVE → CLEARED / LOCKED).
- **Contabiliza ocorrências** para análise de confiabilidade e manutenção preditiva.
- **Persiste o histórico** de forma estruturada, com versão e CRC.
- **Expõe** o diagnóstico para CAN, BLE, MQTT e Dashboard Web de forma uniforme.

O DTC é um **padrão de representação** acima do mecanismo de detecção. A detecção
continua distribuída nos managers do domínio (Protection, Battery, Temperature,
Current, Communication, Storage, Hardware e Safety), e o **Fault Manager** atua como
**coordenador central** que converte eventos em DTCs.

### Relação com a arquitetura atual

| Componente atual                                              | Papel no modelo DTC                                     |
| ------------------------------------------------------------- | ------------------------------------------------------- |
| `core/fault/fault_flags.h` (`FaultFlag`)                      | Detecção binária de condição (sinal de entrada).        |
| `core/fault/fault_reason.h` (`FaultReason`, `FaultInfo`)      | Base de dados de metadados da falha (reason/code).      |
| `core/fault/fault_manager.{h,cpp}`                            | Coordenador / motor de estado DTC (novo núcleo).        |
| `core/fault/fault_history.{h,cpp}`                            | Cache em RAM do histórico (RING buffer).                |
| `system/storage/fault_storage.{h,cpp}`                        | Backend de persistência (a ser abstraído).             |
| `system/logger/*`                                             | Publicação de eventos de diagnóstico (Event Logger).    |
| `core/state/self_test`                                        | Verificação de integridade do subsistema de fault.      |

> **Objetivo do documento:** definir o modelo DTC, os códigos, os estados, o fluxo,
> a política de persistência, a integração futura e as regras de dependência.
> Nenhum código é alterado nesta etapa.

---

# 2. Modelo de código de falha

## 2.1 Formato

```
BMS-<CAT>-[NNN]
```

| Campo    | Significado                                                      | Exemplo   |
| -------- | ---------------------------------------------------------------- | --------- |
| `BMS`    | Prefixo fixo do domínio (Battery Management System).             | `BMS`     |
| `<CAT>`  | Categoria da falha (2 letras, ver seção 3).                      | `OV`      |
| `[NNN]`  | Número sequencial de 3 dígitos dentro da categoria (001–999).    | `001`     |

O código é **estável**: uma vez publicado, `BMS-OV-001` significa sempre
"Over Voltage Cell". Novas falhas recebem o próximo número livre da categoria;
códigos não são reutilizados.

## 2.2 Exemplos (conforme solicitado)

| Código DTC      | Descrição         | Categoria      |
| --------------- | ----------------- | -------------- |
| `BMS-OV-001`    | Over Voltage Cell | Cell Voltage   |
| `BMS-OT-001`    | Over Temperature  | Temperature    |
| `BMS-OC-001`    | Over Current      | Current        |

## 2.3 Representação interna (estrutura base)

```cpp
struct DtcRecord
{
    uint16_t id;          // ID numérico compacto (mapeado para o string BMS-XXX-NNN)
    uint8_t  category;    // DtcCategory (ver seção 3)
    uint8_t  state;       // DtcState (ver seção 4)
    uint8_t  severity;    // INFO / WARNING / ERROR / CRITICAL / FATAL
    uint8_t  source;      // Célula (1..N) ou 0xFF (pack/global)
    float    value;       // Valor medido no momento da falha
    float    limit;       // Limiar que disparou a falha
    uint32_t occurrence;  // Contador de ocorrências (persistido)
    uint32_t timestampMs; // Tempo desde o boot / época (clock)
    uint32_t clearedAtMs; // Tempo de limpeza (0 se ainda ativo)
};
```

> O `DtcRecord` é compatível com o `FaultInfo` existente e pode ser convertido
> bidirecionalmente para manter a compatibilidade com o histórico atual.

## 2.4 Mapeamento para o sistema atual

O modelo DTC **substitui/estende** os códigos numéricos atuais (`0x0101`, `0x0102`,
`0x0201`, `0x0301`) e os `FaultReason` por códigos padronizados. O mapeamento será
feito por uma tabela de tradução (`DtcTable`), mantendo o `FaultManager` como fonte
de verdade:

| Código atual | FaultReason atual             | DTC proposto  | Descrição                     |
| ------------ | ----------------------------- | ------------- | ----------------------------- |
| `0x0101`     | `CELL_OVERVOLTAGE`            | `BMS-CV-001`  | Cell Over Voltage             |
| `0x0102`     | `CELL_UNDERVOLTAGE`           | `BMS-CV-002`  | Cell Under Voltage            |
| `0x0201`     | `OVERTEMPERATURE`             | `BMS-OT-001`  | Over Temperature              |
| `0x0301`     | `OVERCURRENT_CHARGE`          | `BMS-OC-001`  | Over Current (Charge)         |
| —            | `OVERCURRENT_DISCHARGE`       | `BMS-OC-002`  | Over Current (Discharge)      |
| —            | `PACK_OVERVOLTAGE`            | `BMS-BT-001`  | Pack Over Voltage             |
| —            | `PACK_UNDERVOLTAGE`           | `BMS-BT-002`  | Pack Under Voltage            |
| —            | `UNDERTEMPERATURE`            | `BMS-OT-002`  | Under Temperature             |
| —            | `ADC_FAILURE`                 | `BMS-HW-001`  | ADC Failure                   |
| —            | `SENSOR_FAILURE`              | `BMS-HW-002`  | Sensor Failure                |
| —            | `COMMUNICATION_TIMEOUT`       | `BMS-COM-001` | Communication Timeout         |

> A numeração exata será definida na tabela de referência final (`DtcTable`).
> O importante é a **estabilidade** e o **padrão** descrito aqui.

---

# 3. Categorias

As categorias agrupam falhas por subsistema e definem o prefixo de 2 letras do código.

| Categoria      | Prefixo | Escopo                                                          |
| -------------- | ------- | --------------------------------------------------------------- |
| Battery        | `BT`    | Falhas do pack: pack OV/UV, estado inválido, SOH baixo.         |
| Cell Voltage   | `CV`    | Falhas por célula: over/under voltage, desbalanceamento, delta. |
| Temperature    | `OT`    | Over/under temperature, sensor térmico, gradiente.              |
| Current        | `OC`    | Overcurrent charge/discharge, curto-circuito, corrente inválida.|
| Communication  | `COM`   | Timeout/erro CAN, BLE, perda de enlace, protocolo inválido.     |
| Storage        | `STO`   | Erro de flash/NVS/EEPROM, CRC, corrupção, write fail.           |
| Hardware       | `HW`    | ADC, sensor, MOSFET, watchdog, pilha, clock.                    |
| Safety         | `SAF`   | Desligamento de emergência, estado LOCKED, violação de segurança.|

## 3.1 Tabela de sub-códigos por categoria (primeira proposta)

**Battery (`BT`)**
| Código        | Descrição                  |
| ------------- | -------------------------- |
| `BMS-BT-001`  | Pack Over Voltage          |
| `BMS-BT-002`  | Pack Under Voltage         |
| `BMS-BT-003`  | Pack State Invalid         |
| `BMS-BT-004`  | SOH Below Threshold        |

**Cell Voltage (`CV`)**
| Código        | Descrição                  |
| ------------- | -------------------------- |
| `BMS-CV-001`  | Cell Over Voltage          |
| `BMS-CV-002`  | Cell Under Voltage         |
| `BMS-CV-003`  | Cell Delta Exceeded        |
| `BMS-CV-004`  | Cell Reading Invalid       |

**Temperature (`OT`)**
| Código        | Descrição                  |
| ------------- | -------------------------- |
| `BMS-OT-001`  | Over Temperature           |
| `BMS-OT-002`  | Under Temperature          |
| `BMS-OT-003`  | Temperature Sensor Invalid |

**Current (`OC`)**
| Código        | Descrição                  |
| ------------- | -------------------------- |
| `BMS-OC-001`  | Over Current (Charge)      |
| `BMS-OC-002`  | Over Current (Discharge)   |
| `BMS-OC-003`  | Short Circuit              |
| `BMS-OC-004`  | Current Sensor Invalid     |

**Communication (`COM`)**
| Código         | Descrição                  |
| -------------- | -------------------------- |
| `BMS-COM-001`  | Communication Timeout      |
| `BMS-COM-002`  | CAN Bus Error              |
| `BMS-COM-003`  | BLE Link Lost              |
| `BMS-COM-004`  | Protocol Frame Invalid     |

**Storage (`STO`)**
| Código         | Descrição                  |
| -------------- | -------------------------- |
| `BMS-STO-001`  | Storage Write Failure      |
| `BMS-STO-002`  | Storage CRC Mismatch       |
| `BMS-STO-003`  | Storage Corrupted          |

**Hardware (`HW`)**
| Código         | Descrição                  |
| -------------- | -------------------------- |
| `BMS-HW-001`   | ADC Failure                |
| `BMS-HW-002`   | Sensor Failure             |
| `BMS-HW-003`   | MOSFET Failure             |
| `BMS-HW-004`   | Watchdog Timeout           |
| `BMS-HW-005`   | Clock Invalid              |

**Safety (`SAF`)**
| Código         | Descrição                  |
| -------------- | -------------------------- |
| `BMS-SAF-001`  | Emergency Shutdown         |
| `BMS-SAF-002`  | Fault Locked (latched)     |
| `BMS-SAF-003`  | Security Violation         |

---

# 4. Estados da falha

Cada DTC possui um **estado de ciclo de vida**. O estado reflete a condição
**atual** da falha no sistema.

| Estado    | Significado                                                          | Ação típica                                   |
| --------- | -------------------------------------------------------------------- | --------------------------------------------- |
| `ACTIVE`  | Condição de falha está presente agora.                               | Proteções ativas, comunicação notifica.       |
| `CLEARED` | Condição desapareceu e a falha foi reconhecida/limpa.                | Registro mantido no histórico.                |
| `WARNING` | Condição de alerta (limiar de pré-falha), sem bloqueio de operação.  | Monitoramento, log WARNING.                   |
| `CRITICAL`| Falha grave que limita/desliga operação.                             | Proteção + desligamento seguro.               |
| `LOCKED`  | Falha latched: exige reset manual / permissão para limpar.           | Espera reconhecimento do operador.            |

## 4.1 Transições de estado

```text
        (detecção do limiar de pré-falha)
                 │
                 ▼
             WARNING ──────────────┐
                 │ (limiar de falha)│
                 ▼                  │
             ACTIVE ────────────────┤
            ╱        ╲              │
      (grave)          (recuperado) │
     ╱                  │           │
   ▼                    ▼           │
 CRITICAL            CLEARED ◄──────┘
     │
     │ (latche / requer reset manual)
     ▼
   LOCKED ──(reconhecimento/reset)──► CLEARED
```

Regras de transição:

- **WARNING → ACTIVE**: quando o limiar de falha é atingido.
- **ACTIVE → CLEARED**: quando a condição desaparece e a falha é auto-limpa
  (se não for latched).
- **ACTIVE → CRITICAL**: quando a falha é elevada a nível inaceitável.
- **CRITICAL → LOCKED**: quando a falha exige intervenção manual (ex.: curto,
  emergência, violação de segurança).
- **LOCKED → CLEARED**: somente após reconhecimento manual (CLI `clearfaults`,
  BLE, Dashboard) ou reset de fábrica.

> **Compatibilidade com o atual:** o `FaultManager` atual só possui o conceito
> binário `active` em `FaultInfo`. O estado `CLEARED`/`LOCKED` será adicionado.
> Falhas `FATAL` do ICD-010 mapeiam para `LOCKED` + `CRITICAL`.

---

# 5. Fluxo

O fluxo de uma falha DTC atravessa as camadas do sistema respeitando a arquitetura
em camadas (core → system → communication → hal).

```text
 Sensor
   │      (leitura de tensão, corrente, temperatura, estado)
   ▼
 Protection Manager
   │      (aplica limiares, decide se é pré-falha / falha / crítica)
   ▼
 Fault Manager
   │      (converte evento em DTC, define estado, severidade, ocorrência)
   ▼
 Event Logger
   │      (publica evento INFO/WARNING/ERROR/CRITICAL na EventQueue)
   ▼
 Storage
   │      (persiste registro DTC com versão + CRC)
   ▼
 Communication
        (expõe para CAN / BLE / MQTT / Dashboard Web)
```

## 5.1 Responsabilidades por etapa

| Etapa                | Responsabilidade                                                   |
| -------------------- | ------------------------------------------------------------------ |
| **Sensor**           | Amostra grandezas físicas via HAL (adc, ina219, max17048, NTC).     |
| **Protection Manager** | Aplica limiares (config.), decide limiar de warning/fault/critical. |
| **Fault Manager**    | Centraliza: mapeia evento → DTC, gerencia estado, contador, ordem. |
| **Event Logger**     | Publica evento na fila (não bloqueante) e formata para saída.      |
| **Storage**          | Grava `DtcRecord` persistente (flash/NVS/EEPROM) com CRC.           |
| **Communication**    | Serializa DTCs para protocolos (CAN, BLE, MQTT, Web).               |

O **Fault Manager** é o único responsável por **mudanças de estado** do DTC.
Os demais módulos apenas **consultam** ou **são notificados**.

---

# 6. Persistência

## 6.1 Quando salvar

- **Ao transicionar** para `ACTIVE` / `CRITICAL` / `LOCKED` (primeira ocorrência
  de um DTC).
- **Ao limpar** (`CLEARED`) — grava o `clearedAtMs` e finaliza o registro.
- **Ao mudar** o contador de ocorrência (cada nova ocorrência incrementa e persiste).
- **Em shutdown seguro**, garantindo que o último estado seja persistido.

## 6.2 Quando limpar

- **Auto-limpeza** (`ACTIVE → CLEARED`): quando a condição normaliza e o DTC não é
  latched.
- **Limpeza manual** (`LOCKED → CLEARED`): via reconhecimento (CLI/BLE/Dashboard).
- **Reset de fábrica**: apaga todo o histórico de DTC.

## 6.3 Histórico máximo

- Buffer em RAM (circular): `MAX_HISTORY = 50` registros (mantém o valor atual de
  `FaultHistory`).
- Persistência em storage: **máximo configurável**, ex. `MAX_PERSISTED = 200`
  registros (FIFO: descarta o mais antigo ao atingir o limite).
- O valor máximo é configurável via `config/config.h` e respeita o wear-leveling
  da memória flash.

## 6.4 Contador de ocorrência

- Cada DTC possui um **contador persistido** `occurrence`.
- Incrementado a cada transição para `ACTIVE` (nova ocorrência).
- **Não é zerado** na limpeza (`CLEARED` mantém o total).
- Usado para estatística de confiabilidade e manutenção preditiva.

## 6.5 Estrutura de armazenamento

Para garantir robustez, o storage do DTC será estendido sobre o `IStorageDriver`
abstrato existente, adicionando:

- **Cabeçalho com versão** (`DTC_STORAGE_VERSION`).
- **CRC** por registro e por bloco (usar `storage_crc` existente).
- **Índice de ocorrência** (contador persistido).
- **Migração** via `migration_manager` (compatível com versões futuras).

> O `FaultStorage` atual grava `FaultInfo` cru em `fault_history.bin` sem versão,
> CRC ou limite de tamanho. O modelo DTC **corrige** essas lacunas.

---

# 7. Integração futura

O modelo DTC é agnóstico de transporte: o mesmo `DtcRecord` é serializado para vários
canais.

| Canal        | Uso do DTC                                                          |
| ------------ | ------------------------------------------------------------------- |
| **CAN**      | Transmissão de códigos compactos em frames de diagnóstico (J1939/DTC). |
| **BLE**      | Notificação de DTCs ativos e leitura do histórico (GATT).           |
| **MQTT**     | Publicação de eventos DTC em JSON para backend/nuvem.               |
| **Dashboard Web** | Exibição de DTCs ativos, histórico e estatísticas em tempo real. |

## 7.1 Formato de serialização (exemplo JSON para MQTT/Web)

```json
{
  "dtc": "BMS-OV-001",
  "state": "ACTIVE",
  "severity": "CRITICAL",
  "source": 3,
  "value": 4.28,
  "limit": 4.25,
  "occurrence": 12,
  "timestampMs": 125034
}
```

> A integração em CAN/BLE/MQTT/Web reutiliza as camadas `communication/*` existentes
> (CAN Manager, BLE Service) e o `web/*` do dashboard.

---

# 8. Regras

As regras garantem a **independência de camadas** e a **portabilidade** do sistema
DTC (funciona em Mock, Wokwi e ESP32 físico).

| Regra                                                        | Invariante                                                   |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| **Domain não depende de hardware.**                           | `core/fault/*` não inclui nenhum header de HAL/ESP32.        |
| **Fault Manager não conhece ESP32.**                          | `FaultManager` opera apenas sobre `BatteryPack` e DTCs; sem `esp32_*`. |
| **Storage é abstraído.**                                      | Persistência via `IStorageDriver`; `FaultManager` não chama `fopen`/`fwrite`/NVS diretamente. |
| **Logger é independente.**                                    | `FaultManager` publica eventos via interface do `Logger` (EventQueue), sem acoplamento ao backend de log. |
| **Comunicação é downstream.**                                 | `FaultManager` não conhece CAN/BLE; expõe dados via API de consulta. |
| **Código DTC é estável.**                                     | Uma vez publicado, `BMS-XXX-NNN` não muda de significado.    |
| **Thread-safe / não bloqueante.**                             | Transições de estado rápidas; persistência em background.    |
| **Testável.**                                                 | Núcleo DTC puro em C++ (sem HW) testado em `tests/`.         |

Diagrama de dependências (as setas indicam dependência):

```text
       core/fault (FaultManager)   ← não depende de nada externo de HW
        │            │
        │            ├──► (interface) IStorageDriver   [system/storage]
        │            └──► (interface) Logger/EventQueue [system/logger]
        ▼
   Protection / Battery / Temperature / Current / Communication
        ▼
communication (CAN/BLE) ──► web (Dashboard)
```

---

# 9. Estrutura de código proposta

A implementação do sistema DTC será organizada respeitando a arquitetura em camadas
do projeto. Abaixo está a estrutura de arquivos proposta, que **substitui/refatora**
alguns arquivos atuais e adiciona novos módulos.

## 9.1 `core/fault/` (Domínio — núcleo DTC puro)

O núcleo DTC vive em `core/fault/` e **não depende de hardware**.

```
core/fault/
├── fault_code.h        # DtcCode, DtcCategory, DtcSeverity, DtcState (enums)
├── fault_event.h       # DtcRecord / FaultEvent (estrutura de dados da falha)
├── fault_manager.h     # FaultManager (interface do coordenador central)
├── fault_manager.cpp   # Implementação: motor de estado DTC, transições
├── fault_registry.h    # FaultRegistry (tabela de metadados dos códigos DTC)
├── fault_registry.cpp  # Implementação: cadastro/consulta de códigos DTC
├── fault_flags.h       # (mantido) FaultFlag – detecção binária de entrada
├── fault_reason.h      # (mantido) FaultReason / FaultInfo – compatibilidade
└── fault_history.h     # (mantido) FaultHistory – cache em RAM (RING buffer)
```

| Arquivo novo            | Responsabilidade                                              |
| ----------------------- | ------------------------------------------------------------- |
| `fault_code.h`          | Define os enums `DtcCode`, `DtcCategory`, `DtcSeverity`, `DtcState` (seções 2–4). |
| `fault_event.h`         | Define `DtcRecord` / `FaultEvent` (estrutura do registro persistido). |
| `fault_manager.cpp`     | Motor de estado DTC: converte evento → DTC, aplica transições, contador. |
| `fault_registry.{h,cpp}`| Registro/consulta dos códigos DTC (metadados: categoria, descrição, limiar). |

## 9.2 `system/storage/` (Infraestrutura — abstração de persistência)

A persistência é abstraída via `IStorageDriver`. O `FaultStorage` passa a gravar
`DtcRecord` com versão e CRC.

```
system/storage/
├── fault_storage.h      # FaultStorage (interface de persistência DTC)
└── fault_storage.cpp    # Implementação com versão + CRC + limite de histórico
```

| Mudança em `fault_storage`        | Descrição                                                        |
| --------------------------------- | --------------------------------------------------------------- |
| Gravar `DtcRecord` (em vez de cru) | Usa o formato com versão e CRC.                                 |
| Respeitar `MAX_PERSISTED` (FIFO)   | Descarta o registro mais antigo ao atingir o limite.             |
| Usar `IStorageDriver` / `storage_crc` | Abstrai o backend (flash/NVS/EEPROM) e valida integridade.   |

## 9.3 `communication/` (Camada de comunicação — downstream)

A comunicação é **consumidora** dos DTCs. Não é conhecida pelo `FaultManager`.

```
communication/
└── fault_protocol.h     # FaultProtocol (serialização de DTC para CAN/BLE/MQTT/Web)
└── fault_protocol.cpp   # Implementação: codifica/decodifica DtcRecord
```

| Arquivo novo            | Responsabilidade                                              |
| ----------------------- | ------------------------------------------------------------- |
| `fault_protocol.{h,cpp}`| Serializa `DtcRecord` para os transportes (CAN, BLE, MQTT, Web). |

## 9.4 Diagrama de dependência dos arquivos

```text
   core/fault (fault_code, fault_event, fault_registry, fault_manager)
        │                       │
        │                       └──► (interface) IStorageDriver
        │                                        │
        │                              system/storage/fault_storage.cpp
        │
        ▼
   communication/fault_protocol.cpp
        
   (FaultManager não inclui nenhum header de communication nem de HAL)
```

> **Regra de ouro:** `core/fault/*` não inclui `communication/*`, `system/*` de
> backend concreto, nem qualquer header de HAL/ESP32. A dependência é sempre do
> núcleo **para** interfaces abstratas.

---

# 10. Glossário

| Termo        | Definição                                                        |
| ------------ | ---------------------------------------------------------------- |
| **DTC**      | Diagnostic Trouble Code – código padrão que identifica uma falha.|
| **CAT**      | Categoria da falha (prefixo de 2 letras no código).              |
| **STATE**    | Estado de ciclo de vida da falha (ACTIVE, CLEARED, etc.).        |
| **WARNING**  | Pré-falha / alerta sem bloqueio de operação.                     |
| **CRITICAL** | Falha grave que limita ou desliga a operação.                    |
| **LOCKED**   | Falha latched que exige reset manual para limpar.                |
| **Occurrence**| Contador persistido de quantas vezes o DTC ocorreu.             |

---

# 11. Controle de versão

| Versão | Data       | Descrição                                             |
| ------ | ---------- | ----------------------------------------------------- |
| 1.0.0  | 04/07/2026 | Definição inicial do modelo DTC (conceito, código, categorias, estados, fluxo, persistência, integração e regras). Este documento é apenas arquitetura; nenhum código foi alterado. |

---

## Próximos passos sugeridos (fora do escopo deste documento)

1. Definir a `DtcTable` completa (mapeamento de todos os `FaultReason` para DTC).
2. Especificar a API do novo `FaultManager` orientado a DTC (ICD-010 atualizado).
3. Estender `FaultStorage` com versão, CRC e limite de histórico.
4. Adicionar testes unitários do núcleo DTC (`tests/test_unit_fault.cpp`).
