# DDS-005 – Protection Manager

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001, DDS-002, DDS-003 e DDS-004

---

# 1. Objetivo

O Protection Manager é responsável por monitorar continuamente as condições elétricas e térmicas do pack de baterias, detectando situações que possam comprometer a segurança, a integridade da bateria ou dos equipamentos conectados.

Ao identificar uma condição anormal, o módulo deverá aplicar a estratégia de proteção correspondente, registrar o evento e informar os demais módulos do sistema.

---

# 2. Objetivos da Proteção

* Garantir a segurança da bateria.
* Proteger os MOSFETs de carga e descarga.
* Evitar danos permanentes às células.
* Preservar a vida útil do pack.
* Garantir operação segura em todas as condições.

---

# 3. Proteções Implementadas

## OVP – Over Voltage Protection

Monitora a tensão máxima das células.

Ações:

* Desabilitar MOSFET de carga.
* Registrar evento.
* Notificar CAN, BLE e Logger.

---

## UVP – Under Voltage Protection

Monitora a tensão mínima das células.

Ações:

* Desabilitar MOSFET de descarga.
* Registrar evento.
* Atualizar estado do sistema.

---

## OCP – Over Current Protection

Monitora corrente excessiva.

Ações:

* Interromper carga ou descarga.
* Registrar ocorrência.
* Entrar em estado de falha.

---

## SCP – Short Circuit Protection

Detecta curto-circuito através da corrente e da queda rápida de tensão.

Ações:

* Desligamento imediato dos MOSFETs.
* Registro crítico da ocorrência.
* Exigir reinicialização ou procedimento de recuperação configurável.

---

## OTP – Over Temperature Protection

Monitora temperatura acima do limite.

Ações:

* Desabilitar carga.
* Desabilitar descarga quando necessário.
* Registrar evento.

---

## UTP – Under Temperature Protection

Monitora temperatura abaixo do limite permitido.

Ações:

* Bloquear carga quando aplicável.
* Informar o estado aos demais módulos.

---

# 4. Estados de Falha

Cada falha poderá assumir os seguintes estados:

* Detectada.
* Confirmada.
* Ativa.
* Recuperada.
* Histórica.

Esses estados serão gerenciados pelo Fault Manager.

---

# 5. Prioridade das Falhas

Ordem de prioridade:

1. Curto-circuito (SCP)
2. Sobrecorrente (OCP)
3. Sobretensão (OVP)
4. Subtensão (UVP)
5. Sobretemperatura (OTP)
6. Subtemperatura (UTP)

Quando múltiplas falhas ocorrerem simultaneamente, prevalecerá a de maior prioridade.

---

# 6. Histerese

Todas as proteções deverão utilizar histerese configurável para evitar oscilações entre estados.

Exemplo:

* OVP ativa em 4,25 V.
* OVP libera somente abaixo de 4,15 V.

---

# 7. Temporização

Cada proteção poderá utilizar tempos independentes de confirmação.

Exemplos:

* SCP: atuação imediata.
* OCP: alguns milissegundos.
* OVP: confirmação configurável.
* OTP: confirmação configurável.

Os tempos serão definidos nos arquivos de configuração.

---

# 8. Máquina de Estados

```text id="prot001"
NORMAL
   │
   ▼
ALARME
   │
   ▼
FAULT
   │
   ├── Recuperação automática (quando permitido)
   └── Recuperação manual (quando configurado)
```

---

# 9. Integração

O Protection Manager utilizará informações do Battery Manager e comunicará seus estados para:

* Balance Manager.
* Logger.
* Communication Manager.
* Display.
* CLI.
* BLE.
* CAN.

---

# 10. Registro de Eventos

Cada falha deverá armazenar:

* Tipo.
* Data e hora (quando disponível).
* Valor medido.
* Limite configurado.
* Ação executada.
* Estado final.

Esses registros poderão ser persistidos na memória não volátil.

---

# 11. Configuração

Todos os limites deverão ser configuráveis por perfil.

Exemplos:

* Tensão máxima.
* Tensão mínima.
* Corrente máxima.
* Temperatura máxima.
* Temperatura mínima.
* Histerese.
* Tempo de confirmação.
* Estratégia de recuperação.

---

# 12. Requisitos de Desempenho

* Execução determinística.
* Prioridade elevada.
* Baixa latência.
* Compatibilidade com FreeRTOS.
* Operação contínua.

---

# 13. Critérios de Aceitação

O Protection Manager será considerado aprovado quando:

* Todas as proteções forem detectadas corretamente.
* As ações de proteção forem executadas conforme especificado.
* Os eventos forem registrados.
* Os estados forem propagados aos demais módulos.
* O funcionamento for validado em Mock, Wokwi e ESP32 físico.

---

# 14. Evoluções Futuras

O projeto deverá permitir a inclusão de:

* Proteção por potência.
* Proteção por energia.
* Diagnóstico avançado de falhas.
* Algoritmos preditivos.
* Redundância de sensores.

---

# 15. Controle de Versão

| Versão | Data       | Descrição                                    |
| ------ | ---------- | -------------------------------------------- |
| 1.0.0  | 03/07/2026 | Especificação inicial do Protection Manager. |

