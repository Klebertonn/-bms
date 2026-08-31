# SRS-005 – Comunicação

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Em desenvolvimento
**Dependências:** SRS-001, SRS-002, SRS-003 e SRS-004

---

# 1. Objetivo

Este documento define todas as interfaces de comunicação do firmware BMS.

Os objetivos são:

* Disponibilizar monitoramento em tempo real.
* Permitir integração com outros sistemas.
* Facilitar diagnóstico e manutenção.
* Oferecer configuração segura dos parâmetros do BMS.
* Garantir uma arquitetura de comunicação modular e expansível.

---

# 2. Arquitetura de Comunicação

Todos os módulos de comunicação deverão receber informações apenas dos serviços internos do firmware.

```text
Sensores
      │
      ▼
Serviços Internos (SOC, SOH, Proteções, Balanceamento)
      │
      ▼
Communication Manager
      ├── CAN
      ├── BLE
      ├── Serial / CLI
      └── Display OLED
```

Nenhum módulo de comunicação poderá acessar diretamente sensores ou GPIO.

---

# 3. Interface CAN (TWAI)

## Objetivos

* Envio periódico de telemetria.
* Divulgação de alarmes.
* Integração com sistemas externos.
* Diagnóstico.

## Configurações

* Velocidade configurável (125 kbps, 250 kbps, 500 kbps e 1 Mbps).
* Identificador padrão de mensagens.
* Filtros configuráveis.

## Dados transmitidos

* Tensão total do pack.
* Corrente.
* SOC.
* SOH.
* Estado do BMS.
* Temperaturas.
* Tensões das células.
* Estado dos MOSFETs.
* Estado do balanceamento.
* Falhas ativas.

---

# 4. Bluetooth Low Energy (BLE)

## Objetivos

* Monitoramento em aplicativo móvel.
* Configuração remota.
* Diagnóstico.

## Serviços

### Informações Gerais

* Nome do dispositivo.
* Versão do firmware.
* Número de série.

### Telemetria

* Pack Voltage
* Pack Current
* SOC
* SOH
* Temperatura
* Estado do sistema

### Configuração

Permitir alteração de parâmetros autorizados, como:

* Limites de tensão.
* Limites de corrente.
* Limites de temperatura.
* Intervalo de atualização.

Toda alteração deverá exigir autenticação.

---

# 5. Comunicação Serial

A interface Serial será utilizada para:

* Desenvolvimento.
* Testes.
* Diagnóstico.
* Configuração local.

Velocidade padrão:

115200 bps

---

# 6. CLI (Command Line Interface)

Comandos mínimos:

```text
help
status
cells
temps
current
soc
soh
faults
events
config
save
reboot
reset
version
```

Cada comando deverá retornar mensagens claras e padronizadas.

---

# 7. Dashboard Serial

O firmware deverá apresentar um painel semelhante ao seguinte:

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
```

O formato deverá permanecer consistente para facilitar leitura por operadores e ferramentas automatizadas.

---

# 8. Sistema de Logs

Os logs deverão possuir níveis de severidade:

* TRACE
* DEBUG
* INFO
* WARNING
* ERROR
* CRITICAL

Formato recomendado:

```text
[2026-07-03 15:10:24]
[WARNING]
OVP detectada
Cell: 3
Voltage: 4.28 V
```

Todos os eventos críticos deverão ser registrados.

---

# 9. Display OLED

O display deverá apresentar, no mínimo:

* Estado do sistema.
* SOC.
* SOH.
* Tensão total.
* Corrente.
* Temperatura.
* Alarmes.
* Status do balanceamento.

As telas poderão alternar automaticamente ou por comando.

---

# 10. Gerenciador de Comunicação

Um módulo central deverá coordenar todas as interfaces.

Responsabilidades:

* Agendar transmissões.
* Evitar bloqueios.
* Controlar filas de mensagens.
* Garantir consistência dos dados.

---

# 11. Segurança

Configurações críticas deverão exigir autorização.

Exemplos:

* Alteração de limites de proteção.
* Reinicialização do BMS.
* Limpeza do histórico de falhas.
* Atualização de firmware.

Todas as alterações deverão ser registradas.

---

# 12. Requisitos de Desempenho

* Comunicação não deve bloquear o loop principal.
* Uso de filas quando necessário.
* Atualizações periódicas configuráveis.
* Prioridade para mensagens de falha.

---

# 13. Critérios de Aceitação

O módulo de comunicação será considerado aprovado quando:

* Todas as interfaces funcionarem de forma independente.
* Os dados transmitidos forem consistentes.
* O sistema continuar responsivo durante transmissões.
* Logs e mensagens forem claros e padronizados.
* Alterações de configuração forem seguras e rastreáveis.

---

# 14. Evoluções Futuras

A arquitetura deverá permitir:

* Wi-Fi.
* MQTT.
* Modbus.
* Interface Web.
* Aplicativo móvel completo.
* Atualização OTA.
* Exportação de logs.
* Integração com sistemas supervisórios.

---

# 15. Controle de Versão

| Versão | Data       | Descrição                                               |
| ------ | ---------- | ------------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Definição inicial da arquitetura de comunicação do BMS. |

