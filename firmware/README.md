# firmware/

Código embarcado do BMS (firmware).

> **Nota sobre a reorganização (Sprint de estruturação):**
> Para **não quebrar o build** (`pio run -e native`) e não modificar os ~160
> includes relativos existentes (regra de "não alterar includes sem necessidade"),
> o código-fonte **permanece fisicamente na raiz do repositório**:
> `core/`, `system/`, `communication/`, `lib/`, `src/`, `config/`.

Esta pasta é o **ponto de mapeamento lógico** do firmware. A localização física
dos diretórios é documentada abaixo:

| Módulo       | Localização física            |
|--------------|-------------------------------|
| Núcleo BMS   | `core/` (raiz)                |
| Drivers      | `src/hal/esp32/` (raiz)       |
| HAL          | `lib/hal/` (raiz)             |
| Comunicação  | `communication/` (raiz)       |
| Sistema      | `system/` (raiz)              |
| Config       | `config/config.h` (raiz)      |
| Aplicação    | `src/app/` (raiz)             |

## Build

```bash
# Ambiente desktop/mock (padrão)
pio run -e native

# Ambiente ESP32 (hardware real) — requer espressif32
pio run -e esp32dev
```

## Migração futura (opcional)
Para mover fisicamente para `firmware/`, será necessário:
1. Reescrever todos os includes relativos (`../`, `lib/`, `core/`, `system/`, `communication/`, `config/`).
2. Atualizar `build_flags` e `build_src_filter` no `platformio.ini`.
3. Revalidar `pio run -e native` e `pio test`.
