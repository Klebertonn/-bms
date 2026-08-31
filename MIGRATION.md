# Plano de Migração — Reorganização do Repositório

**Abordagem:** Opção B (reorganização estrutural leve, baixo risco).

## Objetivo
Criar a estrutura-alvo `docs/`, `hardware/`, `firmware/`, `simulation/`, `scripts/`, `tools/`, `tests/`
sem alterar comportamento, lógica, includes ou quebrar `pio run -e native`.

## Estrutura Alvo
- `docs/` — documentação (SRS, DDS, ICD, diagramas, relatórios, arquitetura)
- `hardware/` — esquemas, pinagem, datasheets (novo, placeholder)
- `firmware/` — código embarcado (mapeado via README; código físico permanece na raiz para não quebrar includes)
- `simulation/` — mocks/ambiente native (mapeado via README)
- `scripts/` — scripts de automação (report)
- `tools/` — ferramentas auxiliares (novo, placeholder)
- `tests/` — testes unitários/integração/sistema

## Arquivos a Movidos (baixo risco — não impactam build)
| De | Para |
|----|------|
| `test/` | `tests/` |
| `test/generate_report.py` | `scripts/generate_report.py` |
| `web/` | `simulation/` → mantém referência (dashboard) |

## Impacto no PlatformIO
- O código-fonte (`core/`, `system/`, `communication/`, `lib/`, `src/`, `config/`) **permanece na raiz**.
- `platformio.ini` permanece na raiz e **não é alterado**.
- O build `pio run -e native` **não é impactado**.

## Riscos
- **Baixo:** mover `test/` → `tests/` pode afetar localização do `test_dir` do PlatformIO. Verificar se `test_dir` é configurado; se for `test/` padrão, mover pode quebrar `pio test`. **Mitigação:** documentar e, se necessário, ajustar `test_dir` no platformio.ini (não altera `pio run`).

## Validação
- `pio run -e native` → SUCCESS
- `program.exe` → runtime BMS OK
