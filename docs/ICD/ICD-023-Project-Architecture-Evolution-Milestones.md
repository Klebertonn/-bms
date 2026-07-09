# ICD-023 — Project Architecture Evolution (Milestones)

## Escopo
Este documento define marcos (milestones) para evoluir do estado atual do projeto (módulos Core/Communication/Logger/Storage + RTOS stubs) até um firmware integrado e validado.

## Milestone 1 — Firmware integrado e executando no ESP32
**Objetivo:** o firmware inicia, cria a arquitetura base e executa loop/task mínimo.

**Checklist mínimo**
- TaskManager inicia o scheduler (xTaskCreate/vTaskStartScheduler)
- Logger inicializa e registra eventos
- StorageManager faz init/load
- Managers Core e Communication inicializam sem crash

## Milestone 2 — Comunicação CAN e BLE funcionando
**Objetivo:** telemetria e comandos básicos via CAN e BLE.

**Checklist mínimo**
- CAN RX/TX com protocolo/frames básicos
- BLE service com telemetria e comandos
- Logger publica/espelha eventos relevantes via CAN/BLE

## Milestone 3 — Testes em simulador HIL
**Objetivo:** executar cenários de falha e validação funcional.

**Checklist mínimo**
- Drivers HAL em modo mock/simulação
- Cenários: sobretensão, subtensão, sobrecorrente, curto, falha de sensor, balanceamento
- Relatórios de falhas e validação de proteção

## Milestone 4 — Validação em bancada com hardware real
**Objetivo:** validar medições reais e atuação de proteções.

**Checklist mínimo**
- Integração com sensores e Shunt/ADC reais
- Temporização e limites corretos (OVP/UVP/OCP/OTP)
- Watchdog e diagnósticos funcionais

## Milestone 5 — Versão candidata para produção
**Objetivo:** hardening final e release.

**Checklist mínimo**
- Atualizações de firmware com segurança (quando aplicável)
- CRC/backup/migração de storage consolidados
- Testes automatizados (mínimos) e documentação de release

