# Logger / Event Manager (system/logger)

Módulo central para registro de eventos do BMS.

## Objetivo
Registrar eventos relevantes para rastreabilidade, diagnóstico e auditoria (histórico).

## Responsabilidades (placeholder inicial)
- Registrar eventos
- Registrar falhas/alarmes
- Enfileirar logs e dar flush
- Publicar (por enquanto: console/printf)
- Persistir (stub: backends reais depois)

## Fluxo
Outros módulos (Protection/Battery/Temperature/Current/SOC/Balance/BLE/CAN) chamam `Logger::log()`.

---

## Status
- Implementação de ring buffer e formatter
- Persistência e múltiplos backends ainda são stubs

