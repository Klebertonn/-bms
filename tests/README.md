# tests/

Testes unitários, de integração e de sistema do BMS.

> **Nota:** esta pasta foi renomeada de `test/` para `tests/` na reorganização do
> repositório. O `platformio.ini` foi atualizado com `test_dir = tests`.

## Framework
- **Unity** (via PlatformIO) — `test_framework = unity` em `platformio.ini`.

## Como executar

```bash
pio test -e native
```

## Geração de relatório
- `scripts/generate_report.py` lê os resultados e gera um relatório.

## Arquivos
- `test_unit_*.cpp` — testes unitários (scheduler, state machine, CAN, BLE, storage, self test, fault).
- `test_integration.cpp` — testes de integração entre módulos.
