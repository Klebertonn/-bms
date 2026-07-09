#pragma once

// Limites iniciais (stubs). Ajustar conforme características do pack.

// Tensões por célula
#ifndef MAX_CELL_VOLTAGE
#define MAX_CELL_VOLTAGE 4.20f
#endif

#ifndef MIN_CELL_VOLTAGE
#define MIN_CELL_VOLTAGE 3.00f
#endif

// Temperatura (°C)
#ifndef MAX_TEMPERATURE
#define MAX_TEMPERATURE 60.0f
#endif

#ifndef MIN_TEMPERATURE
#define MIN_TEMPERATURE -20.0f
#endif

// Corrente (A)
#ifndef MAX_CHARGE_CURRENT
#define MAX_CHARGE_CURRENT 30.0f
#endif

#ifndef MAX_DISCHARGE_CURRENT
#define MAX_DISCHARGE_CURRENT 30.0f
#endif

// Balanceamento (stubs)
#ifndef BALANCE_START_DELTA
#define BALANCE_START_DELTA 0.020f
#endif

#ifndef BALANCE_STOP_DELTA
#define BALANCE_STOP_DELTA 0.010f
#endif

#ifndef MAX_BALANCE_TEMP
#define MAX_BALANCE_TEMP 45.0f
#endif

#ifndef MAX_BALANCE_TIME_MS
#define MAX_BALANCE_TIME_MS 600000
#endif


