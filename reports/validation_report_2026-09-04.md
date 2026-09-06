# BMS Validation Report - 2026-09-04

## Scope

Software and build validation for the BMS firmware. This report does not claim hardware, safety, or regulatory certification.

## Automated evidence

| Gate | Result | Evidence |
|---|---|---|
| Native build | PASS | `pio run -e native` |
| ESP32 build | PASS | `pio run -e esp32dev` |
| Native tests | PASS | 41/41 cases, 5 suites |
| Protection injection | PASS | over/under voltage, temperature, current, short circuit, recovery |
| Endurance 5 min | PASS | `reports/endurance_5m_2026-09-04.txt` |
| Endurance 30 min | PASS | `reports/endurance_30m_2026-09-04.txt` |
| Endurance 1 hour | PASS | `reports/endurance_1h_2026-09-04.txt` |
| Software watchdog | PASS | 500 ms heartbeat timeout, native integration test |
| ESP32 hardware watchdog build | PASS | TWDT initialization/reset and reset reason logging compile |
| Test report generation | PASS | `scripts/report/index.html` |
| Build warnings audit | PASS | No warnings/errors captured in native and ESP32 filtered builds |

## Observed behavior

- The native mock intentionally starts with one cell at 4.30 V, producing an active cell overvoltage DTC.
- Charge MOSFET is disabled and discharge MOSFET remains enabled for that scenario.
- DTC raised count remains one across repeated update cycles; no occurrence inflation was observed.
- Fault history is loaded on subsequent executions with `CLEAR_FAULT_HISTORY_ON_BOOT = false`.

## Remaining acceptance gates

1. Hardware bench validation with current-limited supply, fuse, calibrated multimeter and oscilloscope where applicable.
2. ADC divider accuracy, cell measurement range and sensor-disconnect behavior.
3. Real MOSFET charge/discharge/balance safe-state and short-circuit tests.
4. Real CAN, BLE and OLED interoperability tests.
5. Hardware watchdog timeout and reset-cause test on ESP32.
6. Brownout, power-cycle, reset recovery and persistent-storage corruption tests.
7. Thermal, EMC, ESD, abuse and pack-level safety tests appropriate to the product.
8. Reproducible release build with pinned toolchain and dependency versions.
9. Product-specific regulatory and certification review. Depending on application, evaluate applicable requirements such as UN 38.3, IEC 62619, IEC 62133, ISO 26262, or IEC 61508 with a qualified laboratory.

## Release recommendation

Do not release as production-certified firmware yet. The software gate is strong for native simulation and compilation, but physical safety behavior and product certification remain open.
