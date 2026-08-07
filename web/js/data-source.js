/* ==========================================================
 * BMS Dashboard — Fonte de Dados
 * ==========================================================
 * Abstrai a origem dos dados do BMS.
 *
 * Modo 1 (padrão): DEMO — gera dados simulados realistas.
 * Modo 2 (produção): API — busca em endpoint/WebSocket conectado
 *                     ao BMS real (ex.: ESP32 via HTTP/WebSocket).
 *
 * Para conectar ao hardware real, defina BMS_DATA_SOURCE="api"
 * e aponte BMS_API_URL para o seu endpoint.
 * ==========================================================
 */

const BMS_DATA_SOURCE = 'demo'; // 'demo' | 'api'
const BMS_API_URL = 'http://192.168.4.1/api/bms'; // exemplo ESP32 (AP)
const BMS_API_INTERVAL_MS = 1000;

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
 */
function generateDemoData() {
    const now = Date.now();
    const t = now / 1000;

    const soc = Math.round(78 + Math.sin(t / 30) * 3);
    const soh = 96;
    const packVoltage = 12.4 + Math.sin(t / 20) * 0.3;
    const current = 2.1 + Math.sin(t / 15) * 1.2;
    const temp = 26.5 + Math.sin(t / 25) * 0.8;

    return {
        state: 'charging',
        valid: true,
        charging: true,
        discharging: false,
        balancing: soc > 80,

        soc,
        soh,
        packVoltage,
        packCurrent: current,
        packPower: packVoltage * current,
        packDelta: 0.035,

        temperature: temp,
        tempMin: temp - 0.6,
        tempMax: temp + 0.6,

        fault: {
            active: current > 25,
            name: current > 25 ? 'OVERCURRENT' : 'Nenhuma',
            code: current > 25 ? 0x0104 : 0x0000,
            cell: 0xFF,
            value: current,
            limit: 30.0,
        },

        timestamp: now,
        uptime: Math.round(now / 1000),
    };
}

/**
 * Cria um evento de falha de demonstração.
 */
function createDemoFault(timestamp, index) {
    const faults = [
        { name: 'CELL_OVERVOLTAGE', code: 0x0101, cell: 0, value: 4.31, limit: 4.25 },
        { name: 'OVERTEMPERATURE', code: 0x0103, cell: 0xFF, value: 63.0, limit: 60.0 },
        { name: 'UNDERVOLTAGE', code: 0x0102, cell: 1, value: 2.95, limit: 3.00 },
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
    }, 1000);
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
