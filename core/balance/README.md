# Balance Manager (CORE)

Módulo responsável por balanceamento de células (BMS profissional).

## Objetivos
- Detectar diferenças de tensão entre células.
- Selecionar células para balanceamento.
- Acionar MOSFETs de balanceamento (integração via `balance_driver` na etapa seguinte).
- Interromper/pausar balanceamento em caso de falha crítica (delegado ao `ProtectionManager`).
- Respeitar limites configuráveis de tensão/temperatura.
- Expor status para Logger/Display/BLE/CAN (integração posterior).

## Fluxo (versão inicial)
1. `BatteryManager` calcula `PackData` (ex.: `deltaVoltage`, `temperature`).
2. `ProtectionManager` valida se há falha crítica.
3. `BalanceManager` só permite balancear quando:
   - `protection.isFault() == false`
   - `temperature <= MAX_BALANCE_TEMP`
   - `deltaVoltage >= BALANCE_START_DELTA`
4. Seleciona a célula de maior tensão e habilita flag de MOSFET.

> Nesta fase, os MOSFETs são representados por flags (`cellEnabled[]`). A camada `hal/balance_driver` deve traduzir isso para o hardware.

