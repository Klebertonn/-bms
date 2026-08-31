# ICD-022 — Task Heartbeat Policy

## Objetivo
Padronizar o heartbeat utilizado pelo Watchdog Manager.

## Regras
- Cada tarefa crítica deve chamar `WatchdogManager::heartbeat(taskIndex)`.
- O watchdog avalia janelas (timeout) por tarefa.
- Em caso de falha:
  1) registrar evento crítico no Logger
  2) publicar alarme via CAN e BLE
  3) tentar recuperação (reset de fila/estado)
  4) reiniciar sistema (watchdog reset) se persistir

## Notas
Nesta fase atual, o Watchdog Manager está como stub; integração real será feita no target ESP32.

