# TODO — SPRINT TEST-INFRA/NATIVE-ENDURANCE

## Objetivo
Finalizar a infraestrutura de testes e criar execução contínua (endurance) do BMS no ambiente native (Option A: manter apenas os testes ativos `test/native/test_*`).

## Passos

### Infraestrutura de build/teste
- [ ] 1. platformio.ini — Remover de `lib_deps` os módulos locais `bms-core`, `bms-system`, `bms-communication` (não são pacotes de registry). Manter `lib_extra_dirs`. Limpar comentários/duplicações.
- [ ] 2. platformio.ini — Garantir que `build_src_filter` do env native não interfira no build de teste; definir `test_dir` se necessário.
- [ ] 3. scripts/test_build_hook.py — Reescrever para adicionar corretamente as fontes de produção (core/system/communication/lib) ao build de teste native via mecanismo correto do PlatformIO (PIOTEST_SRC_FILTER / BuildSources), resolvendo os `undefined reference`. Não mascarar erros de link.
- [ ] 4. Garantir que `unity.h` seja encontrado no build de teste (via `[env] test_framework = unity` + lib_deps do Unity).

### Validação de testes
- [ ] 5. Executar `pio test -e native` e registrar resultados (quantidade, pass, fail, compile/link/runtime errors).
- [ ] 6. Corrigir falhas reais (sem alterar testes para forçá-los a passar até serem válidos).
- [ ] 7. Executar novamente até PASS.

### Build nativo
- [ ] 8. Executar `pio run -e native` e confirmar PASS.
- [ ] 9. Executar `pio run -e esp32dev` (se disponível) e confirmar estado.

### Modo endurance (separado dos testes unitários)
- [ ] 10. Criar `tools/bms_endurance/main_endurance.cpp` reutilizando a classe `App` (sem segunda implementação).
- [ ] 11. Implementar argumento `--endurance` no entrypoint (CTRL+C para terminar).
- [ ] 12. Loop contínuo `${INIT → SELF TEST → READY → UPDATE → UPDATE → ...}` com `MAIN_LOOP_PERIOD_MS` (sleep, não busy loop).
- [ ] 13. Telemetria periódica a cada 5s (UPTIME, LOOPS, STATE, PACK, MIN/MAX CELL, DELTA, CURRENT, TEMP, SOC, SOH, ACTIVE DTC, HEARTBEAT, LOOP ERRORS, STORAGE ERR) — valores reais dos módulos.
- [ ] 14. Contadores de estabilidade: loops, uptime, fault_count, storage_errors, heartbeat_errors, self_test_failures, exceptions, loop_time (avg/max).
- [ ] 15. Fault Injection para cenários 1-6 (overvoltage→fault→clear, undervoltage, overtemperature, overcurrent, short circuit) usando ProtectionManager→FaultManager.
- [ ] 16. Persistência: confirmar `CLEAR_FAULT_HISTORY_ON_BOOT = false` (eventos não desaparecem entre execuções).
- [ ] 17. Evitar inflação de occurrence (confirmar que raiseFault só ocorre em transição nova; já correto no ProtectionManager).

### Relatório + longa duração
- [ ] 18. Criar `reports/` e gerar relatório automático de endurance ao final (Start, End, Duration, Loops, DTC Raised/Cleared, erros, self test, estado final, RESULT).
- [ ] 19. Executar endurance: 5 min, 30 min, 1 h (sem crash/segfault/assert/deadlock/crescimento de memória/corrupção de histórico/inflação de DTC).
- [ ] 20. Relatório final objetivo (arquivos alterados, problemas, correções, testes, comandos, resultado endurance, pendências, recomendação hardware).

