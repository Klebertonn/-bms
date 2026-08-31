# DDS-002 – Hardware Abstraction Layer (HAL)

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Arquitetura de Implementação

**Dependências:** DDS-001

---

# 1. Objetivo

Este documento especifica a Hardware Abstraction Layer (HAL), responsável por isolar completamente o firmware das APIs específicas do hardware.

Os objetivos da HAL são:

* Permitir reutilização do firmware.
* Facilitar testes em Mock.
* Permitir simulação no Wokwi.
* Simplificar a troca de componentes físicos.
* Reduzir acoplamento entre hardware e lógica de negócio.

---

# 2. Princípios

A HAL deverá:

* Expor interfaces padronizadas.
* Ocultar detalhes de implementação.
* Não conter lógica de negócio.
* Ser substituível conforme o ambiente.

---

# 3. Ambientes Suportados

A HAL deverá possuir implementações específicas para:

* MOCK (PC)
* WOKWI (ESP32 Virtual)
* HARDWARE (ESP32 Real)

A seleção ocorrerá em tempo de compilação.

---

# 4. Estrutura Recomendada

```text id="hal001"
hal/
├── adc/
│   ├── hal_adc.h
│   ├── hal_adc_mock.cpp
│   ├── hal_adc_wokwi.cpp
│   └── hal_adc_esp32.cpp
│
├── gpio/
├── pwm/
├── can/
├── ble/
├── uart/
├── i2c/
├── spi/
├── storage/
├── display/
├── rtc/
└── platform/
```

---

# 5. Interfaces da HAL

## HAL_ADC

Responsabilidades:

* Ler tensão das células.
* Ler tensão do pack.
* Ler corrente.
* Ler sensores analógicos.

Operações:

* Inicializar ADC.
* Ler canal.
* Calibrar.
* Validar leitura.

---

## HAL_GPIO

Responsabilidades:

* Configurar pinos.
* Ler entradas.
* Acionar saídas.
* Controlar LEDs.

---

## HAL_PWM

Responsabilidades:

* Controlar MOSFETs de balanceamento.
* Controlar buzzer (quando presente).
* Ajustar duty cycle.

---

## HAL_CAN

Responsabilidades:

* Inicializar TWAI.
* Transmitir mensagens.
* Receber mensagens.
* Monitorar erros.

---

## HAL_BLE

Responsabilidades:

* Inicializar BLE.
* Publicar características.
* Receber comandos.
* Gerenciar conexões.

---

## HAL_UART

Responsabilidades:

* Comunicação Serial.
* CLI.
* Dashboard.
* Logs.

---

## HAL_I2C

Responsabilidades:

* Comunicação com sensores I²C.
* OLED.
* EEPROM externa.

---

## HAL_SPI

Responsabilidades:

* Comunicação com dispositivos SPI.
* Expansões futuras.

---

## HAL_STORAGE

Responsabilidades:

* NVS.
* EEPROM.
* Flash.
* Configurações persistentes.

---

## HAL_DISPLAY

Responsabilidades:

* Inicializar OLED.
* Atualizar telas.
* Exibir alarmes.
* Exibir status.

---

## HAL_RTC

Responsabilidades:

* Data.
* Hora.
* Timestamp de eventos.
* Sincronização futura.

---

# 6. Seleção da Implementação

A seleção deverá ocorrer por macros de compilação.

Perfis suportados:

* PROFILE_MOCK
* PROFILE_WOKWI
* PROFILE_HARDWARE

Nenhum módulo acima da HAL deverá conhecer essas diferenças.

---

# 7. Fluxo de Dados

```text id="hal002"
Application
      │
      ▼
Managers
      │
      ▼
Services
      │
      ▼
HAL Interface
      │
      ├── Mock
      ├── Wokwi
      └── ESP32
```

---

# 8. Tratamento de Erros

Cada função da HAL deverá retornar um código de status.

Exemplos:

* HAL_OK
* HAL_ERROR
* HAL_TIMEOUT
* HAL_INVALID_PARAMETER
* HAL_NOT_SUPPORTED
* HAL_NOT_INITIALIZED

Nenhuma função deverá encerrar o sistema diretamente.

---

# 9. Requisitos de Desempenho

* Chamadas rápidas.
* Sem alocação dinâmica desnecessária.
* Sem bloqueios prolongados.
* Compatível com execução em tempo real.

---

# 10. Testabilidade

Cada implementação deverá possuir testes próprios.

Itens mínimos:

* Inicialização.
* Leitura.
* Escrita.
* Tratamento de erro.
* Recuperação.

---

# 11. Extensibilidade

A HAL deverá permitir a inclusão de novos periféricos sem alterar os módulos superiores.

Exemplos:

* Wi-Fi.
* Ethernet.
* Cartão SD.
* GPS.
* Novos sensores.

---

# 12. Critérios de Aceitação

A HAL será considerada aprovada quando:

* O firmware compilar para Mock, Wokwi e Hardware usando a mesma lógica de negócio.
* Não houver chamadas diretas às APIs do ESP32 fora da HAL.
* Todas as interfaces forem documentadas.
* Os testes de cada implementação forem aprovados.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                                |
| ------ | ---------- | -------------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Definição da Hardware Abstraction Layer do firmware BMS. |

