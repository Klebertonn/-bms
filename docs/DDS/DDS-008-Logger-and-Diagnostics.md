# DDS-008 – Logger & Diagnostics

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001 até DDS-007

---

# 1. Objetivo

O Logger & Diagnostics é responsável por registrar eventos, falhas, medições e estatísticas do sistema.

Além do registro interno, o módulo deverá fornecer informações claras para facilitar testes, manutenção e diagnóstico.

---

# 2. Objetivos

* Facilitar depuração.
* Auxiliar manutenção.
* Registrar histórico.
* Permitir rastreabilidade.
* Reduzir tempo de diagnóstico.

---

# 3. Níveis de Log

Serão suportados os seguintes níveis:

DEBUG

Informações detalhadas.

INFO

Operação normal.

WARNING

Condição que merece atenção.

ERROR

Erro recuperável.

CRITICAL

Erro crítico com atuação do sistema.

FATAL

Falha que exige desligamento seguro.

---

# 4. Estrutura de uma Mensagem

Cada mensagem deverá conter:

* Timestamp.
* Nível.
* Módulo.
* Evento.
* Valor medido.
* Limite configurado.
* Ação executada.

Exemplo:

```text
[125034 ms]
[ERROR]
[Protection]
OVP

Cell 4 = 4.28 V

Limite = 4.25 V

Charge MOSFET = OFF
```

---

# 5. Dashboard Principal

A tela padrão deverá apresentar:

```text
==================================

BMS STATUS

==================================

Estado............. NORMAL

Falha.............. NONE

Pack Voltage....... 14.62 V

Pack Current....... 5.24 A

SOC................ 83 %

SOH................ 98 %

SOE................ 81 %

----------------------------------

Cell 1............. 3.654 V

Cell 2............. 3.657 V

Cell 3............. 3.653 V

Cell 4............. 3.658 V

Delta.............. 0.005 V

----------------------------------

Temp 1............. 28.4 °C

Temp 2............. 29.1 °C

----------------------------------

Charge MOSFET...... ON

Discharge MOSFET... ON

Balancing.......... OFF

----------------------------------

CAN................ OK

BLE................ Connected

OLED............... OK

----------------------------------

Loop............... 9 ms

Heap............... 262 KB

Stack............... OK

==================================
```

---

# 6. Registro de Eventos

Eventos registrados:

* Inicialização.
* Reinicialização.
* Mudança de estado.
* Ativação de proteção.
* Balanceamento.
* Alteração de configuração.
* Atualização OTA (futuro).
* Erros de comunicação.

---

# 7. Histórico de Falhas

Cada falha deverá armazenar:

* Código.
* Tipo.
* Data/Hora.
* Valor medido.
* Limite.
* Módulo.
* Ação executada.
* Estado final.

---

# 8. Estatísticas

O módulo deverá acompanhar:

* Tempo de execução.
* Uso de CPU.
* Heap disponível.
* Stack das tarefas.
* Número de falhas.
* Número de reinicializações.
* Tempo de operação.
* Número de ciclos de carga.

---

# 9. Exportação

O Logger deverá permitir exportação em:

* Texto.
* CSV.
* JSON.

Versões futuras:

* MQTT.
* REST API.
* Banco de dados.

---

# 10. Integração

Receberá eventos de:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Communication Manager.
* Task Manager.
* HAL.
* Drivers.

---

# 11. Persistência

Quando configurado, deverá armazenar:

* Últimas falhas.
* Histórico de alarmes.
* Estatísticas.
* Configurações.

---

# 12. Configuração

Parâmetros configuráveis:

* Nível mínimo de log.
* Intervalo do dashboard.
* Número máximo de registros.
* Persistência habilitada.
* Exportação automática.

---

# 13. Diagnóstico

O módulo deverá disponibilizar comandos para:

* Ver estado atual.
* Listar falhas.
* Limpar histórico.
* Mostrar estatísticas.
* Testar sensores.
* Testar comunicação.
* Testar MOSFETs.
* Testar balanceamento.

---

# 14. Requisitos

* Não bloquear tarefas críticas.
* Compatível com FreeRTOS.
* Baixo consumo de memória.
* Tempo determinístico.

---

# 15. Critérios de Aceitação

O módulo será aprovado quando:

* Todos os eventos forem registrados corretamente.
* O dashboard refletir o estado atual do sistema.
* O histórico puder ser consultado.
* Os testes forem aprovados em Mock, Wokwi e ESP32.

---

# 16. Evoluções Futuras

* Dashboard Web.
* Interface gráfica para PC.
* Integração com Grafana.
* Exportação para banco de dados.
* Diagnóstico remoto.

---

# 17. Controle de Versão

| Versão | Data       | Descrição                              |
| ------ | ---------- | -------------------------------------- |
| 1.0.0  | 03/07/2026 | Especificação do Logger & Diagnostics. |

