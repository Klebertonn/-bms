
# BLE Manager (communication/ble)

Módulo responsável por comunicação via Bluetooth Low Energy.

## Responsabilidades
- Inicializar BLE
- Gerenciar conexão
- Disponibilizar serviços e características GATT
- Enviar notificações em tempo real
- Receber comandos para configuração/controle
- Implementar autenticação e níveis de acesso (stub por enquanto)
- Publicar telemetria (stub por enquanto)

## Status
- Estrutura completa criada.
- Implementação funcional fica dependente do framework BLE escolhido (ex.: NimBLE-Arduino, BLEServer do Arduino-ESP32, ou ESP-IDF).
- Próximo passo: conectar as características/handlers aos módulos CORE (Battery/Protection/SOC/Temperature/Current/Balancing).

