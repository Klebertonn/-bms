# DDS-003 – Drivers de Hardware

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001 e DDS-002

---

# 1. Objetivo

Este documento define os drivers de hardware suportados pelo firmware do BMS.

Os drivers são responsáveis por implementar a comunicação com os dispositivos físicos através da HAL, fornecendo uma interface padronizada para os módulos superiores.

---

# 2. Princípios

Todos os drivers deverão:

* Utilizar exclusivamente a HAL.
* Não conter lógica de negócio.
* Ser independentes entre si.
* Permitir substituição por outros dispositivos equivalentes.

---

# 3. Organização Recomendada

```text id="drv001"
drivers/
├── adc/
├── current_sensor/
├── temperature/
├── can/
├── ble/
├── display/
├── storage/
├── mosfet/
├── buzzer/
└── leds/
```

---

# 4. Driver ADC

## Objetivo

Realizar a aquisição das tensões das células e da tensão total do pack.

### Funcionalidades

* Inicialização.
* Calibração.
* Leitura de canais.
* Filtro de ruído.
* Verificação de limites.

### Entradas

* Canais analógicos.

### Saídas

* Tensões em volts.

---

# 5. Driver do Sensor de Corrente

Sensores previstos:

* INA219
* INA226
* Sensores Hall compatíveis

### Funcionalidades

* Corrente instantânea.
* Tensão do barramento.
* Potência.
* Diagnóstico.

---

# 6. Driver de Temperatura

Sensores previstos:

* NTC
* LM35
* DS18B20
* Outros compatíveis

### Funcionalidades

* Leitura.
* Conversão.
* Calibração.
* Detecção de falha.

---

# 7. Driver CAN (TWAI)

Responsabilidades:

* Inicializar controlador.
* Configurar bitrate.
* Enviar frames.
* Receber frames.
* Detectar erros.
* Recuperação automática.

---

# 8. Driver BLE

Responsabilidades:

* Inicialização.
* Serviços.
* Characteristics.
* Notificações.
* Escrita.
* Reconexão.

---

# 9. Driver Display OLED

Display previsto:

* SSD1306

Funcionalidades:

* Inicialização.
* Limpeza.
* Escrita de texto.
* Ícones.
* Atualização parcial.
* Telas rotativas.

---

# 10. Driver Storage

Memórias suportadas:

* NVS
* EEPROM
* Flash

Responsabilidades:

* Salvar configurações.
* Histórico de falhas.
* Contador de ciclos.
* Estado de saúde.

---

# 11. Driver MOSFET

Responsabilidades:

* Controle do MOSFET de carga.
* Controle do MOSFET de descarga.
* Controle dos MOSFETs de balanceamento.
* Verificação de estado.

---

# 12. Driver Buzzer

Responsabilidades:

* Alarmes sonoros.
* Diferentes padrões de aviso.
* Silenciamento configurável.

---

# 13. Driver LEDs

Responsabilidades:

* Estado do sistema.
* Indicação de carga.
* Indicação de falhas.
* Diagnóstico visual.

---

# 14. Inicialização

A sequência recomendada será:

```text id="drv002"
HAL
 ↓
ADC
 ↓
Current Sensor
 ↓
Temperature
 ↓
Storage
 ↓
CAN
 ↓
BLE
 ↓
OLED
 ↓
MOSFET
 ↓
Buzzer
 ↓
LEDs
```

Cada driver deverá validar sua inicialização e reportar falhas ao Logger e ao Fault Manager.

---

# 15. Interface Padrão

Todos os drivers deverão implementar, quando aplicável, as operações:

* init()
* deinit()
* read()
* write()
* reset()
* selfTest()

Isso facilita a padronização e os testes.

---

# 16. Tratamento de Falhas

Cada driver deverá identificar:

* Erro de comunicação.
* Timeout.
* Valor inválido.
* Falha de inicialização.
* Falha de hardware.

As falhas deverão ser encaminhadas ao Fault Manager.

---

# 17. Requisitos de Desempenho

* Baixa latência.
* Uso reduzido de memória.
* Compatibilidade com FreeRTOS.
* Operação determinística.

---

# 18. Testes

Cada driver deverá possuir testes específicos para:

* Inicialização.
* Operação normal.
* Operação em limite.
* Recuperação de falhas.
* Integração com a HAL.

---

# 19. Critérios de Aceitação

O conjunto de drivers será considerado aprovado quando:

* Todos os periféricos suportados funcionarem corretamente.
* Os drivers forem independentes da lógica de negócio.
* Os módulos superiores utilizarem apenas interfaces padronizadas.
* Os testes previstos forem aprovados.

---

# 20. Evoluções Futuras

A arquitetura deverá permitir a adição de novos drivers, como:

* Cartão SD.
* Wi-Fi.
* Ethernet.
* GPS.
* RTC externo.
* Sensores adicionais.

---

# 21. Controle de Versão

| Versão | Data       | Descrição                                                |
| ------ | ---------- | -------------------------------------------------------- |
| 1.0.0  | 03/07/2026 | Definição da arquitetura dos drivers de hardware do BMS. |

