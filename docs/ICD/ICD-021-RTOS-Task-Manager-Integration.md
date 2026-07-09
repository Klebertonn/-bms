# ICD-021 — RTOS Task Manager Integration

## Objetivo
Definir como o `system/rtos/task_manager` conecta-se com os managers já existentes (Core, Communication, Logger, Storage).

## Componentes
- `TaskManager` (system/rtos)
- Managers de Core: Battery, Current, Temperature, Balance, SOC
- Communication: CAN, BLE
- System: Logger, Storage

## Contratos de comunicação entre tasks (proposta)
- **Queues**: medições (Current/Temperature/Battery) e comandos (Protection/Balance/SOC/Storage)
- **Event Groups**: falhas/proteções, estados de operação (carregando/balanceando)
- **Mutex**: proteção de estruturas compartilhadas (telemetria atual)
- **Watchdog**: heartbeats por task crítica

## Prioridades e períodos
Tabela (stub conforme `system/rtos/task_definitions.h`):
- Protection Task — 5ms — Muito Alta
- Battery Task — 10ms — Alta
- Current Task — 10ms — Alta
- Temperature Task — 20ms — Alta
- Balance Task — 100ms — Média
- SOC Task — 100ms — Média
- Logger Task — 100ms — Média
- CAN TX Task — 20ms — Média
- CAN RX Task — Evento — Alta
- BLE Task — 50ms — Média
- Storage Task — sob demanda — Baixa
- Display Task — 250ms — Baixa
- Diagnostics Task — 1s — Baixa
- Watchdog Task — 500ms — Muito Alta

## Próximos passos
- Implementar queues reais e tipos de mensagens.
- Implementar `xTaskCreate`/`vTaskStartScheduler` no `TaskManager`.
- Implementar watch-dog com heartbeats.

