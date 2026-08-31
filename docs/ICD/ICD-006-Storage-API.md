# ICD-006 – Storage API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** DDS-010

---

# 1. Objetivo

Este documento define a interface pública do Storage Manager, responsável por persistir dados críticos do sistema BMS em memória não volátil.

Isso garante que o sistema mantenha histórico, configurações e estado mesmo após reinicialização ou perda de energia.

---

# 2. Responsabilidades

O Storage Manager deverá:

* Salvar configurações do sistema.
* Recuperar configurações na inicialização.
* Armazenar histórico de falhas.
* Registrar número de ciclos de carga/descarga.
* Persistir estatísticas de operação.
* Garantir integridade dos dados.
* Versionar estruturas de dados armazenadas.

---

# 3. Tipos de Dados Persistidos

## Configurações do Sistema

* Limites de tensão.
* Limites de corrente.
* Limites de temperatura.
* Configuração de balanceamento.
* Configuração de comunicação.

---

## Histórico de Falhas

* Tipo de falha.
* Timestamp.
* Estado do sistema no momento da falha.

---

## Ciclos da Bateria

* Número de ciclos completos.
* Profundidade de descarga (DoD).
* Eficiência média.

---

## Estatísticas

* Tempo total de operação.
* Tempo em carga.
* Tempo em descarga.
* Tempo em falha.

---

# 4. Estruturas de Dados

## StorageStatus

```cpp id="stg001"
enum class StorageStatus
{
    OK,
    ERROR,
    NOT_INITIALIZED,
    CORRUPTED,
    FULL
};
```

---

## StorageRecordType

```cpp id="stg002"
enum class StorageRecordType
{
    CONFIG,
    FAULT,
    CYCLE,
    STATISTICS
};
```

---

## StorageRecord

```cpp id="stg003"
struct StorageRecord
{
    uint32_t id;
    StorageRecordType type;
    uint32_t timestamp;
    uint16_t size;
    uint8_t data[128];
};
```

---

## CycleData

```cpp id="stg004"
struct CycleData
{
    uint32_t cycleCount;
    float depthOfDischarge;
    float efficiency;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="stg005"
StorageStatus storageInit();
```

---

## Salvar Configuração

```cpp id="stg006"
StorageStatus storageSaveConfig(const void* config, uint16_t size);
```

---

## Carregar Configuração

```cpp id="stg007"
StorageStatus storageLoadConfig(void* config, uint16_t size);
```

---

## Registrar Evento

```cpp id="stg008"
StorageStatus storageLogEvent(StorageRecordType type, const void* data, uint16_t size);
```

---

## Obter Histórico

```cpp id="stg009"
StorageStatus storageGetRecord(uint32_t id, StorageRecord* record);
```

---

## Apagar Histórico

```cpp id="stg010"
StorageStatus storageClear();
```

---

## Obter Estatísticas de Ciclos

```cpp id="stg011"
StorageStatus storageGetCycleData(CycleData* data);
```

---

## Incrementar Ciclo

```cpp id="stg012"
void storageIncrementCycle();
```

---

## Verificar Integridade

```cpp id="stg013"
bool storageIntegrityCheck();
```

---

# 6. Fluxo de Operação

```text id="stg014"
Sistema inicia
      │
      ▼
storageInit()
      │
      ▼
Carrega configs salvas
      │
      ▼
BMS executa normalmente
      │
      ▼
Eventos ocorrem
      │
      ▼
Storage registra logs e ciclos
      │
      ▼
Desligamento seguro
```

---

# 7. Integração

O Storage Manager interage com:

* Configuration Manager.
* Battery Manager.
* Protection Manager.
* Fault Manager.
* Logger.

---

# 8. Tratamento de Erros

Em caso de:

* Falha de escrita.
* Corrupção de dados.
* Memória cheia.

O sistema deverá:

* Ativar modo seguro.
* Registrar falha crítica.
* Tentar fallback em última configuração válida.

---

# 9. Requisitos

* Escrita não bloqueante.
* Compatível com NVS do ESP32.
* Suporte a versionamento de estrutura.
* Baixa taxa de desgaste da memória.

---

# 10. Critérios de Aceitação

O módulo será considerado válido quando:

* Dados persistirem após reboot.
* Falhas forem corretamente registradas.
* Configurações forem restauradas corretamente.
* Não houver corrupção de memória em testes prolongados.

---

# 11. Evoluções Futuras

* Criptografia de dados armazenados.
* Backup externo (CAN/Wi-Fi).
* Sincronização com cloud.
* Sistema de rollback de firmware baseado em estado.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                          |
| ------ | ---------- | ---------------------------------- |
| 1.0.0  | 03/07/2026 | Primeira definição da Storage API. |

