# TODO — SPRINT DTC-02: Unificação ProtectionManager → FaultManager → DTC

## Passos

- [x] 1. src/app/app.cpp — Remover `fault_.evaluate(pack)` como mecanismo primário (marcar como LEGACY COMPATIBILITY ONLY), deixando ProtectionManager como fonte principal de detecção.
- [x] 2. core/protection/protection_manager.cpp — Adicionar detecção de short circuit (BMS_SHORT_CIRCUIT) no update(), mantendo o padrão raiseFault/clearFault.
- [x] 3. test/native/test_fault/test_dtc.cpp — Adicionar os testes obrigatórios do sprint DTC-02 (8 testes: cell overvoltage com source/value corretos, normalização→CLEARED, undervoltage, overtemperature, overcurrent, múltiplas falhas, célula correta, short circuit).
- [x] 4. Validar: `pio run -e native` BUILD SUCCESS (linka program.exe corretamente).
- [ ] 5. Validar: `pio test -e native` TESTS — PRE-EXISTING INFRA ISSUE: lib_extra_dirs (.a) não são linkados no target de teste do PlatformIO native (afeta test_fault, test_self_test e test_storage, mesmo os que eu não modifiquei). Código DTC compila (pio run SUCCESS); a validação por pio test requer correção do ambiente de teste.
- [x] 6. Validar: executar `program.exe` — SELF TEST 8/8 OK, persistência intacta (History loaded, Events found: 1), DTC gerado via ProtectionManager (cell over voltage, CELL 3), ocorrência/estado corretos.
