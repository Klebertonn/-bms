# TODO — SPRINT INDUSTRIALIZATION / NATIVE-ENDURANCE

## Objetivo
Evoluir o MVP para uma versão industrial verificável, mantendo o ambiente native como
gate automatizado e adicionando cobertura de integração, endurance e validação no ESP32.

## Baseline confirmado — 2026-09-04
- `pio test -e native`: **41/41 casos aprovados**, em 5 suítes (`app`, `fault`, `fault_injection`, `self_test`, `storage`).
- O hook `scripts/test_build_hook.py` resolve as fontes reais de produção sem mascarar erros de link.
- O diretório `test/native/` é o conjunto ativo; `tests/` permanece legado até conversão para suítes do runner.
- Smoke endurance: `--duration-ms 1000` executou 10 ciclos, sem exceções, e gerou `reports/endurance_latest.txt`.
- Fault injection: 6 cenários de proteção passaram (detecção, recuperação e MOSFET).
- Ensaio endurance: `--duration-ms 5200` executou 48 ciclos, sem exceções, com latência média de 29 ms e máxima de 125 ms.
- Endurance de 5 minutos: 300.047 ms, 2.698 ciclos, média de 25 ms, máximo de 172 ms, zero exceções/storage/self-test; relatório arquivado em `reports/endurance_5m_2026-09-04.txt`.
- Endurance de 30 minutos: 1.800.062 ms, 16.392 ciclos, zero exceções/storage/self-test; relatório arquivado em `reports/endurance_30m_2026-09-04.txt`.
- Instrumentação de latência refinada para microssegundos; smoke final: média de 32.076 us, máximo de 54.001 us.
- Endurance de 1 hora: 3.600.015 ms, 32.383 ciclos, média de 960 us, máximo de 55.531 us, zero exceções/storage/self-test; relatório arquivado em `reports/endurance_1h_2026-09-04.txt`.

## Passos

### Infraestrutura de build/teste
- [x] 1. `platformio.ini` — módulos locais não estão em `lib_deps`; `lib_extra_dirs` permanece ativo.
- [x] 2. `platformio.ini` — o ambiente native não interfere no build de teste; o runner usa o diretório padrão `test/`.
- [x] 3. `scripts/test_build_hook.py` — fontes reais são adicionadas via `BuildSources`, sem mascarar erros de link.
- [x] 4. `unity.h` — Unity é resolvido pelo `test_framework = unity`.

### Validação de testes
- [x] 5. Executar `pio test -e native` e registrar resultados: 5 suítes, 41 pass, 0 fail, 0 erros de compile/link/runtime.
- [x] 6. Corrigir falhas reais encontradas na infraestrutura, sem alterar testes para forçar aprovação.
- [x] 7. Executar novamente até PASS.

### Build nativo
- [x] 8. Executar `pio run -e native` e confirmar PASS.
- [x] 9. Executar `pio run -e esp32dev` e confirmar PASS: imagem ESP32 criada, sem erros de compilação/link.

### Modo endurance (separado dos testes unitários)
- [x] 10. Criar `tools/bms_endurance/main_endurance.cpp` reutilizando a classe `App` (sem segunda implementação).
- [x] 11. Implementar argumento `--endurance` no entrypoint (CTRL+C para terminar).
- [x] 12. Loop contínuo `${INIT → SELF TEST → READY → UPDATE → UPDATE → ...}` com `MAIN_LOOP_PERIOD_MS` (sleep, não busy loop).
- [x] 13. Telemetria periódica a cada 5s com valores reais de uptime, loops, estado, pack, células, delta, corrente, temperatura, SOC/SOH, DTC, heartbeat e erros.
- [x] 14. Contadores de estabilidade: loops, uptime, DTC raised/cleared, storage_errors, heartbeat_errors, self_test_failures, exceptions e loop_time (avg/max).
- [x] 15. Fault Injection para cenários 1-6 (overvoltage→fault→clear, undervoltage, overtemperature, overcurrent, short circuit) usando ProtectionManager→FaultManager.
- Estado operacional corrigido para reportar `FAULT` quando há DTC ativo; teste contra inflação de DTC aprovado.
- Watchdog de software implementado com timeout de 500 ms, heartbeat por ciclo e contagem de perdas; watchdog de hardware ESP32 permanece pendente.
- [x] 16. Persistência: `CLEAR_FAULT_HISTORY_ON_BOOT = false`; execuções subsequentes carregaram o evento persistido (`Events found: 1`).
- [x] 17. Evitar inflação de occurrence: teste de múltiplos ciclos confirmou um único DTC levantado por transição.

### Relatório + longa duração
- [x] 18. Criar `reports/` e gerar relatório automático de endurance ao final com métricas de execução, DTC ativo, erros, self-test, estado final e RESULT.
- [x] 19. Executar endurance de 5 min, 30 min e 1 h sem crash/segfault/assert/deadlock, com histórico estável e sem inflação de DTC.
- [x] 20. Relatório final objetivo em `reports/validation_report_2026-09-04.md`, com problemas, correções, testes, comandos, endurance, pendências e recomendação de hardware/certificação.

