# SRS-007 – Diagnóstico e Logging

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Em desenvolvimento
**Dependências:** SRS-001 a SRS-006

---

# 1. Objetivo

Este documento define a arquitetura de diagnóstico, monitoramento e registro de eventos do firmware BMS.

O sistema de diagnóstico deverá permitir identificar rapidamente falhas, acompanhar o estado do sistema e registrar informações úteis para manutenção e análise.

---

# 2. Objetivos do Sistema

O módulo deverá:

* Registrar eventos.
* Registrar falhas.
* Registrar avisos.
* Registrar informações de inicialização.
* Registrar desempenho.
* Registrar reinicializações.
* Permitir exportação dos logs.
* Facilitar o diagnóstico remoto.

---

# 3. Níveis de Log

Todos os registros deverão possuir um nível de severidade.

| Nível    | Descrição                                             |
| -------- | ----------------------------------------------------- |
| TRACE    | Informações detalhadas para depuração profunda.       |
| DEBUG    | Informações técnicas de desenvolvimento.              |
| INFO     | Eventos normais de operação.                          |
| WARNING  | Situações que exigem atenção, mas sem risco imediato. |
| ERROR    | Falhas que afetam funcionalidades específicas.        |
| CRITICAL | Falhas que colocam o sistema em estado inseguro.      |

---

# 4. Estrutura do Log

Cada registro deverá conter:

* Data e hora (quando disponível).
* Tempo desde o boot.
* Nível do log.
* Módulo de origem.
* Código do evento.
* Descrição.
* Valor associado (quando aplicável).

Exemplo:

```text
[00:02:15.342]
[WARNING]
[Protection]
OVP
Cell=3
Voltage=4.28V
```

---

# 5. Dashboard Serial

O painel deverá apresentar:

```text
==============================
BMS STATUS
==============================

Estado............. NORMAL
Falha.............. NONE

Pack Voltage....... 11.08 V
Pack Current....... 2.35 A

SOC................ 82 %
SOH................ 98 %

Cell 1............. 3.689 V
Cell 2............. 3.581 V
Cell 3............. 3.804 V

Delta.............. 0.223 V

Temperature........ 25.4 °C

Charge MOSFET...... ON
Discharge MOSFET... ON

Balancing.......... Cell 3

CAN................ OK
BLE................ Connected

Loop............... 10 ms
CPU................ 18 %

Free Heap.......... 243 KB

Uptime............. 00:15:42
```

---

# 6. Fault Codes

Todos os erros deverão possuir códigos padronizados.

Exemplos:

| Código | Descrição                      |
| ------ | ------------------------------ |
| F001   | Sobretensão (OVP)              |
| F002   | Subtensão (UVP)                |
| F003   | Sobrecorrente de carga         |
| F004   | Sobrecorrente de descarga      |
| F005   | Sobretemperatura               |
| F006   | Baixa temperatura              |
| F007   | Curto-circuito                 |
| F008   | Falha de comunicação CAN       |
| F009   | Falha de comunicação BLE       |
| F010   | Sensor de temperatura inválido |
| F011   | Sensor de corrente inválido    |
| F012   | Erro de inicialização          |
| F013   | Watchdog acionado              |
| F014   | Falha de armazenamento         |
| F015   | Erro interno do firmware       |

---

# 7. Event Codes

Eventos normais também deverão possuir identificação.

Exemplos:

| Código | Evento                      |
| ------ | --------------------------- |
| E001   | Boot do sistema             |
| E002   | Inicialização concluída     |
| E003   | Entrada em modo NORMAL      |
| E004   | Início do balanceamento     |
| E005   | Fim do balanceamento        |
| E006   | Atualização de configuração |
| E007   | Reinicialização solicitada  |
| E008   | Conexão BLE estabelecida    |
| E009   | Conexão CAN ativa           |

---

# 8. Métricas

O sistema deverá acompanhar:

* Tempo de execução do loop.
* Uso de memória.
* Uso da CPU.
* Número de ciclos do scheduler.
* Número de mensagens CAN enviadas.
* Número de mensagens BLE enviadas.
* Número de falhas registradas.
* Número de reinicializações.

---

# 9. Histórico de Falhas

O firmware deverá manter um histórico persistente contendo:

* Código da falha.
* Data e hora (quando disponível).
* Tempo desde o boot.
* Valores medidos no momento da falha.
* Estado do sistema.
* Ação executada.

---

# 10. Interface CLI

Comandos obrigatórios:

```text
help
status
faults
events
cells
temps
current
soc
soh
heap
tasks
uptime
version
config
save
reboot
reset
clearfaults
```

---

# 11. Diagnóstico em Tempo Real

O firmware deverá detectar:

* Loop lento.
* Task atrasada.
* Sensor sem resposta.
* Comunicação interrompida.
* Overflow de fila.
* Reinicialização inesperada.
* Perda de memória.
* Tempo excessivo de balanceamento.

---

# 12. Exportação de Logs

Os registros deverão poder ser exportados por:

* Serial.
* BLE (quando autorizado).
* CAN (eventos críticos).

A arquitetura deverá permitir futura exportação para cartão SD ou rede.

---

# 13. Requisitos de Desempenho

* O sistema de logs não deverá bloquear tarefas críticas.
* Eventos deverão ser armazenados em fila.
* Registros críticos terão prioridade sobre mensagens informativas.

---

# 14. Critérios de Aceitação

O módulo de diagnóstico será considerado aprovado quando:

* Registrar corretamente eventos e falhas.
* Exibir o dashboard em tempo real.
* Permitir consulta por CLI.
* Preservar o histórico conforme configurado.
* Não interferir na execução das tarefas críticas.

---

# 15. Evoluções Futuras

A arquitetura deverá permitir:

* Exportação para cartão SD.
* Interface Web.
* Integração com MQTT.
* Dashboard remoto.
* Geração de relatórios automáticos.
* Integração com aplicativos móveis.

---

# 16. Controle de Versão

| Versão | Data       | Descrição                                              |
| ------ | ---------- | ------------------------------------------------------ |
| 1.0.0  | 03/07/2026 | Definição inicial do sistema de diagnóstico e logging. |

