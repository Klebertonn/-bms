# FreeRTOS Task Manager (system/rtos)

Estrutura base para arquitetura FreeRTOS.

## Status
- Implementação atual: **stubs** (camadas/arquitetura), sem criar tarefas reais.
- Próximo passo: integrar com FreeRTOS (xTaskCreate, queues, mutexes, event groups) no target ESP32/PlatformIO.

## Objetivo
Separar responsabilidades por tarefas para reduzir latência e aumentar confiabilidade.

## Próximas etapas recomendadas
1. Definir as filas de medições e comandos entre tasks.
2. Conectar handlers do Battery/Current/Temperature/SOC/Protection/Balancing/Logger/CAN/BLE.
3. Implementar watchdog com heartbeats por tarefa.
4. Medir stack high-water mark e tempo de execução.

