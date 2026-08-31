/* ==========================================================
 * BMS Dashboard — Fonte de Dados
 * ==========================================================
 * Abstrai a origem dos dados do BMS.
 *
 * Modo 1 (padrão): DEMO — gera dados simulados realistas
 *                     (espelha o comportamento do BMS native).
 * Modo 2 (produção): API — busca em endpoint/WebSocket conectado
 *                     ao BMS real (ex.: ESP32 via HTTP/WebSocket).
 *
 * Para conectar ao hardware real, defina BMS_DATA_SOURCE="api"
 * e aponte BMS_API_URL para o seu endpoint.
 * ==========================================================
 */

const BMS_DATA_SOURCE = 'demo'; // 'demo' | 'api'
const BMS_API_URL = 'http://192.168.4.1/api/bms'; // exemplo ESP32 (AP)
const BMS_API_INTERVAL_MS = 2500;

/**
 * Estado compartilhado do dashboard.
 */
const BMSState = {
    data: null,
    history: [],
    heartbeat: { alive: false, lastBeat: 0 },
    listeners: [],
    timerApi: null,
    timerDemo: null,
    startTime: Date.now(),
    loopCount: 0,
};

/**
 * Registra um callback para ser chamado quando novos dados chegarem.
 */
function onBMSData(callback) {
    BMSState.listeners.push(callback);
}

/**
 * Notifica todos os listeners com os dados atuais.
 */
function notifyBMSData() {
    for (const cb of BMSState.listeners) {
        try {
            cb(BMSState.data, BMSState.history, BMSState.heartbeat);
        } catch (e) {
            console.error('Erro no listener do dashboard:', e);
        }
    }
}

/**
 * Gera um snapshot de dados simulados (demo).
 * Espelha o comportamento do BMS native:
 *   - 3 células: 4.30V / 4.18V / 4.19V (célula 0 em sobretensão)
 *   - Corrente: 2.35A
 *   - Temperatura: 25°C
 *   - SOC: 100% (tensão do pack acima do full)
 *   - SOH: 100%
 *   - Falha: Cell over voltage (DTC 0x0101)
 *   - MOSFET: Charge OFF, Discharge ON, Balance OFF
 */
function generateDemoData() {
    const now = Date.now();
    const uptimeMs = now - BMSState.startTime;
    BMSState.loopCount += 10; // ~10 loops por segundo (100ms cada)

    // Células (mock do BMS native)
    const cells = [
        { voltage: 4.30, valid: false },  // Sobretensão
        { voltage: 4.18, valid: true },
        { voltage: 4.19, valid: true },
    ];

    const packVoltage = cells.reduce((s, c) => s + c.voltage, 0);
    const minVoltage = Math.min(...cells.map(c => c.voltage));
    const maxVoltage = Math.max(...cells.map(c => c.voltage));
    const deltaVoltage = maxVoltage - minVoltage;

    const current = 2.35;
    const temperature = 25.0;
    const soc = 100;
    const soh = 100;

    // Falha ativa: Cell over voltage (célula 0)
    const faultActive = true;
    const fault = {
        active: faultActive,
        name: 'CELL_OVERVOLTAGE',
        code: 0x0101,
        severity: 'CRITICAL',
        cell: 1, // célula 1 (1-based)
        value: 4.30,
        limit: 4.25,
    };

    return {
        state: 'ready',
        valid: true,
        charging: false,
        discharging: true,
        balancing: false,

        soc,
        soh,
        packVoltage,
        packCurrent: current,
        packPower: packVoltage * current,
        packDelta: deltaVoltage,

        cells,
        minCell: minVoltage,
        maxCell: maxVoltage,
        cellDelta: deltaVoltage,

        temperature,
        tempMin: temperature,
        tempMax: temperature,

        mosfet: {
            charge: false,
            discharge: true,
            balance: false,
        },

        fault,

        system: {
            uptimeMs,
            loops: BMSState.loopCount,
            heartbeat: true,
            faultCount: 1,
        },

        timestamp: now,
    };
}

/**
 * Cria um evento de falha de demonstração.
 */
function createDemoFault(timestamp, index) {
    const faults = [
        { name: 'CELL_OVERVOLTAGE', code: 0x0101, cell: 1, value: 4.30, limit: 4.25, severity: 'CRITICAL' },
        { name: 'OVERTEMPERATURE', code: 0x0103, cell: 0xFF, value: 63.0, limit: 60.0, severity: 'CRITICAL' },
        { name: 'UNDERVOLTAGE', code: 0x0102, cell: 2, value: 2.95, limit: 3.00, severity: 'CRITICAL' },
    ];
    const f = faults[index % faults.length];
    return {
        id: index + 1,
        timestamp,
        name: f.name,
        code: f.code,
        cell: f.cell,
        value: f.value,
        limit: f.limit,
        severity: f.severity,
    };
}

/**
 * Inicializa o histórico de falhas (demo).
 */
function initDemoHistory() {
    const now = Date.now();
    const history = [];
    for (let i = 0; i < 5; i++) {
        history.push(createDemoFault(now - (i + 1) * 60000, 5 - i));
    }
    BMSState.history = history;
}

/**
 * Busca dados reais da API (produção).
 */
async function fetchFromAPI() {
    try {
        const response = await fetch(BMS_API_URL);
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        const json = await response.json();
        BMSState.data = json.data;
        if (json.history) {
            BMSState.history = json.history;
        }
        BMSState.heartbeat.alive = true;
        BMSState.heartbeat.lastBeat = Date.now();
        notifyBMSData();
    } catch (err) {
        console.warn('Falha ao buscar dados do BMS:', err);
        BMSState.heartbeat.alive = false;
        notifyBMSData();
    }
}

/**
 * Loop de dados de demonstração.
 */
function startDemoLoop() {
    initDemoHistory();
    BMSState.timerDemo = setInterval(() => {
        BMSState.data = generateDemoData();
        BMSState.heartbeat = { alive: true, lastBeat: Date.now() };
        notifyBMSData();
    }, BMS_API_INTERVAL_MS);
}

/**
 * Loop de dados da API (produção).
 */
function startApiLoop() {
    BMSState.timerApi = setInterval(fetchFromAPI, BMS_API_INTERVAL_MS);
    fetchFromAPI();
}

/**
 * Inicializa a fonte de dados conforme a configuração.
 */
function initDataSource() {
    if (BMS_DATA_SOURCE === 'api') {
        startApiLoop();
    } else {
        startDemoLoop();
    }
}