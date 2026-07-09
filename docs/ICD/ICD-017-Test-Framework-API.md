# ICD-017 – Test Framework API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-001 a ICD-016

---

# 1. Objetivo

O Test Framework fornece uma infraestrutura para execução de testes automatizados do firmware, permitindo validar módulos individuais, integrações e o sistema completo em diferentes plataformas (Mock, Wokwi e ESP32).

---

# 2. Responsabilidades

O Test Framework deverá:

* Registrar casos de teste.
* Executar testes unitários.
* Executar testes de integração.
* Executar testes de sistema.
* Registrar resultados.
* Gerar estatísticas de aprovação.
* Integrar-se ao Logger e ao CLI.

---

# 3. Tipos de Teste

## TestType

```cpp id="test001"
enum class TestType
{
    UNIT,
    INTEGRATION,
    SYSTEM,
    PERFORMANCE,
    STRESS,
    HARDWARE
};
```

---

## TestResult

```cpp id="test002"
enum class TestResult
{
    PASSED,
    FAILED,
    SKIPPED,
    ERROR
};
```

---

# 4. Estruturas

## TestCase

```cpp id="test003"
struct TestCase
{
    const char* name;
    TestType type;
    bool (*execute)();
};
```

---

## TestReport

```cpp id="test004"
struct TestReport
{
    uint32_t totalTests;
    uint32_t passedTests;
    uint32_t failedTests;
    uint32_t skippedTests;
    uint32_t executionTimeMs;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="test005"
bool testFrameworkInit();
```

---

## Registrar Teste

```cpp id="test006"
bool testRegister(const TestCase& test);
```

---

## Executar Teste

```cpp id="test007"
TestResult testRun(const char* name);
```

---

## Executar Todos

```cpp id="test008"
TestReport testRunAll();
```

---

## Obter Relatório

```cpp id="test009"
TestReport testGetReport();
```

---

## Limpar Resultados

```cpp id="test010"
void testClearResults();
```

---

# 6. Fluxo de Operação

```text id="test011"
Inicialização
      │
      ▼
Registrar testes
      │
      ▼
Selecionar teste
      │
      ▼
Executar
      │
 ┌────┴─────┐
 │          │
 ▼          ▼
PASS      FAIL
 │          │
 ▼          ▼
Registrar resultado
      │
      ▼
Gerar relatório
```

---

# 7. Integração

O Test Framework poderá testar:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Communication Manager.
* Logger.
* Storage.
* Configuration.
* Task Manager.
* Watchdog.
* Fault Manager.
* BLE.
* CAN.
* Display.

---

# 8. Exemplos de Casos de Teste

* Inicialização do Battery Manager.
* Detecção de sobretensão.
* Detecção de subtensão.
* Balanceamento automático.
* Registro de falhas.
* Persistência de configurações.
* Comunicação CAN.
* Comunicação BLE.
* Atualização OTA.
* Recuperação após Watchdog.

---

# 9. Tratamento de Erros

Em caso de falha:

* Registrar detalhes no Logger.
* Informar o CLI.
* Marcar o teste como FAILED.
* Permitir repetição do teste.

---

# 10. Requisitos

* Compatível com Mock, Wokwi e ESP32.
* Execução automatizada.
* Baixo impacto no firmware.
* Relatórios claros.
* Fácil adição de novos testes.

---

# 11. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Executar todos os testes registrados.
* Gerar relatório consolidado.
* Identificar corretamente testes aprovados e reprovados.
* Integrar-se ao Logger e ao CLI.

---

# 12. Evoluções Futuras

* Integração com CI/CD.
* Geração de relatórios em HTML e JSON.
* Cobertura de código.
* Testes de carga e desempenho automatizados.
* Execução remota via CLI ou BLE.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                 |
| ------ | ---------- | ----------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Test Framework API. |

