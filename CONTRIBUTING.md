# Guia de Contribuição

Obrigado por contribuir com o firmware BMS! Este guia descreve como participar do desenvolvimento de forma consistente.

## Fluxo de Contribuição

1. **Fork** o repositório.
2. Crie um **branch** com nome descritivo:
   - `feat/nome-da-funcionalidade`
   - `fix/nome-do-corretivo`
   - `docs/nome-da-documentacao`
3. Faça **commits** pequenos e descritivos.
4. Execute o **build** e os **testes** antes de enviar.
5. Abra um **Pull Request** para o branch principal.

## Convenções de Código

- **Linguagem:** C++.
- **Padrão de nomes:**
  - Classes: `PascalCase` (ex.: `BatteryManager`).
  - Métodos: `camelCase` (ex.: `getPack()`).
  - Variáveis: `camelCase` (ex.: `totalVoltage`).
  - Membros privados: sufixo `_` (ex.: `pack_`).
  - Constantes: `SCREAMING_SNAKE` (ex.: `MAX_CELL_VOLTAGE`).
- **Arquitetura:**
  - Lógica de negócio em `core/` (independe de hardware).
  - Infraestrutura em `system/`.
  - Interfaces de hardware em `lib/hal/`.
  - Drivers específicos de plataforma em `src/hal/`.
- **Comentários:** use `//` para comentários de linha e `/** */` para documentação de classe/métodos.
- **Não acesse periféricos** diretamente da lógica de negócio — sempre via HAL.

## Build e Testes

### Build native (PC/mock)

```bash
pio run -e native
```

### Build ESP32

```bash
pio run -e esp32dev
```

### Testes

```bash
pio test -e native
python test/generate_report.py
```

## Estrutura de Módulos

| Diretório | Conteúdo |
|-----------|----------|
| `core/` | Lógica de negócio. |
| `system/` | Infraestrutura. |
| `communication/` | CAN e BLE. |
| `lib/hal/` | Interfaces de hardware. |
| `src/hal/` | Mocks e drivers. |
| `src/app/` | Camada de aplicação. |
| `test/` | Testes. |
| `docs/` | Documentação. |

## Checklist do Pull Request

- [ ] Build `native` compilou.
- [ ] (Se aplicável) Build `esp32dev` compilou.
- [ ] Testes passaram.
- [ ] Documentação atualizada.
- [ ] Nenhuma funcionalidade existente removida.
- [ ] Código segue as convenções do projeto.

## Reportando Issues

- Use um título descritivo.
- Descreva o comportamento esperado vs. atual.
- Inclua passos de reprodução e logs quando possível.
