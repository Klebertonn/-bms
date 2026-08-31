# Temperature Manager (CORE)

Módulo responsável por medição/filtragem e interpretação das temperaturas do pack.

## Responsabilidades
- Ler múltiplos sensores de temperatura via HAL (`ITemperatureSensor`).
- Aplicar filtro digital por sensor.
- Calcular temperatura mínima, máxima e média.
- Detectar falhas (curto/aberto/valor inválido).
- Fornecer estatísticas para Proteção, Balanceamento, SOC/SOH, Logger, Display e BLE/CAN.

## Versão atual
- Filtro IIR simples (alpha = 0.15).
- Falhas detectadas por faixas:
  - `< -50°C` => SHORT_CIRCUIT
  - `> 150°C` => DISCONNECTED

