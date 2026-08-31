/* ==========================================================
 * BMS Dashboard — Configuração dos Gráficos (Chart.js)
 * ========================================================== */

const MAX_POINTS = 30; // Número de pontos mantidos no histórico dos gráficos

// Buffers de histórico por gráfico
const ChartBuffer = {
    voltage: [],
    current: [],
    soc: [],
    temp: [],
    labels: [],
};

// Instâncias dos gráficos
const Charts = {
    voltage: null,
    current: null,
    soc: null,
    temp: null,
};

/**
 * Tema base dos gráficos (dark mode).
 */
const chartTheme = {
    color: '#e2e8f0',
    grid: 'rgba(148, 163, 184, 0.15)',
};

/**
 * Cria um gráfico de linha com configuração padrão.
 */
function createLineChart(ctxId, label, borderColor, fillColor, unit) {
    const ctx = document.getElementById(ctxId);
    if (!ctx) return null;

    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: `${label} (${unit})`,
                data: [],
                borderColor,
                backgroundColor: fillColor,
                borderWidth: 2,
                fill: true,
                tension: 0.3,
                pointRadius: 2,
            }],
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: false,
            plugins: {
                legend: { display: false },
            },
            scales: {
                x: {
                    ticks: { color: chartTheme.color, maxTicksLimit: 6 },
                    grid: { color: chartTheme.grid },
                },
                y: {
                    ticks: { color: chartTheme.color },
                    grid: { color: chartTheme.grid },
                },
            },
        },
    });
}

/**
 * Inicializa todos os gráficos.
 */
function initCharts() {
    Charts.voltage = createLineChart('voltageChart', 'Tensão', '#38bdf8', 'rgba(56, 189, 248, 0.15)', 'V');
    Charts.current = createLineChart('currentChart', 'Corrente', '#34d399', 'rgba(52, 211, 153, 0.15)', 'A');
    Charts.soc     = createLineChart('socChart', 'SOC', '#22c55e', 'rgba(34, 197, 94, 0.15)', '%');
    Charts.temp    = createLineChart('tempChart', 'Temperatura', '#f97316', 'rgba(249, 115, 22, 0.15)', '°C');
}

/**
 * Adiciona um ponto a um buffer de gráfico (com limite de tamanho).
 */
function pushPoint(buffer, value) {
    buffer.push(value);
    if (buffer.length > MAX_POINTS) {
        buffer.shift();
    }
}

/**
 * Atualiza um gráfico com os buffers atuais.
 */
function updateChart(chart, buffer, labels) {
    if (!chart) return;
    chart.data.labels = labels.slice();
    chart.data.datasets[0].data = buffer.slice();
    chart.update();
}

/**
 * Alimenta os gráficos com o snapshot mais recente.
 */
function feedCharts(data) {
    if (!data) return;

    const time = new Date(data.timestamp).toLocaleTimeString('pt-BR');

    pushPoint(ChartBuffer.voltage, data.packVoltage);
    pushPoint(ChartBuffer.current, data.packCurrent);
    pushPoint(ChartBuffer.soc, data.soc);
    pushPoint(ChartBuffer.temp, data.temperature);
    pushPoint(ChartBuffer.labels, time);

    updateChart(Charts.voltage, ChartBuffer.voltage, ChartBuffer.labels);
    updateChart(Charts.current, ChartBuffer.current, ChartBuffer.labels);
    updateChart(Charts.soc, ChartBuffer.soc, ChartBuffer.labels);
    updateChart(Charts.temp, ChartBuffer.temp, ChartBuffer.labels);
}
