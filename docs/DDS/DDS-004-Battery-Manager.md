# DDS-004 – Battery Manager

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001, DDS-002 e DDS-003

---

# 1. Objetivo

O Battery Manager é responsável por adquirir, processar e disponibilizar todas as informações relacionadas ao estado da bateria.

Ele concentra os cálculos de tensão, corrente, potência, energia, State of Charge (SOC), State of Health (SOH), State of Energy (SOE) e demais parâmetros utilizados pelos módulos do sistema.

---

# 2. Responsabilidades

O Battery Manager deverá:

* Ler os valores dos sensores através da HAL.
* Validar as medições recebidas.
* Filtrar ruídos.
* Calcular parâmetros elétricos.
* Atualizar o estado global da bateria.
* Disponibilizar os dados aos demais módulos.

---

# 3. Dados Monitorados

## Tensão

* Tensão individual de cada célula.
* Tensão mínima.
* Tensão máxima.
* Tensão média.
* Tensão total do pack.
* Diferença entre células (Delta).

---

## Corrente

* Corrente instantânea.
* Corrente média.
* Corrente máxima.
* Corrente mínima.

---

## Temperatura

* Temperatura de cada sensor.
* Temperatura média.
* Temperatura máxima.
* Temperatura mínima.

---

## Energia

* Potência instantânea.
* Energia consumida.
* Energia carregada.

---

# 4. State of Charge (SOC)

O cálculo do SOC utilizará uma combinação de métodos:

* Contagem de Coulombs (Coulomb Counting).
* Correção por tensão em repouso (OCV – Open Circuit Voltage).
* Ajuste periódico para compensação de deriva.

O firmware deverá permitir a substituição futura por algoritmos mais avançados, como filtro de Kalman.

---

# 5. State of Health (SOH)

O SOH deverá estimar a degradação da bateria com base em:

* Capacidade efetiva.
* Resistência interna (quando disponível).
* Número de ciclos.
* Histórico de operação.

---

# 6. State of Energy (SOE)

O SOE representa a energia disponível no pack considerando tensão e carga restante.

Será utilizado para estimativas de autonomia e gerenciamento energético.

---

# 7. Filtragem

As medições deverão passar por filtros para reduzir ruídos.

Filtros previstos:

* Média móvel.
* Média exponencial.
* Janela deslizante.

A escolha do filtro deverá ser configurável.

---

# 8. Estrutura de Dados

O Battery Manager deverá manter uma estrutura única contendo, entre outros:

* Tensões das células.
* Correntes.
* Temperaturas.
* SOC.
* SOH.
* SOE.
* Potência.
* Energia acumulada.
* Capacidade restante.
* Número de ciclos.

Essa estrutura será utilizada pelos demais módulos apenas para leitura.

---

# 9. Fluxo de Processamento

```text id="bat001"
HAL
 ↓
Drivers
 ↓
Leitura dos sensores
 ↓
Validação
 ↓
Filtragem
 ↓
Cálculos
 ↓
Atualização do estado da bateria
 ↓
Disponibilização para os demais módulos
```

---

# 10. Atualização

A taxa de atualização deverá ser configurável.

Exemplo:

* Sensores críticos: 10 ms.
* Cálculo de SOC: 100 ms.
* SOH: 1 s.
* Estatísticas: 5 s.

---

# 11. Tratamento de Erros

O Battery Manager deverá detectar:

* Valores fora da faixa.
* Leituras inconsistentes.
* Falhas de sensores.
* Dados indisponíveis.

As falhas deverão ser registradas e comunicadas ao Fault Manager.

---

# 12. Integração

Os dados produzidos pelo Battery Manager serão consumidos por:

* Protection Manager.
* Balance Manager.
* Logger.
* Communication Manager.
* Display.
* CLI.
* BLE.
* CAN.

---

# 13. Requisitos de Desempenho

* Atualização determinística.
* Baixo consumo de memória.
* Baixa latência.
* Compatibilidade com FreeRTOS.

---

# 14. Critérios de Aceitação

O Battery Manager será considerado aprovado quando:

* Todos os parâmetros forem atualizados corretamente.
* Os cálculos forem consistentes.
* Os módulos consumidores utilizarem apenas sua interface pública.
* O funcionamento for validado em Mock, Wokwi e ESP32 físico.

---

# 15. Evoluções Futuras

O projeto deverá permitir a inclusão de:

* Estimativa avançada de SOC (Filtro de Kalman).
* Diagnóstico de resistência interna.
* Previsão de autonomia.
* Detecção de envelhecimento acelerado.
* Suporte a diferentes químicas de bateria.

---

# 16. Controle de Versão

| Versão | Data       | Descrição                                 |
| ------ | ---------- | ----------------------------------------- |
| 1.0.0  | 03/07/2026 | Especificação inicial do Battery Manager. |

