# TODO — Sprint 4.1.4 (Passo 1)

- [x] Adicionar FaultStorage ao App
  - [ ] Editar `src/app/app.h`: incluir `fault_storage.h` e adicionar `FaultStorage faultStorage_{};`

- [ ] Carregar histórico no boot
  - [ ] Editar `src/app/app.cpp`: atualizar `App::init()` com:
    - `faultStorage_.init();`
    - `std::vector<FaultInfo> persistedHistory; faultStorage_.load(persistedHistory);`
    - copiar para `faultHistory_` com `faultHistory_.push(fault)`
    - prints: Loading / History loaded / Events found: X ou No previous events.
- [ ] Compilar e validar
  - [ ] Rodar 2 vezes e validar mensagens esperadas

