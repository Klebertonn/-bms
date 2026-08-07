#!/usr/bin/env python3
"""
BMS Test Report Generator
=========================
Gera um relatório HTML consolidado a partir dos resultados dos testes
unitários e de integração.

Uso:
    python test/generate_report.py

Pré-requisito: ter executado `pio test -e native` para gerar os arquivos
de saída dos testes (test/build/...). Se os arquivos não existirem,
o relatório será gerado com marcadores de "não executado".
"""

import os
import re
import datetime
import html

# Diretório onde o PlatformIO coloca os artefatos de teste
BUILD_DIR = os.path.join(os.path.dirname(__file__), 'build')

# Arquivos de saída de teste procurados
REPORT_PATTERNS = [
    'test_unit_fault.txt',
    'test_unit_storage.txt',
    'test_unit_scheduler.txt',
    'test_unit_state_machine.txt',
    'test_unit_can.txt',
    'test_unit_ble.txt',
    'test_unit_eeprom_flash.txt',
    'test_unit_self_test.txt',
    'test_integration.txt',
]


def find_test_outputs():
    """Busca arquivos de saída dos testes."""
    results = {}
    if not os.path.isdir(BUILD_DIR):
        return results

    for root, _, files in os.walk(BUILD_DIR):
        for fname in files:
            if fname.endswith('.txt'):
                path = os.path.join(root, fname)
                for pattern in REPORT_PATTERNS:
                    base = pattern.replace('.txt', '')
                    if base in fname:
                        results[base] = path
    return results


def parse_unity_output(path):
    """Extrai estatísticas de um arquivo de saída Unity."""
    if not path or not os.path.isfile(path):
        return None

    with open(path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    summary = {
        'tests': 0,
        'pass': 0,
        'fail': 0,
        'ignore': 0,
        'raw': content,
    }

    # Ex.: "4 Tests 4 Failures 0 Ignored"
    m = re.search(r'(\d+)\s+Tests\s+(\d+)\s+Failures\s+(\d+)\s+Ignored', content)
    if m:
        summary['tests'] = int(m.group(1))
        summary['fail'] = int(m.group(2))
        summary['ignore'] = int(m.group(3))
        summary['pass'] = summary['tests'] - summary['fail']
    else:
        # Formato alternativo
        m2 = re.search(r'(\d+)\s+Test\w*\s+[-]?\d+\s+Failures', content)
        if m2:
            summary['tests'] = int(m2.group(1))

    return summary


def build_html(results):
    """Constrói o HTML do relatório."""
    now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    total_tests = 0
    total_pass = 0
    total_fail = 0

    rows = []
    for pattern in REPORT_PATTERNS:
        base = pattern.replace('.txt', '')
        path = results.get(base)
        summary = parse_unity_output(path)

        if summary is None:
            rows.append(f"""
                <tr>
                    <td>{html.escape(base)}</td>
                    <td><span class="badge badge--pending">NÃO EXECUTADO</span></td>
                    <td>—</td><td>—</td><td>—</td><td>—</td>
                </tr>""")
            continue

        total_tests += summary['tests']
        total_pass += summary['pass']
        total_fail += summary['fail']

        status = 'PASS' if summary['fail'] == 0 else 'FAIL'
        badge_class = 'badge--pass' if summary['fail'] == 0 else 'badge--fail'
        rows.append(f"""
            <tr>
                <td>{html.escape(base)}</td>
                <td><span class="badge {badge_class}">{status}</span></td>
                <td>{summary['tests']}</td>
                <td>{summary['pass']}</td>
                <td>{summary['fail']}</td>
                <td>{summary['ignore']}</td>
            </tr>""")

    overall = 'PASS' if total_fail == 0 and total_tests > 0 else 'FAIL'
    overall_class = 'badge--pass' if overall == 'PASS' else 'badge--fail'

    html_doc = f"""<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>BMS — Relatório de Testes</title>
<style>
    * {{ margin: 0; padding: 0; box-sizing: border-box; }}
    body {{
        font-family: 'Segoe UI', system-ui, sans-serif;
        background: #0f172a; color: #e2e8f0;
        padding: 2rem; line-height: 1.5;
    }}
    .container {{ max-width: 900px; margin: 0 auto; }}
    h1 {{ margin-bottom: 0.5rem; }}
    .meta {{ color: #94a3b8; margin-bottom: 2rem; }}
    .summary {{
        display: flex; gap: 1rem; flex-wrap: wrap; margin-bottom: 2rem;
    }}
    .summary-box {{
        background: #1e293b; border-radius: 12px; padding: 1.25rem;
        flex: 1; min-width: 140px; text-align: center;
    }}
    .summary-box .num {{ font-size: 2rem; font-weight: 700; }}
    .summary-box .lbl {{ color: #94a3b8; font-size: 0.85rem; }}
    table {{
        width: 100%; border-collapse: collapse; background: #1e293b;
        border-radius: 12px; overflow: hidden;
    }}
    th, td {{ padding: 0.75rem 1rem; text-align: left; border-bottom: 1px solid #334155; }}
    th {{ color: #94a3b8; text-transform: uppercase; font-size: 0.8rem; }}
    .badge {{ padding: 0.25rem 0.75rem; border-radius: 999px; font-weight: 700; font-size: 0.8rem; }}
    .badge--pass {{ background: #34d399; color: #0f172a; }}
    .badge--fail {{ background: #f87171; color: #0f172a; }}
    .badge--pending {{ background: #94a3b8; color: #0f172a; }}
    .footer {{ margin-top: 2rem; color: #94a3b8; font-size: 0.85rem; }}
    @media (max-width: 600px) {{
        body {{ padding: 1rem; }}
        .summary {{ flex-direction: column; }}
    }}
</style>
</head>
<body>
<div class="container">
    <h1>⚡ BMS — Relatório de Testes</h1>
    <p class="meta">Gerado em {now}</p>

    <div class="summary">
        <div class="summary-box"><div class="num">{total_tests}</div><div class="lbl">Testes</div></div>
        <div class="summary-box" style="color:#34d399"><div class="num">{total_pass}</div><div class="lbl">Passaram</div></div>
        <div class="summary-box" style="color:#f87171"><div class="num">{total_fail}</div><div class="lbl">Falharam</div></div>
        <div class="summary-box"><div class="num">{(total_pass / total_tests * 100 if total_tests else 0):.1f}%</div><div class="lbl">Aprovação</div></div>
    </div>

    <table>
        <thead>
            <tr><th>Suíte</th><th>Status</th><th>Total</th><th>PASS</th><th>FAIL</th><th>Ignorados</th></tr>
        </thead>
        <tbody>
            {''.join(rows)}
        </tbody>
    </table>

    <div class="footer">
        Resultado geral: <span class="badge {overall_class}">{overall}</span>
        — Plataforma native (PlatformIO Test).
    </div>
</div>
</body>
</html>"""

    return html_doc


def main():
    results = find_test_outputs()
    html_doc = build_html(results)

    out_dir = os.path.join(os.path.dirname(__file__), 'report')
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, 'index.html')

    with open(out_path, 'w', encoding='utf-8') as f:
        f.write(html_doc)

    print(f'Relatório gerado: {out_path}')


if __name__ == '__main__':
    main()
