# DDS-009 – Task Manager (FreeRTOS)

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001 até DDS-008

---

# 1. Objetivo

O Task Manager é responsável por organizar a execução concorrente dos módulos do firmware utilizando o FreeRTOS.

Seu objetivo é garantir previsibilidade, escalabilidade e isolamento entre tarefas críticas e não críticas.

---

# 2. Objetivos

* Separar responsabilidades em tarefas independentes.
* Garantir prioridade às funções de segurança.
* Evitar bloqueios.
* Melhorar desempenho.
* Facilitar manutenção e expansão.

---

# 3. Arquitetura de Tarefas

```text
                Scheduler (FreeRTOS)
                        │
 ┌─────────────┬──────────────┬──────────────┐
 │             │              │              │
 ▼             ▼              ▼              ▼
Safety      Battery      Communication     Logger
Task         Task            Task           Task
 │             │              │              │
 ├──────┐      │      ┌───────┴────────┐     │
 ▼      ▼      ▼      ▼                ▼     ▼
Balance OLED  Storage CAN            BLE   CLI
```

---

# 4. Lista de Tarefas

## Safety Task

Prioridade: Muito Alta

Período: 10 ms

Responsável por:

* OVP
* UVP
* OCP
* OTP
* UTP
* SCP
* Ações de segurança

---

## Battery Task

Prioridade: Alta

Período: 20 ms

Responsável por:

* Leitura dos sensores.
* Cálculo do SOC.
* Cálculo do SOH.
* Atualização do estado da bateria.

---

## Balance Task

Prioridade: Alta

Período: 100 ms

Responsável por:

* Seleção das células.
* Controle dos MOSFETs.
* Monitoramento do balanceamento.

---

## Communication Task

Prioridade: Média

Período: 50 ms

Responsável por:

* CAN.
* BLE.
* UART.
* CLI.

---

## Display Task

Prioridade: Baixa

Período: 250 ms

Responsável por:

* OLED.
* Dashboard.
* Telas rotativas.

---

## Logger Task

Prioridade: Baixa

Período: 500 ms

Responsável por:

* Registro de eventos.
* Histórico.
* Estatísticas.
* Exportação.

---

## Storage Task

Prioridade: Baixa

Execução sob demanda.

Responsável por:

* NVS.
* EEPROM.
* Configurações.
* Histórico persistente.

---

# 5. Comunicação entre Tarefas

A comunicação utilizará recursos do FreeRTOS:

* Queues.
* Mutexes.
* Semáforos binários.
* Semáforos de contagem.
* Event Groups.
* Timers.

---

# 6. Watchdog

Cada tarefa deverá informar periodicamente sua execução.

Caso uma tarefa crítica deixe de responder:

* Registrar evento.
* Reiniciar a tarefa (quando possível).
* Executar recuperação segura.
* Reiniciar o sistema, se necessário.

---

# 7. Afinidade com os Núcleos

Sugestão para ESP32:

**Core 0**

* Safety Task
* Battery Task
* Balance Task

**Core 1**

* Communication Task
* Display Task
* Logger Task
* CLI
* BLE

Essa distribuição busca reduzir interferências entre tarefas críticas e de interface.

---

# 8. Sincronização

O acesso a recursos compartilhados deverá ser protegido.

Recursos críticos:

* Estrutura global da bateria.
* Logger.
* NVS.
* Barramentos de comunicação.

---

# 9. Estados das Tarefas

Cada tarefa poderá estar em:

* Criada.
* Inicializada.
* Executando.
* Suspensa.
* Bloqueada.
* Finalizada.

O Task Manager será responsável por acompanhar esses estados.

---

# 10. Inicialização

Sequência recomendada:

1. Inicializar HAL.
2. Inicializar Drivers.
3. Inicializar Managers.
4. Criar Tarefas.
5. Criar Queues.
6. Criar Semáforos.
7. Iniciar Scheduler.

---

# 11. Tratamento de Erros

Caso uma tarefa apresente falha:

* Registrar no Logger.
* Informar o Fault Manager.
* Tentar recuperação.
* Se necessário, entrar em modo seguro.

---

# 12. Configuração

Parâmetros configuráveis:

* Prioridades.
* Períodos.
* Tamanho das pilhas.
* Afinidade de núcleo.
* Tempo do Watchdog.

---

# 13. Critérios de Desempenho

* Todas as tarefas devem cumprir seus períodos.
* Não deve haver starvation.
* O uso de CPU deverá permanecer dentro dos limites definidos.
* O tempo de resposta das tarefas críticas deve ser determinístico.

---

# 14. Critérios de Aceitação

O Task Manager será considerado aprovado quando:

* Todas as tarefas forem criadas corretamente.
* A comunicação entre tarefas funcionar sem perdas.
* O Watchdog detectar falhas conforme esperado.
* O sistema operar de forma estável em Mock, Wokwi e ESP32 físico.

---

# 15. Evoluções Futuras

* Escalonamento adaptativo.
* Balanceamento de carga entre núcleos.
* Monitor de desempenho em tempo real.
* Atualização dinâmica de tarefas.
* Integração com ferramentas de análise RTOS.

---

# 16. Controle de Versão

| Versão | Data       | Descrição                                                  |
| ------ | ---------- | ---------------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Especificação inicial do Task Manager baseado em FreeRTOS. |

