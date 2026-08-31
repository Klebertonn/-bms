# Processo de Release

Este documento descreve o processo para criar uma release do firmware BMS.

## Versionamento

Usamos [SemVer](https://semver.org/lang/pt-BR/):

- **MAJOR** — mudança incompatível (breaking change).
- **MINOR** — nova funcionalidade compatível.
- **PATCH** — correção compatível.

Formato: `vX.Y.Z` (ex.: `v1.0.0`).

---

## Passos para Criar uma Release

### 1. Atualizar o Changelog

- Adicione a nova versão no topo de [`CHANGELOG.md`](../CHANGELOG.md).
- Categorize em: `Adicionado`, `Melhorado`, `Corrigido`, `Removido`.

### 2. Validar Build e Testes

```bash
# Build native
pio run -e native

# Testes
pio test -e native
python test/generate_report.py

# Build ESP32 (se plataforma disponível)
pio run -e esp32dev
```

Todos devem passar.

### 3. Atualizar Checkout Industrial

- [ ] Marcar itens concluídos em [`PRODUCTION.md`](PRODUCTION.md).
- [ ] Confirmar que os critérios críticos estão 100%.

### 4. Criar Tag e Release

```bash
# Crie a tag
git tag -a v1.0.0 -m "Release 1.0.0"

# Envie a tag
git push origin v1.0.0

# Crie a release no GitHub
# gh release create v1.0.0 --title "BMS 1.0.0" --notes-file CHANGELOG.md
```

### 5. Notas da Release

Inclua:
- Resumo das mudanças.
- Referência ao changelog.
- Artefatos de build (firmware `.bin` para ESP32, se aplicável).
- Checklist de validação.

---

## Hotfix

Para correções urgentes:

1. Crie branch `fix/...` a partir da tag.
2. Aplique a correção.
3. Incremente o `PATCH` (ex.: `v1.0.1`).
4. Siga os passos de release acima.

---

## Referência

- [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/)
- [SemVer](https://semver.org/lang/pt-BR/)
