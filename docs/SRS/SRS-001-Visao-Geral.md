# SRS-001 — Visão Geral do Projeto

**Projeto:** BMS Firmware
**Versão do Documento:** 1.0.0
**Status:** Em desenvolvimento
**Última atualização:** 03/07/2026

---

# 1. Objetivo

Este documento define a visão geral do projeto **BMS Firmware**, estabelecendo os objetivos, escopo, requisitos e diretrizes que serão utilizados durante todo o desenvolvimento.

O objetivo é criar um firmware profissional para gerenciamento de baterias (Battery Management System - BMS) baseado em ESP32, desenvolvido em C++ utilizando PlatformIO, com arquitetura modular, alta confiabilidade, facilidade de manutenção e suporte tanto para simulação quanto para hardware real.

O firmware deverá atender aplicações de pesquisa, prototipagem avançada e sistemas embarcados, permitindo futura expansão para produtos comerciais.

---

# 2. Escopo

O firmware será responsável por:

* Monitorar tensão individual de cada célula.
* Monitorar tensão total do pack.
* Monitorar corrente de carga e descarga.
* Monitorar temperatura do sistema.
* Calcular SOC (State of Charge).
* Estimar SOH (State of Health).
* Controlar balanceamento de células.
* Controlar MOSFETs de carga e descarga.
* Detectar falhas.
* Registrar eventos.
* Comunicar-se através de CAN (TWAI).
* Comunicar-se via Bluetooth Low Energy (BLE).
* Exibir informações em display OLED.
* Disponibilizar diagnóstico via Serial.
* Executar em ESP32 utilizando FreeRTOS.
* Operar em ambiente de simulação (Mock e Wokwi) e hardware real.

---

# 3. Objetivos do Projeto

## Objetivo Geral

Desenvolver um firmware modular, seguro e escalável para gerenciamento inteligente de baterias.

## Objetivos Específicos

* Alta confiabilidade.
* Código organizado.
* Separação completa entre lógica de negócio e hardware.
* Facilidade para manutenção.
* Fácil expansão para novas funcionalidades.
* Diagnóstico rápido.
* Registro detalhado de eventos.
* Compatibilidade com futuras versões do hardware.

---

# 4. Aplicações

O firmware poderá ser utilizado em:

* Packs de baterias de lítio.
* Sistemas de armazenamento de energia.
* UPS.
* Energia solar.
* Veículos elétricos de pequeno porte.
* Robótica.
* Pesquisa acadêmica.
* Bancadas de testes.
* Sistemas embarcados.

---

# 5. Requisitos Funcionais

O firmware deverá permitir:

## RF-001

Monitorar tensão individual de todas as células.

## RF-002

Monitorar tensão total do pack.

## RF-003

Monitorar corrente.

## RF-004

Monitorar temperatura.

## RF-005

Executar balanceamento automático.

## RF-006

Executar proteção contra sobretensão (OVP).

## RF-007

Executar proteção contra subtensão (UVP).

## RF-008

Executar proteção contra sobrecorrente (OCP).

## RF-009

Executar proteção contra sobretemperatura (OTP).

## RF-010

Executar proteção contra baixa temperatura (UTP), quando aplicável.

## RF-011

Calcular SOC utilizando Coulomb Counting com correção por tensão.

## RF-012

Estimar SOH.

## RF-013

Registrar eventos.

## RF-014

Registrar falhas.

## RF-015

Enviar dados via CAN.

## RF-016

Disponibilizar dados via BLE.

## RF-017

Exibir dados em display OLED.

## RF-018

Disponibilizar dashboard via Serial.

## RF-019

Permitir configuração centralizada de parâmetros.

## RF-020

Persistir configurações em NVS.

---

# 6. Requisitos Não Funcionais

O projeto deverá atender aos seguintes requisitos:

* Linguagem C++17.
* Framework Arduino para ESP32.
* PlatformIO como ambiente de compilação.
* Arquitetura modular.
* Código documentado.
* Fácil manutenção.
* Separação entre HAL e lógica do BMS.
* Compilação sem erros.
* Organização por módulos.
* Compatibilidade com Mock, Wokwi e hardware real.

---

# 7. Arquitetura Geral

O firmware será organizado em camadas.

```text
Application
│
├── Core
├── Services
├── Diagnostics
├── Communication
├── HAL
└── Drivers
```

A lógica de negócio nunca deverá acessar diretamente o hardware.

Toda comunicação com sensores e periféricos deverá ocorrer através da HAL.

---

# 8. Comunicação

O firmware deverá suportar:

* Serial
* BLE
* CAN (TWAI)
* Display OLED

---

# 9. Segurança

O firmware deverá possuir:

* OVP
* UVP
* OCP
* OTP
* UTP
* Balanceamento automático
* Watchdog
* Fault Manager
* Event Manager

---

# 10. Modos de Operação

O firmware deverá possuir três modos de execução:

## Mock

Simulação em computador.

## Wokwi

Simulação em ESP32 virtual.

## Hardware Real

Execução utilizando sensores e periféricos físicos.

A seleção do modo deverá ocorrer por configuração, sem alterar a lógica principal do firmware.

---

# 11. Critérios de Aceitação

O firmware será considerado apto quando:

* Compilar no PlatformIO sem erros.
* Executar corretamente em modo Mock.
* Executar corretamente no Wokwi.
* Executar corretamente em hardware real.
* Detectar todas as proteções configuradas.
* Registrar eventos e falhas.
* Operar continuamente sem travamentos.
* Manter a arquitetura modular.
* Possuir documentação atualizada.

---

# 12. Objetivos de Qualidade

O projeto deverá priorizar:

* Segurança.
* Robustez.
* Clareza do código.
* Facilidade de manutenção.
* Escalabilidade.
* Reutilização de componentes.
* Testabilidade.
* Desempenho determinístico.

---

# 13. Roadmap

## Fase 1

Especificação técnica.

## Fase 2

Arquitetura do firmware.

## Fase 3

Implementação dos módulos.

## Fase 4

Integração.

## Fase 5

Testes em Mock.

## Fase 6

Testes no Wokwi.

## Fase 7

Testes em ESP32 real.

## Fase 8

Validação com hardware completo.

## Fase 9

Versão candidata à produção.

---

# 14. Documentos Relacionados

* SRS-002 – Arquitetura
* SRS-003 – Hardware
* SRS-004 – Proteções
* SRS-005 – Comunicação
* SRS-006 – FreeRTOS
* SRS-007 – Diagnóstico
* SRS-008 – Testes
* SRS-009 – Produção
* SRS-010 – Roadmap

---

# 15. Controle de Versão

| Versão | Data       | Descrição                     |
| ------ | ---------- | ----------------------------- |
| 1.0.0  | 03/07/2026 | Criação inicial do documento. |


