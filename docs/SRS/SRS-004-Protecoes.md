# SRS-004 – Proteções e Segurança

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Em desenvolvimento
**Dependências:** SRS-001, SRS-002 e SRS-003

---

# 1. Objetivo

Este documento define todas as estratégias de proteção, segurança e gerenciamento de falhas do firmware BMS.

O objetivo principal é garantir a operação segura do sistema, protegendo as células, os componentes eletrônicos e o equipamento conectado.

Todas as decisões de segurança deverão ser centralizadas no módulo de proteção e executadas pela máquina de estados.

---

# 2. Princípios de Segurança

O firmware deverá obedecer aos seguintes princípios:

* Segurança acima da disponibilidade.
* Falhas devem resultar em estado seguro (Fail-Safe).
* Nenhum módulo poderá ignorar uma condição crítica.
* Todas as falhas deverão ser registradas.
* Toda recuperação deverá ocorrer de forma controlada.

---

# 3. Proteções Obrigatórias

## Sobretensão por célula (OVP)

Objetivo:

Evitar danos às células por excesso de tensão.

Ações:

* Desabilitar carga.
* Registrar evento.
* Alterar estado para FAULT_OVERVOLTAGE.
* Informar a condição via CAN, BLE, Serial e Display.

---

## Subtensão por célula (UVP)

Objetivo:

Evitar descarga profunda.

Ações:

* Desabilitar descarga.
* Registrar evento.
* Alterar estado para FAULT_UNDERVOLTAGE.

---

## Sobrecorrente (OCP)

Proteção para:

* Carga.
* Descarga.

Ações:

* Desligar MOSFET correspondente.
* Registrar corrente medida.
* Incrementar contador de eventos.

---

## Curto-circuito (SCP)

Objetivo:

Detectar correntes extremamente elevadas.

Ações:

* Desligamento imediato dos MOSFETs.
* Estado EMERGENCY.
* Registro permanente do evento.

---

## Sobretemperatura (OTP)

Monitorar todos os sensores configurados.

Ações:

* Desabilitar carga.
* Desabilitar descarga.
* Desabilitar balanceamento.
* Registrar evento.

---

## Baixa temperatura (UTP)

Objetivo:

Evitar carregamento em temperaturas inadequadas.

Ações:

* Bloquear carga.
* Permitir descarga somente se configurado.

---

# 4. Balanceamento

O balanceamento deverá obedecer às seguintes regras:

* Nunca ocorrer durante estado de falha.
* Nunca ocorrer durante desligamento.
* Ser interrompido em caso de sobretemperatura.
* Ser interrompido em caso de sobretensão.
* Atuar apenas quando o desbalanceamento exceder o limite configurado.
* Controlar apenas uma ou mais células conforme a estratégia adotada.

---

# 5. Estados Operacionais

Estados permitidos:

* BOOT
* INIT
* NORMAL
* CHARGING
* DISCHARGING
* BALANCING
* WARNING
* FAULT
* SHUTDOWN
* EMERGENCY

Cada estado possui permissões específicas para carga, descarga, balanceamento e comunicação.

---

# 6. Prioridade das Falhas

Quando múltiplas falhas ocorrerem simultaneamente, a prioridade será:

1. Curto-circuito (SCP)
2. Sobrecorrente (OCP)
3. Sobretemperatura (OTP)
4. Sobretensão (OVP)
5. Subtensão (UVP)
6. Baixa temperatura (UTP)
7. Demais avisos

A falha de maior prioridade determinará o estado do sistema.

---

# 7. Recuperação

As estratégias de recuperação poderão ser configuráveis.

Modos suportados:

* Recuperação automática.
* Recuperação manual.
* Recuperação após reinicialização.
* Recuperação condicionada ao retorno dos parâmetros para a faixa segura.

Toda recuperação deverá ser registrada no histórico.

---

# 8. Fault Manager

Responsabilidades:

* Detectar falhas.
* Classificar severidade.
* Armazenar histórico.
* Notificar outros módulos.
* Solicitar mudança de estado.
* Disponibilizar informações para diagnóstico.

---

# 9. Event Manager

Responsável por registrar:

* Inicialização.
* Reinicialização.
* Entrada em falha.
* Saída de falha.
* Início e fim do balanceamento.
* Mudança de modo de operação.
* Alteração de configuração.

Cada evento deverá conter:

* Data/hora (quando disponível).
* Tipo.
* Descrição.
* Valor associado.

---

# 10. Watchdog

O sistema deverá utilizar watchdog para detectar:

* Travamento de tarefas.
* Falhas de comunicação entre módulos.
* Loop principal bloqueado.

Ao reiniciar, o motivo deverá ser registrado.

---

# 11. Diagnóstico

O firmware deverá fornecer um painel contendo:

* Estado atual.
* Código da falha.
* Tensão do pack.
* Corrente.
* Temperatura.
* SOC.
* SOH.
* Estado dos MOSFETs.
* Estado do balanceamento.
* Status do CAN.
* Status do BLE.
* Tempo de execução do loop.

---

# 12. Registro Permanente

Sempre que possível, deverão ser armazenados:

* Contador de ciclos.
* Número de falhas.
* Última falha.
* Maior temperatura registrada.
* Menor tensão registrada.
* Maior corrente registrada.
* Tempo total de operação.

---

# 13. Configuração

Todos os limites deverão ser configuráveis em um único módulo.

Exemplos:

* Tensão máxima por célula.
* Tensão mínima por célula.
* Corrente máxima de carga.
* Corrente máxima de descarga.
* Temperatura máxima.
* Temperatura mínima.
* Diferença para balanceamento.
* Tempo máximo de balanceamento.

---

# 14. Critérios de Aceitação

O sistema será considerado aprovado quando:

* Todas as proteções forem detectadas corretamente.
* Os MOSFETs responderem conforme especificado.
* Os eventos forem registrados.
* O histórico de falhas estiver consistente.
* O sistema entrar em estado seguro em condições críticas.
* O comportamento for reproduzível em Mock, Wokwi e hardware real.

---

# 15. Evoluções Futuras

A arquitetura deverá permitir a inclusão de:

* Diagnóstico remoto.
* Atualização OTA.
* Integração com sistemas supervisórios.
* Algoritmos avançados de estimativa de SOC/SOH.
* Estratégias adaptativas de balanceamento.

---

# 16. Controle de Versão

| Versão | Data       | Descrição                                                  |
| ------ | ---------- | ---------------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Definição inicial das estratégias de proteção e segurança. |

