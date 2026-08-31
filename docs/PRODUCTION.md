# Preparação para Produção — Checklist Industrial

**Projeto:** BMS Firmware
**Versão:** 1.0.0

Este documento é o **checklist de aceitação industrial** para liberação do firmware para produção.

---

## 1. Build e Compilação

- [ ] `pio run -e native` compila sem erros.
- [ ] `pio run -e esp32dev` compila sem erros (se plataforma disponível).
- [ ] Sem *warnings* críticos (overflow, uninitialized, etc.).
- [ ] Build reproduzível (mesma versão de toolchain).

## 2. Self Test (POST)

- [ ] Boot executa `SELF_TEST`.
- [ ] Todos os 8 testes (ADC, EEPROM, FLASH, NVS, Current, Temperature, MOSFET, RAM) validam.
- [ ] `READY` apenas se todos os testes passarem.
- [ ] `FAULT` se qualquer teste falhar.
- [ ] Log de self test visível no diagnóstico.

## 3. Proteções Elétricas

- [ ] Sobretensão de célula → desliga MOSFET de carga.
- [ ] Subtensão de célula → desliga MOSFET de descarga.
- [ ] Sobre/subtemperatura → ação de proteção.
- [ ] Sobrecorrente (carga/descarga) → ação de proteção.
- [ ] Curto-circuito → ação de proteção.
- [ ] Falha de sensor → detecção de `SENSOR_FAILURE`.

## 4. Máquina de Estados

- [ ] `BOOT → INIT → SELF_TEST → READY`.
- [ ] Transições `IDLE`, `CHARGING`, `DISCHARGING`, `BALANCING`, `FAULT`, `RECOVERY`, `SHUTDOWN`.
- [ ] Falha crítica → `FAULT`.
- [ ] Recuperação de falha.

## 5. MOSFETs

- [ ] Controle independente de carga/descarga/balanço.
- [ ] Estados ON/OFF/Protegido/Emergência.
- [ ] Ação segura em falha (default OFF/tudo seguro).

## 6. Diagnóstico e Persistência

- [ ] Fault History persiste eventos.
- [ ] Logger registra eventos com timestamp.
- [ ] Storage (NVS/arquivo) funciona.
- [ ] Histórico carrega no boot.

## 7. Comunicação

### CAN
- [ ] Heartbeat periódico (0x100).
- [ ] Telemetria do pack (0x101).
- [ ] Recepção de frames (diagnóstico/CLI).

### BLE
- [ ] Serviço GATT inicializa.
- [ ] Leitura de dados principais.

## 8. Telemetria e Display

- [ ] Telemetria correta (tensão, corrente, temperatura, SOC, SOH).
- [ ] Display OLED mostra estado do sistema.

## 9. Watchdog e Confiabilidade

- [ ] Watchdog de software ativo.
- [ ] Watchdog de hardware (quando disponível).
- [ ] Reinício seguro registra causa.

## 10. Segurança Funcional

- [ ] HAL impede acesso direto a periféricos.
- [ ] Limites de configuração em `config/config.h`.
- [ ] Comportamento fail-safe em caso de erro.

## 11. Testes

- [ ] Testes unitários passam.
- [ ] Testes de integração passam.
- [ ] Relatório de testes gerado (`test/generate_report.py`).

## 12. Documentação

- [ ] `README.md` completo.
- [ ] `docs/ARCHITECTURE.md` atualizado.
- [ ] `CHANGELOG.md` atualizado.
- [ ] `SECURITY.md` presente.
- [ ] `CONTRIBUTING.md` presente.
- [ ] SRS/DDS/ICD coesos com a implementação.

## 13. Repositório / GitHub

- [ ] `.gitignore` correto.
- [ ] `LICENSE` presente.
- [ ] Estrutura de pastas organizada.
- [ ] Tag de release (`v1.0.0`).

---

## Critérios de Bloqueio (Gate)

Para considerar o firmware **pronto para produção**, os itens marcados como **críticos** devem estar **100%**:

- **CRÍTICO**: Self Test (2), Proteções Elétricas (3), Máquina de Estados (4), MOSFETs (5), Watchdog (9), Segurança Funcional (10).
- **ALTO**: Diagnóstico e Persistência (6), Comunicação (7), Telemetria (8).
- **MÉDIO**: Testes (11), Documentação (12), Repositório (13).

---

## Registro de Aprovação

| Data | Versão | Aprovado por | Status |
|------|--------|--------------|--------|
| 2026-07-03 | 1.0.0 | — | Pendente |
