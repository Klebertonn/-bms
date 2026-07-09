# Current Manager (CORE)

Módulo responsável por medição/filtragem e interpretação da corrente do pack.

## Responsabilidades
- Ler a corrente do pack através da HAL (`ICurrentSensor`).
- Aplicar filtro digital para reduzir ruído.
- Detectar direção (carga/descarga/idle).
- Calcular potência instantânea (P = V * I).
- Fornecer dados para outros módulos (SOC/SOH, Proteção, Logger, Display, BLE/CAN).

## Versão atual
- Média atual é equivalente ao valor filtrado (arquitetura pronta para evoluir para janela temporal).

