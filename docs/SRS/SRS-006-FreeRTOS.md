# SRS-006 – FreeRTOS e Execução em Tempo Real

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Em desenvolvimento
**Dependências:** SRS-001 a SRS-005

---

# 1. Objetivo

Este documento define a arquitetura de execução em tempo real do firmware utilizando FreeRTOS.

O objetivo é garantir:

* Execução determinística.
* Baixa latência para funções críticas.
* Isolamento entre módulos.
* Escalabilidade.
* Robustez.

---

# 2. Princípios

O firmware deverá operar utilizando múltiplas tarefas independentes.

Cada tarefa será responsável por apenas uma função.

Nenhuma tarefa poderá bloquear outra.

Nenhum módulo utilizará delay() para sincronização.

---

# 3. Scheduler

Será utilizado o scheduler padrão do FreeRTOS fornecido pelo ESP32.

A inicialização ocorrerá durante o boot do firmware.

---

# 4. Organização das Tasks

## TaskSensors

Responsabilidades:

* Ler tensões das células.
* Ler tensão do pack.
* Ler corrente.
* Ler temperaturas.

Periodicidade:

10 ms

Prioridade:

Alta

---

## TaskProtection

Responsabilidades:

* Verificar OVP.
* Verificar UVP.
* Verificar OCP.
* Verificar OTP.
* Verificar UTP.
* Atualizar Fault Manager.

Periodicidade:

10 ms

Prioridade:

Máxima

---

## TaskBattery

Responsabilidades:

* Atualizar SOC.
* Atualizar SOH.
* Calcular energia.
* Calcular capacidade restante.

Periodicidade:

100 ms

Prioridade:

Alta

---

## TaskBalancing

Responsabilidades:

* Verificar necessidade de balanceamento.
* Acionar MOSFETs de balanceamento.
* Monitorar tempo de balanceamento.

Periodicidade:

500 ms

Prioridade:

Média

---

## TaskCAN

Responsabilidades:

* Enviar telemetria.
* Enviar alarmes.
* Receber comandos.

Periodicidade:

100 ms

Prioridade:

Média

---

## TaskBLE

Responsabilidades:

* Atualizar características BLE.
* Processar comandos do aplicativo.

Periodicidade:

200 ms

Prioridade:

Baixa

---

## TaskDisplay

Responsabilidades:

* Atualizar OLED.
* Exibir alarmes.
* Alternar telas.

Periodicidade:

250 ms

Prioridade:

Baixa

---

## TaskLogger

Responsabilidades:

* Registrar eventos.
* Registrar falhas.
* Exportar logs.

Periodicidade:

100 ms

Prioridade:

Baixa

---

## TaskCLI

Responsabilidades:

* Processar comandos da Serial.

Execução sob demanda.

---

## TaskStorage

Responsabilidades:

* Salvar configurações.
* Atualizar histórico.
* Persistir dados.

Execução sob demanda.

---

## TaskDiagnostics

Responsabilidades:

* Monitorar tempo de execução.
* Verificar uso de memória.
* Alimentar Watchdog.
* Gerar estatísticas.

Periodicidade:

1000 ms

Prioridade:

Baixa

---

# 5. Comunicação entre Tasks

As tarefas deverão utilizar:

* Queues.
* Mutexes.
* Event Groups.
* Semáforos.

Variáveis globais compartilhadas deverão ser evitadas.

---

# 6. Prioridades

| Task            | Prioridade |
| --------------- | ---------- |
| TaskProtection  | Máxima     |
| TaskSensors     | Alta       |
| TaskBattery     | Alta       |
| TaskBalancing   | Média      |
| TaskCAN         | Média      |
| TaskBLE         | Baixa      |
| TaskDisplay     | Baixa      |
| TaskLogger      | Baixa      |
| TaskCLI         | Baixa      |
| TaskStorage     | Baixa      |
| TaskDiagnostics | Baixa      |

---

# 7. Tempo Máximo de Execução

Cada tarefa deverá concluir sua execução dentro do período previsto.

Exemplos:

* TaskProtection: até 2 ms
* TaskSensors: até 5 ms
* TaskBattery: até 5 ms
* TaskCAN: até 5 ms
* TaskBLE: até 10 ms
* TaskDisplay: até 20 ms

Caso exceda o limite, um evento deverá ser registrado.

---

# 8. Watchdog

Todas as tarefas críticas deverão informar periodicamente seu funcionamento.

Se uma tarefa deixar de responder:

* Registrar evento.
* Registrar falha.
* Reiniciar o sistema quando necessário.

---

# 9. Inicialização

Sequência recomendada:

1. Inicializar HAL.
2. Inicializar armazenamento.
3. Inicializar logger.
4. Inicializar sensores.
5. Inicializar comunicação.
6. Criar todas as tarefas.
7. Iniciar scheduler.

---

# 10. Fluxo Geral

```text
Boot
   │
   ▼
HAL Init
   │
   ▼
Storage Init
   │
   ▼
Logger Init
   │
   ▼
Create Tasks
   │
   ▼
Scheduler
   │
   ├── TaskSensors
   ├── TaskProtection
   ├── TaskBattery
   ├── TaskBalancing
   ├── TaskCAN
   ├── TaskBLE
   ├── TaskDisplay
   ├── TaskLogger
   ├── TaskCLI
   ├── TaskStorage
   └── TaskDiagnostics
```

---

# 11. Uso dos Núcleos do ESP32

Arquitetura recomendada:

Core 0

* Sensores
* Proteção
* Battery
* Balanceamento

Core 1

* CAN
* BLE
* Display
* Logger
* CLI
* Diagnóstico

Essa distribuição deverá ser configurável e poderá ser ajustada conforme testes de desempenho.

---

# 12. Critérios de Aceitação

A arquitetura em tempo real será considerada aprovada quando:

* Todas as tarefas forem executadas nos períodos definidos.
* Nenhuma tarefa bloquear outra.
* As filas não apresentarem perdas recorrentes.
* O watchdog detectar travamentos.
* O sistema permanecer estável durante longos períodos de operação.

---

# 13. Evoluções Futuras

A arquitetura deverá permitir:

* Inclusão de novas tarefas.
* Ajuste dinâmico de prioridades.
* Agendamento configurável.
* Monitoramento de carga do processador.
* Estatísticas de tempo de execução por tarefa.

---

# 14. Controle de Versão

| Versão | Data       | Descrição                                                                |
| ------ | ---------- | ------------------------------------------------------------------------ |
| 1.0.0  | 03/07/2026 | Definição inicial da arquitetura de execução em tempo real com FreeRTOS. |

