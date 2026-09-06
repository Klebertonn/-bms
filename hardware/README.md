# hardware/

Documentação e arquivos de **hardware** da placa BMS.

Destinado a:
- Esquemáticos
- Pinagem / GPIO mapping
- Datasheets
- Arquivos de fabricação (Gerber, BOM, POS)
- Documentação da placa

## Especificacoes atuais

- [Especificacao eletrica do prototipo 3S com ESP32 DevKit](../docs/HARDWARE_3S_ESP32_PROTOTYPE.md)
- [BOM inicial da bancada 3S](../docs/BOM_3S_BENCH.md)
- [BOM detalhada com modelos e marcas](../docs/BOM_3S_BENCH_DETAILED.md)
- [Lista de compra completa da bancada](../docs/LISTA_COMPRA_BANCADA_3S.md)
- [Plano de teste de bancada](../docs/BENCH_TEST_PLAN_3S.md)
- [Roadmap da PCB BMS propria](../docs/BMS_OWN_PCB_ROADMAP.md)
- [Desenho da bancada 3S + ESP32 DevKit](../docs/DIAGRAM_BANCADA_3S_ESP32.md)
- [Desenho da futura PCB BMS 3S](../docs/DIAGRAM_PCB_BMS_3S.md)
- [Netlist de montagem da bancada](../docs/NETLIST_3S_BENCH.md)

> **Estado atual:** ainda nao ha esquematico CAD, PCB, Gerber ou BOM de
> producao. Os documentos acima sao a especificacao de entrada para a fase de
> bancada e nao autorizam ligar um pack sem o procedimento de seguranca.

## Referência de pinagem (config)
A pinagem de GPIO do firmware é definida em `config/config.h` (no diretório raiz).

## Mapeamento para outros artefatos
- Sensores: `lib/hal/` (interfaces) e `src/hal/esp32/` (drivers reais).
- Especificação de hardware: `docs/SRS/SRS-003-Hardware.md`.
