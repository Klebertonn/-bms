# TODO

- [x] Ler arquivos relevantes do BatteryManager (`core/battery/battery_manager.h/.cpp`) e identificar uso de `PackData`.
- [x] Etapa 1: Remover completamente `PackData` e `CellData` do BatteryManager.
- [x] Etapa 2: Introduzir `BatteryPack& getPack()` e `const BatteryPack& getPack() const;` (remover `getPackData()`).
- [x] Etapa 3: Atualizar implementação (`init/update/calculatePack/calculateSOC`) para preencher/usar `BatteryPack`.
- [x] Rodar `platformio run` e corrigir todos os call sites que ainda usm `getPackData()`/`PackData`.

