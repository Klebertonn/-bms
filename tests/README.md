# tests/

Testes unitários, de integração e de sistema do BMS.

> **Nota:** esta pasta contém os testes legados da reorganização. O PlatformIO
> usa o diretório padrão `test/` e executa atualmente as suítes ativas em
> `test/native/`; estes arquivos só devem voltar ao gate quando forem
> convertidos para suítes compatíveis com o runner.

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
