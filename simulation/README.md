# simulation/

Ambiente de **simulação** e mocks.

> **Nota sobre a reorganização (Sprint de estruturação):**
> Para **não quebrar o build**, os mocks permanecem nos seus locais físicos
> originais. Esta pasta documenta o mapeamento lógico da simulação.

## Mapeamento de Mocks / Simulação

| Mock / Simulador            | Localização física                  |
|-----------------------------|-------------------------------------|
| ADC Mock                    | `src/hal/adc/mock_adc.*`            |
| Corrente Mock               | `src/hal/current/current_mock.cpp`  |
| Temperatura Mock            | `src/hal/temperature/temperature_mock.*` |
| MOSFET Mock                 | `src/hal/gpio/mock_mosfet_driver.h` |
| CAN Mock                    | `communication/can/can_mock.cpp`    |
| Ambiente native (desktop)   | `platformio.ini` → `[env:native]`   |
| Dashboard (visualização)    | `web/` (raiz)                       |

## Executar simulação (native)

```bash
pio run -e native
.pio/build/native/program.exe
