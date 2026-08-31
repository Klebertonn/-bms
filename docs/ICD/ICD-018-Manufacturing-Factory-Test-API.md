# ICD-018 – Manufacturing & Factory Test API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001 a ICD-017

---

# 1. Objetivo

O Manufacturing & Factory Test Manager fornece uma infraestrutura para validação, calibração e certificação do hardware durante a fabricação e manutenção do BMS.

O módulo permite executar testes automáticos em todos os periféricos, registrar resultados e garantir que apenas equipamentos aprovados sejam liberados para operação.

---

# 2. Responsabilidades

O Factory Test deverá:

* Executar autotestes de hardware.
* Validar sensores.
* Validar comunicação.
* Calibrar medições.
* Registrar resultados.
* Gerar relatório de aprovação.
* Gravar informações de produção.

---

# 3. Modos de Operação

## FactoryMode

```cpp id="factory001"
enum class FactoryMode
{
    DISABLED,
    SELF_TEST,
    CALIBRATION,
    FINAL_INSPECTION,
    BURN_IN
};
```

---

## FactoryResult

```cpp id="factory002"
enum class FactoryResult
{
    PASSED,
    FAILED,
    WARNING,
    ABORTED
};
```

---

# 4. Estruturas

## FactoryReport

```cpp id="factory003"
struct FactoryReport
{
    FactoryResult result;
    uint32_t testsExecuted;
    uint32_t testsPassed;
    uint32_t testsFailed;
    char firmwareVersion[16];
    char serialNumber[32];
    uint32_t timestamp;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="factory004"
bool factoryInit();
```

---

## Entrar em Modo de Fábrica

```cpp id="factory005"
bool factoryEnterMode(FactoryMode mode);
```

---

## Executar Testes

```cpp id="factory006"
FactoryResult factoryRunAllTests();
```

---

## Calibrar Sistema

```cpp id="factory007"
bool factoryCalibrate();
```

---

## Gerar Relatório

```cpp id="factory008"
FactoryReport factoryGenerateReport();
```

---

## Sair do Modo de Fábrica

```cpp id="factory009"
void factoryExitMode();
```

---

## Executar Autoteste

```cpp id="factory010"
bool factorySelfTest();
```

---

# 6. Testes Obrigatórios

O processo deverá validar:

* Inicialização do ESP32.
* Memória Flash.
* RAM disponível.
* Watchdog.
* ADC.
* Sensores de tensão.
* Sensores de temperatura.
* Sensor de corrente.
* MOSFET de carga.
* MOSFET de descarga.
* Balanceamento.
* Interface CAN.
* Interface BLE.
* Display.
* Armazenamento NVS.
* Logger.
* Fonte de alimentação.

---

# 7. Fluxo de Operação

```text id="factory011"
Entrar em modo fábrica
        │
        ▼
Inicializar módulos
        │
        ▼
Executar autotestes
        │
        ▼
Calibrar sensores
        │
        ▼
Executar testes funcionais
        │
        ▼
Gerar relatório
        │
        ▼
Aprovado?
   ┌────┴────┐
   │         │
  Sim       Não
   │         │
   ▼         ▼
Liberar   Bloquear unidade
```

---

# 8. Integração

O Factory Test utiliza:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Logger.
* Storage Manager.
* Fault Manager.
* Communication Manager.
* Display Manager.
* Test Framework.

---

# 9. Tratamento de Erros

Caso um teste falhe:

* Registrar a falha no Logger.
* Marcar a unidade como reprovada.
* Impedir a liberação do equipamento.
* Permitir nova execução após correção.

---

# 10. Requisitos

* Execução automatizada.
* Compatível com Mock, Wokwi e ESP32 físico.
* Registro permanente dos resultados.
* Baixo impacto no firmware operacional.
* Interface acessível via CLI.

---

# 11. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Executar todos os testes previstos.
* Gerar relatório completo.
* Registrar falhas corretamente.
* Permitir calibração dos sensores.
* Validar periféricos críticos.

---

# 12. Evoluções Futuras

* Integração com banco de dados de produção.
* Geração de etiquetas com QR Code.
* Assinatura digital do relatório.
* Exportação em PDF e CSV.
* Integração com sistemas MES/ERP.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                               |
| ------ | ---------- | ------------------------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Manufacturing & Factory Test API. |

