# DDS-006 – Balance Manager

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001, DDS-002, DDS-003, DDS-004 e DDS-005

---

# 1. Objetivo

O Balance Manager é responsável por reduzir as diferenças de tensão entre as células do pack, aumentando a eficiência, a capacidade utilizável e a vida útil da bateria.

O módulo deverá operar de forma segura, coordenando o acionamento dos MOSFETs de balanceamento conforme as condições definidas pelo sistema.

---

# 2. Objetivos do Balanceamento

* Reduzir o desbalanceamento entre células.
* Evitar sobrecarga de células individuais.
* Melhorar a capacidade efetiva do pack.
* Aumentar a vida útil das células.
* Manter a operação segura.

---

# 3. Tipos de Balanceamento

## Balanceamento Passivo

Características:

* Dissipação da energia excedente em resistores.
* Controle por MOSFET.
* Implementação inicial do projeto.

---

## Balanceamento Ativo (Evolução)

Previsto para versões futuras.

Características:

* Transferência de energia entre células.
* Maior eficiência energética.
* Menor aquecimento.

---

# 4. Critérios para Início

O balanceamento poderá iniciar quando todos os critérios abaixo forem atendidos:

* Sistema em estado NORMAL.
* Proteções inativas.
* Diferença entre células acima do limite configurado.
* Temperatura dentro da faixa permitida.
* Tensão mínima das células acima do valor configurado.
* Modo de operação permitir balanceamento (carga, repouso ou ambos).

---

# 5. Critérios para Encerramento

O balanceamento deverá ser interrompido quando ocorrer qualquer uma das situações:

* Delta de tensão abaixo do limite.
* Falha detectada pelo Protection Manager.
* Temperatura acima do limite.
* Tempo máximo de balanceamento excedido.
* Solicitação do sistema.

---

# 6. Estratégias de Balanceamento

O firmware deverá permitir selecionar a estratégia por configuração.

### Estratégia Sequencial

Balanceia uma célula por vez.

Vantagens:

* Simplicidade.
* Menor aquecimento.

---

### Round Robin

Balanceia diferentes células em sequência, distribuindo o tempo de acionamento.

Vantagens:

* Distribuição uniforme da carga térmica.
* Maior eficiência.

---

### Balanceamento Simultâneo

Permite balancear múltiplas células quando suportado pelo hardware.

Deverá respeitar:

* Limite de corrente.
* Limite térmico.
* Quantidade máxima configurável de células simultâneas.

---

# 7. Controle dos MOSFETs

Cada canal de balanceamento deverá possuir:

* Estado (ON/OFF).
* Tempo acumulado de acionamento.
* Número de ativações.
* Temperatura associada (quando disponível).

---

# 8. Monitoramento Térmico

Durante o balanceamento deverão ser monitorados:

* Temperatura do pack.
* Temperatura dos MOSFETs (quando houver sensor).
* Tempo contínuo de operação.

Ao exceder os limites configurados, o balanceamento deverá ser interrompido.

---

# 9. Fluxo de Operação

```text id="bal001"
Leitura das células
        │
        ▼
Cálculo do delta
        │
        ▼
Verificação das condições
        │
        ▼
Seleção das células
        │
        ▼
Acionamento dos MOSFETs
        │
        ▼
Monitoramento contínuo
        │
        ▼
Encerramento do ciclo
```

---

# 10. Configuração

Parâmetros configuráveis:

* Delta mínimo para iniciar.
* Delta para encerrar.
* Tempo máximo de balanceamento.
* Número máximo de células simultâneas.
* Estratégia utilizada.
* Limites de temperatura.
* Intervalo de verificação.

Todos os parâmetros deverão ser armazenados de forma persistente.

---

# 11. Registro de Eventos

Cada ciclo de balanceamento deverá registrar:

* Data e hora (quando disponível).
* Células balanceadas.
* Tempo de acionamento.
* Delta inicial.
* Delta final.
* Resultado do ciclo.

Esses registros poderão ser utilizados para diagnóstico e manutenção.

---

# 12. Integração

O Balance Manager utilizará informações do Battery Manager e do Protection Manager.

Também disponibilizará informações para:

* Logger.
* Display.
* Communication Manager.
* CLI.
* BLE.
* CAN.

---

# 13. Requisitos de Desempenho

* Operação determinística.
* Baixo consumo de CPU.
* Compatibilidade com FreeRTOS.
* Baixo impacto sobre a medição das células.

---

# 14. Critérios de Aceitação

O Balance Manager será considerado aprovado quando:

* As células forem balanceadas conforme os critérios definidos.
* Nenhum limite de segurança for violado.
* Todos os eventos forem registrados.
* A operação for validada em Mock, Wokwi e ESP32 físico.

---

# 15. Evoluções Futuras

O projeto deverá permitir:

* Balanceamento ativo.
* Estratégias adaptativas.
* Controle por PWM.
* Balanceamento baseado em SOH.
* Algoritmos otimizados para diferentes químicas de bateria.

---

# 16. Controle de Versão

| Versão | Data       | Descrição                                 |
| ------ | ---------- | ----------------------------------------- |
| 1.0.0  | 03/07/2026 | Especificação inicial do Balance Manager. |

