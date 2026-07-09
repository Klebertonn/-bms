# DDS-007 – Communication Manager

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001 até DDS-006

---

# 1. Objetivo

O Communication Manager centraliza toda a comunicação do BMS com dispositivos externos.

Ele deverá fornecer uma interface única para envio e recebimento de informações, independentemente do meio de comunicação utilizado.

---

# 2. Objetivos

* Centralizar toda a comunicação.
* Evitar duplicação de código.
* Facilitar futuras expansões.
* Padronizar mensagens.
* Garantir confiabilidade.

---

# 3. Interfaces Suportadas

Na versão 1.0:

* Serial UART
* CLI (Command Line Interface)
* Dashboard Serial
* CAN (TWAI)
* Bluetooth Low Energy (BLE)
* Display OLED

Versões futuras:

* Wi-Fi
* MQTT
* Ethernet
* Modbus RTU
* Modbus TCP
* USB CDC

---

# 4. Arquitetura

```text
                Communication Manager
                         │
     ┌──────────┬────────┼────────┬──────────┐
     ▼          ▼        ▼        ▼          ▼
   Serial      CAN      BLE     OLED       CLI
```

Cada interface será implementada em um módulo independente.

---

# 5. Dashboard Serial

O Dashboard deverá exibir informações em tempo real:

* Estado do sistema.
* Tensão do pack.
* Corrente.
* SOC.
* SOH.
* SOE.
* Temperaturas.
* Estado dos MOSFETs.
* Balanceamento.
* Falhas.
* Tempo de execução do loop.
* Uso de memória.

A frequência de atualização deverá ser configurável.

---

# 6. CLI (Command Line Interface)

A CLI permitirá:

* Consultar parâmetros.
* Alterar configurações.
* Executar testes.
* Calibrar sensores.
* Reiniciar o sistema.
* Limpar histórico de falhas.
* Restaurar configurações de fábrica.

Todos os comandos deverão possuir resposta padronizada.

---

# 7. CAN (TWAI)

Responsabilidades:

* Inicialização.
* Configuração do bitrate.
* Transmissão periódica.
* Recepção de comandos.
* Diagnóstico de barramento.
* Tratamento de erros.

As mensagens deverão possuir:

* ID.
* Prioridade.
* Periodicidade.
* Tamanho.
* Check de integridade quando necessário.

---

# 8. Bluetooth Low Energy (BLE)

O BLE deverá disponibilizar:

* Serviço principal do BMS.
* Características para leitura.
* Características para escrita.
* Notificações automáticas.
* Estado da conexão.

Dados disponíveis:

* Tensões.
* Correntes.
* Temperaturas.
* SOC.
* SOH.
* Alarmes.
* Estado dos MOSFETs.
* Balanceamento.

---

# 9. Display OLED

O Display deverá apresentar:

* Tela inicial.
* Estado do sistema.
* SOC.
* Tensão do pack.
* Corrente.
* Temperatura.
* Alarmes ativos.
* Estado do balanceamento.

As telas poderão alternar automaticamente ou por comando.

---

# 10. Formato das Mensagens

As mensagens deverão seguir formatos padronizados.

Exemplo de estrutura lógica:

* Timestamp.
* Origem.
* Tipo.
* Dados.
* CRC (quando aplicável).

Essa padronização facilitará futuras integrações.

---

# 11. Prioridade de Comunicação

Ordem de prioridade:

1. Alarmes críticos.
2. Falhas.
3. Estado do sistema.
4. Dados da bateria.
5. Informações estatísticas.
6. Logs de depuração.

---

# 12. Registro de Eventos

Todos os eventos importantes deverão ser registrados pelo Logger antes do envio aos canais de comunicação.

---

# 13. Configuração

Parâmetros configuráveis:

* Velocidade da UART.
* Bitrate CAN.
* Nome BLE.
* Intervalo de atualização.
* Nível de log.
* Idioma das mensagens (expansão futura).

---

# 14. Integração

O Communication Manager receberá dados de:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Fault Manager.
* Logger.
* Task Manager.

---

# 15. Requisitos de Desempenho

* Comunicação não bloqueante.
* Compatibilidade com FreeRTOS.
* Baixa utilização de CPU.
* Filas para envio de mensagens.
* Recuperação automática em caso de falha.

---

# 16. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Todas as interfaces funcionarem conforme especificado.
* As mensagens forem padronizadas.
* A comunicação não comprometer o tempo de resposta do sistema.
* O funcionamento for validado em Mock, Wokwi e ESP32 físico.

---

# 17. Evoluções Futuras

O projeto deverá permitir:

* Aplicativo Android/iOS.
* Dashboard Web.
* Atualização OTA.
* Integração com sistemas supervisórios.
* Comunicação em nuvem.

---

# 18. Controle de Versão

| Versão | Data       | Descrição                                       |
| ------ | ---------- | ----------------------------------------------- |
| 1.0.0  | 03/07/2026 | Especificação inicial do Communication Manager. |

