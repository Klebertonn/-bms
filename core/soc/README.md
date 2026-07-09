# SOC/SOH Manager (CORE)

Módulo responsável por estimar:
- SOC (State of Charge)
- SOH (State of Health)
- contagem de coulombs
- ciclos
- energia restante e consumida

## Implementação (versão inicial)
- CoulombCounter faz a contagem de carga/descarga a partir de `pack.current` e do tempo (`deltaTimeSeconds`).
- SOC é calculado como estimativa híbrida:
  - 30% por tensão do pack
  - 70% por coulomb counting

## Dependências
- `PackData` (do `BatteryManager`): `totalVoltage`, `current`
- `config/config.h` (valores nominais; nesta etapa podem ser defaults stubs)

