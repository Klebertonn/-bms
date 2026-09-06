# Changelog

Todas as mudanças notáveis deste projeto serão documentadas neste arquivo.

O formato segue [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/) e o versionamento adere ao [SemVer](https://semver.org/lang/pt-BR/).

---

## [Unreleased] - 2026-09-04

### Corrigido
- Corrigido o filtro de fontes e o link das bibliotecas locais no alvo ESP32.
- Corrigidas incompatibilidades dos drivers GPIO, INA219, CAN e BLE com o toolchain instalado.
- Adicionado teste de integração do ciclo `App::init()` + `App::update()`.
- Registrado o baseline nativo de 35/35 testes aprovados.
- Adicionados 6 cenários de fault injection para proteção, recuperação e MOSFETs.
- Implementado executor endurance nativo com métricas e relatório.
- Adicionada telemetria endurance periódica com dados reais do pack e da aplicação.
- Registrados ensaios endurance aprovados de 5 e 30 minutos.
- Registrado ensaio endurance aprovado de 1 hora.
- Melhorada a medição de latência do loop para resolução de microssegundos.
- Corrigida a telemetria para refletir o estado operacional `FAULT`.
- Implementado watchdog de software com timeout e diagnóstico de perdas.
- Ativado o Task Watchdog Timer do ESP32 e o registro da causa de reset.
- Adicionado relatório consolidado de validação e riscos de liberação.

## [1.0.0] - 2026-07-03

### Adicionado
- **Self Test (POST)** integrado à máquina de estados.
  - Validação de ADC, EEPROM, FLASH, NVS, Current Sensor, Temperature Sensor, MOSFET Driver e RAM.
  - Fluxo `BOOT → INIT → SELF_TEST → READY` (ou `FAULT` em caso de falha).
- **Interfaces HAL** com suporte a inicialização (`begin()`):
  - `ICurrentSensor`, `ISOCSensor`, `ICANDriver`.
- **Mocks atualizados** para as novas interfaces (`begin()`).
- **Documentação de produção**:
  - `docs/ARCHITECTURE.md`
  - `docs/PRODUCTION.md` (checklist industrial)
  - `docs/RELEASE.md`
  - `README.md` completo
  - `CHANGELOG.md`
  - `LICENSE` (MIT)
  - `SECURITY.md`
  - `CONTRIBUTING.md`
- **Estrutura pronta para GitHub**:
  - `.gitignore` atualizado.
- **Web Dashboard** (telemetria) — `web/`.
- **Scheduler & Heartbeat** — agendamento de tarefas e supervisão.

### Melhorado
- Padronização de formatação no `src/app/app.h`.
- Documentação de arquitetura e fluxo de dados.

### Corrigido
- Inicialização de sensores de hardware no `main_esp32.cpp` (via `begin()` nas interfaces).

---

## [0.x] - Versões anteriores

Versões de desenvolvimento anteriores (pré-1.0) incluíram:

- State Machine BMS.
- Fault Manager e Fault History.
- Protection Manager.
- SOC.
- Temperature Manager.
- Current Manager.
- Balance Manager.
- MOSFET Controller.
- Logger & Diagnostics.
- Storage (NVS, EEPROM, FLASH, arquivo).
- CAN (TWAI/MCP2515/mock).
- BLE (GATT).
- Testes Unity.

Consulte o histórico de commits para detalhes completos.

<!-- Links de comparação -->
[1.0.0]: https://github.com/SEU_USUARIO/bms/releases/tag/v1.0.0
