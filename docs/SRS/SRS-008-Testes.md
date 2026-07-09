# SRS-008 – Plano de Testes e Validação

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Em desenvolvimento
**Dependências:** SRS-001 a SRS-007

---

# 1. Objetivo

Este documento estabelece a estratégia oficial de testes do firmware do BMS.

Os testes deverão verificar:

* Funcionamento correto.
* Segurança.
* Desempenho.
* Robustez.
* Estabilidade.
* Compatibilidade entre Mock, Wokwi e hardware real.

Nenhuma versão deverá ser considerada pronta sem atender aos critérios definidos neste documento.

---

# 2. Estratégia de Validação

A validação será realizada em cinco etapas sequenciais:

1. Testes unitários.
2. Testes de integração.
3. Simulação Mock (PC).
4. Simulação no Wokwi.
5. Hardware ESP32.
6. Hardware completo com BMS.

Cada etapa deve ser aprovada antes da próxima.

---

# 3. Testes Unitários

Cada módulo deverá possuir testes próprios.

Módulos mínimos:

* Sensors
* Battery
* Protection
* Balancing
* Communication
* Logger
* Fault Manager
* Event Manager
* HAL
* Storage

Critérios:

* Entradas válidas.
* Entradas inválidas.
* Limites mínimos.
* Limites máximos.
* Tratamento de erros.

---

# 4. Testes de Integração

Validar a interação entre módulos.

Exemplos:

* Sensors → Battery.
* Battery → Protection.
* Protection → State Machine.
* State Machine → Communication.
* Logger → Storage.

---

# 5. Testes em Mock (PC)

Objetivo:

Executar toda a lógica do firmware sem hardware.

Devem ser simulados:

* Tensões das células.
* Corrente.
* Temperatura.
* Balanceamento.
* Eventos.
* Falhas.

Critérios:

* Sem travamentos.
* Logs consistentes.
* Estados corretos.

---

# 6. Testes no Wokwi

Objetivo:

Validar o firmware em um ESP32 virtual.

Verificar:

* Inicialização.
* Dashboard Serial.
* Atualização das leituras simuladas.
* Comunicação Serial.
* Mudanças de estado.

---

# 7. Testes no ESP32

Objetivo:

Executar o firmware em hardware físico.

Validar:

* Inicialização.
* Uso de memória.
* Watchdog.
* Estabilidade.
* Comunicação Serial.

---

# 8. Testes com Hardware Completo

Após integrar sensores e periféricos, validar:

* Leitura das tensões das células.
* Leitura da corrente.
* Leitura das temperaturas.
* Balanceamento.
* Controle dos MOSFETs.
* CAN.
* BLE.
* Display OLED.

Todos os testes devem ser realizados inicialmente em bancada, com equipamentos adequados e respeitando as especificações dos componentes.

---

# 9. Testes de Proteção

Validar individualmente:

## OVP

Simular sobretensão.

Resultado esperado:

* Desligar carga.
* Registrar falha.
* Atualizar estado.

---

## UVP

Simular subtensão.

Resultado esperado:

* Desligar descarga.
* Registrar falha.

---

## OCP

Simular corrente acima do limite.

Resultado esperado:

* Desligar MOSFET correspondente.
* Registrar evento.

---

## OTP

Simular temperatura elevada.

Resultado esperado:

* Bloquear carga.
* Bloquear descarga.
* Encerrar balanceamento.

---

## UTP

Simular baixa temperatura.

Resultado esperado:

* Bloquear carga conforme configuração.

---

# 10. Testes de Comunicação

## CAN

Verificar:

* Inicialização.
* Envio periódico.
* Recepção de comandos.
* Recuperação após falha.

---

## BLE

Verificar:

* Conexão.
* Leitura de dados.
* Atualização de características.
* Alteração autorizada de configurações.

---

## Serial

Verificar:

* Dashboard.
* CLI.
* Logs.

---

# 11. Testes de Estresse

Executar o firmware continuamente por longos períodos.

Monitorar:

* Uso de memória.
* Tempo das tarefas.
* Reinicializações.
* Estabilidade.

---

# 12. Testes de Persistência

Validar:

* Salvamento de configurações.
* Histórico de falhas.
* Contador de ciclos.
* Recuperação após reinicialização.

---

# 13. Testes de Recuperação

Simular:

* Reinicialização inesperada.
* Queda de alimentação.
* Falha de comunicação.
* Reinício por watchdog.

Verificar se o sistema retorna a um estado seguro.

---

# 14. Checklist de Aprovação

Antes de liberar uma versão:

* Compila sem erros.
* Compila sem warnings relevantes.
* Testes unitários aprovados.
* Testes de integração aprovados.
* Mock aprovado.
* Wokwi aprovado.
* ESP32 aprovado.
* Proteções aprovadas.
* Comunicação aprovada.
* Dashboard funcional.
* Logs consistentes.
* Documentação atualizada.

---

# 15. Critérios de Aceitação

Uma versão será considerada apta quando:

* Todos os testes obrigatórios forem aprovados.
* Nenhuma falha crítica permanecer aberta.
* A documentação estiver sincronizada com o código.
* O firmware permanecer estável durante os testes planejados.

---

# 16. Evoluções Futuras

A estratégia de testes deverá permitir a inclusão de:

* Testes automatizados em CI.
* Testes de regressão.
* Testes de desempenho.
* Testes HIL (Hardware-in-the-Loop).
* Testes de compatibilidade com novos sensores e placas.

---

# 17. Controle de Versão

| Versão | Data       | Descrição                                         |
| ------ | ---------- | ------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Definição inicial do plano de testes e validação. |

