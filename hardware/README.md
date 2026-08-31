# hardware/

Documentação e arquivos de **hardware** da placa BMS.

Destinado a:
- Esquemáticos
- Pinagem / GPIO mapping
- Datasheets
- Arquivos de fabricação (Gerber, BOM, POS)
- Documentação da placa

> **Estado atual:** ainda não há arquivos de hardware. Esta pasta foi criada na
> reorganização do repositório (Sprint de estruturação) para receber futuramente
> esses artefatos.

## Referência de pinagem (config)
A pinagem de GPIO do firmware é definida em `config/config.h` (no diretório raiz).

## Mapeamento para outros artefatos
- Sensores: `lib/hal/` (interfaces) e `src/hal/esp32/` (drivers reais).
- Especificação de hardware: `docs/SRS/SRS-003-Hardware.md`.
