# Política de Segurança

Este documento descreve o processo para reportar vulnerabilidades de segurança no firmware BMS.

## Versões Suportadas

| Versão | Suportada |
|--------|-----------|
| 1.0.x  | ✅ |
| 0.x    | ❌ |

## Reportando uma Vulnerabilidade

Se você descobrir uma vulnerabilidade de segurança, **não abra uma issue pública**. Em vez disso, entre em contato diretamente pelo meio de contato do mantenedor do projeto.

**Ao reportar, inclua:**
- Descrição da vulnerabilidade.
- Passos para reprodução.
- Impacto potencial.
- Sugestão de mitigação (se houver).

## Política de Divulgação

- Confirmaremos o recebimento em até 5 dias úteis.
- Trabalharemos numa correção e divulgaremos quando estiver pronta.
- A divulgação pública será feita após a correção estar disponível.

## Práticas de Segurança do Firmware

- **Self Test (POST)** no boot valida os periféricos antes de entrar em `READY`.
- **Proteções** (over/under voltage, current, temperature) desligam MOSFETs de forma segura.
- **Fault Manager** registra eventos de falha de forma persistente.
- **Watchdog** detecta travamentos e reinicia o sistema de forma segura.
- **HAL** impede acesso direto da lógica de negócio aos periféricos (menor superfície de ataque).

**Nota:** Este firmware é destinado a sistemas de gestão de bateria. A segurança elétrica (proteção de alta corrente/voltagem) deve ser validada por engenharia de hardware e certificações aplicáveis.
