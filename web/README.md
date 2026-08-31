# BMS Dashboard Web

Dashboard web responsivo para monitoramento do Battery Management System (BMS).

## Estrutura

```
web/
├── index.html          # Página principal
├── css/
│   └── style.css       # Estilos responsivos (dark theme)
├── js/
│   ├── data-source.js  # Fonte de dados (demo ou API real)
│   ├── charts.js       # Gráficos (Chart.js)
│   └── dashboard.js    # Lógica de renderização
└── README.md
```

## Funcionalidades

- **SOC** — State of Charge com barra de progresso e indicador de saúde
- **SOH** — State of Health
- **Pack** — Tensão total, delta entre células, corrente e potência
- **Temperatura** — Média, mínima e máxima
- **Fault** — Falha ativa (nome, código, célula, valor, limite)
- **Histórico** — Tabela de eventos de falha
- **Estado** — Badge com estado do BMS (BOOT/INIT/READY/CHARGING/FAULT/etc.)
- **Heartbeat** — Indicador de firmware vivo (pulso)
- **Gráficos** — Tensão, corrente, SOC e temperatura em tempo real (Chart.js)

## Como executar

### Simulação local em paralelo com o BMS

Para iniciar a simulação do firmware e abrir o dashboard web ao mesmo tempo, execute:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\start_bms_simulation.ps1
```

Esse script:
- executa o binário nativo do BMS em `.\.pio\build\native\program.exe`
- abre automaticamente o dashboard em `web\index.html`
- mantém o fluxo de simulação e o painel web sincronizados em modo local

### Modo demo (padrão)

Basta abrir o `index.html` no navegador. Os dados são simulados em tempo real.

### Modo produção (hardware real)

1. Abra `js/data-source.js`.
2. Altere `BMS_DATA_SOURCE` de `'demo'` para `'api'`.
3. Ajuste `BMS_API_URL` para o endpoint do BMS (ex.: ESP32).
   ```js
   const BMS_DATA_SOURCE = 'api';
   const BMS_API_URL = 'http://192.168.4.1/api/bms';
   ```
4. O endpoint deve retornar JSON no formato:
   ```json
   {
     "data": {
       "state": "charging",
       "valid": true,
       "charging": true,
       "discharging": false,
       "balancing": false,
       "soc": 78,
       "soh": 96,
       "packVoltage": 12.4,
       "packCurrent": 2.1,
       "packPower": 26.0,
       "packDelta": 0.035,
       "temperature": 26.5,
       "tempMin": 25.9,
       "tempMax": 27.1,
       "fault": {
         "active": false,
         "name": "Nenhuma",
         "code": 0,
         "cell": 255,
         "value": 0,
         "limit": 0
       },
       "timestamp": 1710000000000,
       "uptime": 3600
     },
     "history": [
       {
         "id": 1,
         "timestamp": 1710000000000,
         "name": "CELL_OVERVOLTAGE",
         "code": 257,
         "cell": 0,
         "value": 4.31,
         "limit": 4.25
       }
     ]
   }
   ```

## Tecnologias

- HTML5 / CSS3 (CSS Grid + Flexbox, responsivo)
- JavaScript (vanilla ES6+)
- Chart.js 4.x (CDN)

## Requisitos

- Navegador moderno
- Conexão com internet para o CDN do Chart.js (ou arquivo local)

