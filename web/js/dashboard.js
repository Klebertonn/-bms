/* ==========================================================
 * BMS Dashboard — Lógica principal de renderização
 * ========================================================== */

/**
 * Atualiza o heartbeat (dot + label).
 */
function renderHeartbeat(heartbeat) {
    const dot = document.getElementById('heartbeatDot');
    const label = document.getElementById('heartbeatLabel');

    if (heartbeat && heartbeat.alive) {
        dot.className = 'heartbeat__dot heartbeat__dot--alive';
        label.textContent = 'Firmware vivo';
    } else {
        dot.className = 'heartbeat__dot heartbeat__dot--dead';
        label.textContent = 'Sem sinal';
    }

    document.getElementById('lastUpdate').textContent =
        'Última atualização: ' + new Date().toLocaleTimeString('pt-BR');
}

/**
 * Atualiza o cartão de estado do sistema.
 */
function renderState(data) {
    const badge = document.getElementById('stateBadge');
    const stateMap = {
        boot: ['BOOT', 'badge--idle'],
        init: ['INIT', 'badge--idle'],
        ready: ['READY', 'badge--ready'],
        idle: ['IDLE', 'badge--idle'],
        charging: ['CHARGING', 'badge--charging'],
        discharging: ['DISCHARGING', 'badge--discharge'],
        balancing: ['BALANCING', 'badge--balance'],
        fault: ['FAULT', 'badge--fault'],
        shutdown: ['SHUTDOWN', 'badge--idle'],
    };

    const key = (data.state || 'idle').toLowerCase();
    const entry = stateMap[key] || ['UNKNOWN', 'badge--idle'];

    badge.textContent = entry[0];
    // Remove classes antigas e aplica a nova (preserva base badge)
    badge.className = 'badge ' + entry[1];

    const boolText = (v) => (v ? 'SIM' : 'NÃO');
    document.getElementById('flagCharging').textContent = `Carregando: ${boolText(data.charging)}`;
    document.getElementById('flagDischarging').textContent = `Descarregando: ${boolText(data.discharging)}`;
    document.getElementById('flagBalancing').textContent = `Balanço: ${boolText(data.balancing)}`;
    document.getElementById('flagValid').textContent = `Pack Válido: ${boolText(data.valid)}`;
}

/**
 * Atualiza os cards de métricas.
 */
function renderMetrics(data) {
    // SOC
    const soc = Math.max(0, Math.min(100, data.soc));
    document.getElementById('socValue').textContent = `${soc}%`;
    document.getElementById('socBar').style.width = `${soc}%`;
    document.getElementById('socBar').style.background =
        soc > 60 ? 'var(--color-soc)' : soc > 25 ? 'var(--color-warning)' : 'var(--color-danger)';
    document.getElementById('socHint').textContent =
        soc > 60 ? 'Bateria saudável' : soc > 25 ? 'Nível moderado' : 'Bateria baixa';

    // SOH
    const soh = data.soh;
    document.getElementById('sohValue').textContent = `${soh}%`;
    document.getElementById('sohValue').style.color =
        soh > 80 ? 'var(--color-success)' : soh > 50 ? 'var(--color-warning)' : 'var(--color-danger)';
    document.getElementById('sohHint').textContent =
        soh > 80 ? 'Boas condições' : soh > 50 ? 'Desgaste moderado' : 'Atenção — substituir';

    // Tensão do pack
    document.getElementById('packVoltage').textContent = `${data.packVoltage.toFixed(2)} V`;
    document.getElementById('packDelta').textContent = `ΔV: ${(data.packDelta * 1000).toFixed(0)} mV`;

    // Corrente
    document.getElementById('packCurrent').textContent = `${data.packCurrent.toFixed(2)} A`;
    document.getElementById('packPower').textContent = `Potência: ${data.packPower.toFixed(1)} W`;

    // Temperatura
    document.getElementById('packTemp').textContent = `${data.temperature.toFixed(1)} °C`;
    document.getElementById('tempRange').textContent =
        `Min: ${data.tempMin.toFixed(1)} °C / Máx: ${data.tempMax.toFixed(1)} °C`;
}

/**
 * Renderiza as células do pack.
 */
function renderCells(data) {
    const grid = document.getElementById('cellsGrid');
    if (!grid) return;

    const cells = data.cells || [];
    const staticLabels = ['Cell 1', 'Cell 2', 'Cell 3', 'Menor', 'Maior', 'Delta'];

    if (!grid.dataset.initialized) {
        const html = staticLabels.map((label, index) => {
            const extraClass = index >= 3 ? 'cell--info' : '';
            return `
                <div class="cell-item ${extraClass}">
                    <span class="cell__label">${label}</span>
                    <span class="cell__voltage">--</span>
                    <span class="cell__status">--</span>
                </div>`;
        }).join('');

        grid.innerHTML = html;
        grid.dataset.initialized = 'true';
    }

    const items = grid.querySelectorAll('.cell-item');
    cells.forEach((cell, i) => {
        const item = items[i];
        if (!item) return;

        const isOver = cell.voltage > 4.25;
        const isUnder = cell.voltage < 3.00;
        const statusClass = isOver ? 'cell--over' : isUnder ? 'cell--under' : 'cell--ok';
        const statusText = isOver ? 'SOBRETENSÃO' : isUnder ? 'SUBTENSÃO' : 'OK';

        item.className = `cell-item ${statusClass}`;
        item.querySelector('.cell__voltage').textContent = `${cell.voltage.toFixed(3)} V`;
        item.querySelector('.cell__status').textContent = statusText;
    });

    const minItem = items[3];
    const maxItem = items[4];
    const deltaItem = items[5];

    if (minItem) {
        minItem.className = 'cell-item cell--info';
        minItem.querySelector('.cell__voltage').textContent = `${data.minCell.toFixed(3)} V`;
        minItem.querySelector('.cell__status').textContent = 'MIN';
    }

    if (maxItem) {
        maxItem.className = 'cell-item cell--info';
        maxItem.querySelector('.cell__voltage').textContent = `${data.maxCell.toFixed(3)} V`;
        maxItem.querySelector('.cell__status').textContent = 'MAX';
    }

    if (deltaItem) {
        deltaItem.className = 'cell-item cell--info';
        deltaItem.querySelector('.cell__voltage').textContent = `${(data.cellDelta * 1000).toFixed(0)} mV`;
        deltaItem.querySelector('.cell__status').textContent = 'ΔV';
    }
}

/**
 * Renderiza o estado dos MOSFETs.
 */
function renderMosfet(data) {
    const mosfet = data.mosfet || {};

    const setMosfet = (id, enabled) => {
        const el = document.getElementById(id);
        if (!el) return;
        const status = el.querySelector('.mosfet__status');
        status.textContent = enabled ? 'ON' : 'OFF';
        status.className = 'mosfet__status ' + (enabled ? 'mosfet__status--on' : 'mosfet__status--off');
    };

    setMosfet('mosfetCharge', mosfet.charge);
    setMosfet('mosfetDischarge', mosfet.discharge);
    setMosfet('mosfetBalance', mosfet.balance);
}

/**
 * Renderiza as informações do sistema.
 */
function renderSystem(data) {
    const sys = data.system || {};

    // Uptime formatado
    const uptimeMs = sys.uptimeMs || 0;
    const secs = Math.floor(uptimeMs / 1000);
    const h = Math.floor(secs / 3600);
    const m = Math.floor((secs % 3600) / 60);
    const s = secs % 60;
    const uptimeStr = `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;

    document.getElementById('sysUptime').textContent = uptimeStr;
    document.getElementById('sysLoops').textContent = (sys.loops || 0).toLocaleString('pt-BR');
    document.getElementById('sysHeartbeat').textContent = sys.heartbeat ? 'ALIVE' : 'DEAD';
    document.getElementById('sysHeartbeat').style.color =
        sys.heartbeat ? 'var(--color-success)' : 'var(--color-danger)';
    document.getElementById('sysFaults').textContent = sys.faultCount || 0;
}

/**
 * Atualiza o cartão de fault ativa.
 */
function renderFault(data) {
    const f = data.fault || {};
    const valueEl = document.getElementById('faultValue');
    const infoEl = document.getElementById('faultInfo');

    if (f.active) {
        valueEl.textContent = f.name || 'FALHA';
        valueEl.style.color = 'var(--color-danger)';
        infoEl.textContent =
            `Cód 0x${(f.code || 0).toString(16).padStart(4, '0')} · Sev ${f.severity || '—'} · ` +
            `Célula ${f.cell === 0xFF ? 'Pack' : f.cell} · ` +
            `${f.value != null ? f.value : '—'} / ${f.limit != null ? f.limit : '—'}`;
    } else {
        valueEl.textContent = 'Nenhuma';
        valueEl.style.color = 'var(--color-success)';
        infoEl.textContent = 'Sem falha ativa';
    }
}

/**
 * Renderiza dados simulados no formato esperado (normaliza se necessário).
 */
function renderTable(data) {
    // Sem ação extra além do histórico
    void data;
}

/**
 * Renderiza o histórico de falhas na tabela.
 */
function renderHistory(history) {
    const tbody = document.getElementById('faultTableBody');
    const countEl = document.getElementById('faultCount');

    countEl.textContent = history.length === 0
        ? '0 eventos'
        : `${history.length} evento(s)`;

    if (!history || history.length === 0) {
        tbody.innerHTML = '<tr><td colspan="7" class="table__empty">Nenhum evento registrado</td></tr>';
        return;
    }

    let html = '';
    history.forEach((item) => {
        const cell = item.cell === 0xFF ? 'Pack' : `Célula ${item.cell}`;
        html += `
            <tr>
                <td>${item.id}</td>
                <td>${formatTimestamp(item.timestamp)}</td>
                <td>${item.name}</td>
                <td>0x${(item.code || 0).toString(16).padStart(4, '0')}</td>
                <td>${cell}</td>
                <td>${item.value != null ? item.value : '—'}</td>
                <td>${item.limit != null ? item.limit : '—'}</td>
            </tr>`;
    });
    tbody.innerHTML = html;
}

/**
 * Formata timestamp (ms ou s) para data/hora local.
 */
function formatTimestamp(ts) {
    if (!ts) return '—';
    // Se timestamp for em segundos (< 10 dígitos), converte para ms
    const ms = ts < 1e12 ? ts * 1000 : ts;
    const d = new Date(ms);
    if (isNaN(d.getTime())) return '—';
    return d.toLocaleString('pt-BR');
}

/**
 * Callback principal: recebe novos dados e atualiza a UI.
 */
function onData(data, history, heartbeat) {
    if (!data) return;

    renderHeartbeat(heartbeat);
    renderState(data);
    renderMetrics(data);
    renderCells(data);
    renderMosfet(data);
    renderSystem(data);
    renderFault(data);
    renderHistory(history || []);
    renderTable(data);
    feedCharts(data);
}

/**
 * Inicialização do dashboard.
 */
function initDashboard() {
    initCharts();
    onBMSData(onData);
    initDataSource();
}

// Aguarda o DOM carregar antes de inicializar.
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initDashboard);
} else {
    initDashboard();
}