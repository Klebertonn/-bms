# Storage Manager (system/storage)

Módulo central para persistência de dados do BMS (configurações, SOC/SOH, contadores e calibrações).

## Status
- Estrutura e CRC32 implementados
- StorageManager com load/save/factoryReset e defaults
- Drivers de backend (NVS/EEPROM/Flash) estão como stubs por enquanto

## Próximos passos
- Implementar drivers reais para ESP32 (NVS), EEPROM ou Flash (com fallback/backup)
- Implementar cópia primária/secundária e recuperação automática
- Implementar wear leveling e dirty flag
- Implementar migration quando STORAGE_VERSION mudar

