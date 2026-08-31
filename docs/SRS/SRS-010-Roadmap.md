# SRS-010 – Roadmap e Evolução do Projeto

**Projeto:** BMS Firmware ESP32
**Versão:** 1.0.0
**Status:** Planejamento Estratégico
**Dependências:** SRS-001 a SRS-009

---

# 1. Objetivo

Este documento define o plano de evolução do firmware do BMS, estabelecendo as funcionalidades previstas para cada versão, prioridades de desenvolvimento e critérios para liberação de novas versões.

O objetivo é garantir uma evolução organizada, mantendo compatibilidade, rastreabilidade e qualidade.

---

# 2. Visão Geral

O projeto será desenvolvido de forma incremental, passando pelas seguintes etapas:

```text
Protótipo
    │
    ▼
Simulação PC (Mock)
    │
    ▼
Simulação Wokwi
    │
    ▼
ESP32 físico
    │
    ▼
BMS em bancada
    │
    ▼
Validação elétrica
    │
    ▼
Versão estável
    │
    ▼
Produção
```

---

# 3. Roadmap de Versões

## Versão 0.1.0 – Estrutura Inicial

Objetivos:

* Estrutura do projeto.
* Organização das pastas.
* Configuração do PlatformIO.
* Arquitetura modular.
* Compilação inicial.

**Status:** Concluído.

---

## Versão 0.2.0 – Simulação Mock

Objetivos:

* HAL Mock.
* Sensores simulados.
* Dashboard Serial.
* Logs básicos.
* Máquina de estados.

**Status:** Concluído.

---

## Versão 0.3.0 – Simulação Wokwi

Objetivos:

* Execução no ESP32 virtual.
* Testes de inicialização.
* Simulação de entradas.
* Validação da arquitetura.

**Status:** Em andamento.

---

## Versão 0.4.0 – Hardware ESP32

Objetivos:

* Execução em ESP32 físico.
* Comunicação Serial.
* Monitoramento de memória.
* Watchdog.
* Ajustes de estabilidade.

---

## Versão 0.5.0 – Integração de Sensores

Objetivos:

* Leitura de tensão das células.
* Leitura da tensão do pack.
* Sensor de corrente.
* Sensores de temperatura.
* Calibração.

---

## Versão 0.6.0 – Proteções

Objetivos:

* OVP.
* UVP.
* OCP.
* OTP.
* UTP.
* Curto-circuito.
* Estado seguro.

---

## Versão 0.7.0 – Balanceamento

Objetivos:

* Balanceamento passivo.
* Controle de MOSFETs.
* Limites configuráveis.
* Temporização.
* Registro de eventos.

---

## Versão 0.8.0 – Comunicação

Objetivos:

* CAN (TWAI).
* BLE.
* Dashboard Serial.
* CLI.
* OLED.

---

## Versão 0.9.0 – Persistência

Objetivos:

* NVS.
* Histórico de falhas.
* Configurações.
* Contador de ciclos.
* Estado de Saúde (SOH).

---

## Versão 1.0.0 – Primeira Versão Estável

Objetivos:

* Todas as funcionalidades principais integradas.
* Testes concluídos.
* Documentação completa.
* Firmware pronto para bancada.

---

# 4. Evolução Pós-1.0

## Versão 1.1

* Atualização OTA.
* Melhorias no BLE.
* Logs avançados.
* Novos comandos CLI.

---

## Versão 1.2

* Aplicativo Android.
* Dashboard Web.
* Exportação de dados.

---

## Versão 1.5

* Balanceamento ativo (opcional).
* Diagnóstico remoto.
* Atualização segura.

---

## Versão 2.0

Objetivos:

* Arquitetura industrial.
* Multi-pack.
* Múltiplos perfis de bateria.
* Comunicação distribuída.
* Alta disponibilidade.

---

# 5. Critérios para Liberação

Cada versão deverá atender aos seguintes requisitos:

* Compilação sem erros.
* Testes unitários aprovados.
* Testes de integração aprovados.
* Documentação atualizada.
* Código revisado.
* Histórico de alterações atualizado.

---

# 6. Controle de Mudanças

Todas as alterações deverão ser registradas com:

* Número da versão.
* Data.
* Autor.
* Descrição da mudança.
* Impacto esperado.

---

# 7. Gestão de Riscos

Riscos identificados:

* Falhas de leitura dos sensores.
* Configuração incorreta de parâmetros.
* Perda de comunicação.
* Sobrecarga de processamento.
* Erros de calibração.
* Falhas de alimentação.

Cada risco deverá possuir plano de mitigação documentado.

---

# 8. Indicadores de Qualidade

O projeto acompanhará, entre outros:

* Cobertura de testes.
* Número de falhas abertas.
* Tempo médio entre falhas.
* Tempo médio de recuperação.
* Uso de memória.
* Tempo de execução das tarefas.
* Estabilidade em testes prolongados.

---

# 9. Documentação Obrigatória

Antes da versão 1.0.0 deverão existir:

* SRS-001 a SRS-010.
* Diagramas de arquitetura.
* Esquemas elétricos.
* Manual do desenvolvedor.
* Manual do usuário.
* Guia de instalação.
* Plano de testes.
* Registro de versões (CHANGELOG).

---

# 10. Critérios de Conclusão

O projeto será considerado apto para uso quando:

* Todos os requisitos obrigatórios forem implementados.
* Todas as proteções forem validadas.
* O firmware operar de forma estável em bancada.
* Os testes de segurança forem aprovados.
* A documentação estiver completa e sincronizada com o código.

---

# 11. Próximas Etapas

Após concluir esta fase de especificação, o desenvolvimento seguirá a ordem:

1. Implementação dos módulos.
2. Testes unitários.
3. Testes de integração.
4. Simulação (Mock).
5. Simulação (Wokwi).
6. ESP32 físico.
7. Bancada com hardware.
8. Validação final.
9. Release 1.0.0.

---

# 12. Controle de Versão

| Versão | Data       | Descrição                                                   |
| ------ | ---------- | ----------------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Roadmap inicial e planejamento de evolução do firmware BMS. |

