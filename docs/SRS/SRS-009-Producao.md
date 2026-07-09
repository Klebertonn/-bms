# SRS-009 – Produção, Configuração e Gerenciamento de Builds

**Projeto:** BMS Firmware
**Versão:** 1.0.0
**Status:** Em desenvolvimento
**Dependências:** SRS-001 a SRS-008

---

# 1. Objetivo

Este documento define a estratégia de configuração do firmware para diferentes ambientes de execução, gerenciamento de builds, controle de versões e preparação para produção.

O firmware deverá utilizar uma única base de código para todos os ambientes suportados.

---

# 2. Ambientes Suportados

## Ambiente Mock (PC)

Objetivo:

* Desenvolvimento da lógica do firmware.
* Testes automatizados.
* Simulação de sensores.
* Depuração rápida.

Características:

* Sem ESP32.
* Sem GPIO reais.
* Sensores simulados.
* Comunicação via console.

---

## Ambiente Wokwi

Objetivo:

* Simulação do ESP32.
* Validação da integração com Arduino Framework.
* Testes da interface Serial.
* Testes preliminares antes do hardware físico.

Características:

* ESP32 virtual.
* GPIO simulados.
* Periféricos suportados pela plataforma.

---

## Ambiente Hardware

Objetivo:

* Operação em equipamento real.

Características:

* ESP32 físico.
* Sensores reais.
* CAN.
* BLE.
* OLED.
* MOSFETs.
* EEPROM/NVS.

---

# 3. Perfis de Compilação

Os perfis deverão ser selecionados durante a compilação.

Perfis previstos:

* MOCK
* WOKWI
* HARDWARE

O código não deverá ser modificado manualmente para trocar de perfil.

---

# 4. Configuração Central

Todas as configurações deverão estar concentradas em um módulo dedicado.

Exemplos de parâmetros:

* Número de células.
* Limites de tensão.
* Limites de corrente.
* Limites de temperatura.
* Intervalos das tarefas.
* Velocidade CAN.
* Nome BLE.
* Frequência de atualização do display.

---

# 5. Organização Recomendada

```text id="k8e5hj"
config/
├── config_default.h
├── config_mock.h
├── config_wokwi.h
├── config_hardware.h
└── config_select.h
```

O arquivo `config_select.h` será responsável por selecionar automaticamente a configuração correta conforme o perfil de compilação.

---

# 6. Estrutura HAL

A camada de abstração de hardware deverá isolar toda dependência do ambiente.

Exemplo:

```text id="4zm8qa"
hal/
├── hal_adc.cpp
├── hal_can.cpp
├── hal_ble.cpp
├── hal_display.cpp
├── hal_storage.cpp
├── hal_mock.cpp
└── hal_select.cpp
```

Nenhum módulo de alto nível deverá acessar diretamente APIs específicas do ESP32.

---

# 7. Controle de Versão

O firmware deverá seguir versionamento semântico:

MAJOR.MINOR.PATCH

Exemplos:

* 1.0.0
* 1.1.0
* 1.2.3
* 2.0.0

Toda alteração deverá atualizar:

* Versão.
* Data.
* Histórico de mudanças.

---

# 8. Builds Oficiais

Cada build deverá conter:

* Versão do firmware.
* Data de compilação.
* Commit Git.
* Perfil utilizado.
* Identificação da placa.

Essas informações deverão ser exibidas pela CLI e registradas no boot.

---

# 9. Organização do Repositório

```text id="xv6rpn"
docs/
config/
include/
src/
hal/
drivers/
services/
middleware/
tasks/
storage/
tests/
scripts/
tools/
```

A estrutura deverá ser mantida consistente para facilitar manutenção e escalabilidade.

---

# 10. Requisitos para Produção

Antes de gerar uma versão oficial:

* Compilação sem erros.
* Compilação sem warnings críticos.
* Testes aprovados.
* Documentação atualizada.
* Número de versão atualizado.
* Tag criada no Git.

---

# 11. Critérios de Aceitação

O gerenciamento de produção será considerado aprovado quando:

* O mesmo código-fonte compilar para todos os perfis suportados.
* Não houver necessidade de editar arquivos para trocar de ambiente.
* As configurações estiverem centralizadas.
* As informações da build forem rastreáveis.

---

# 12. Evoluções Futuras

A arquitetura deverá permitir:

* Integração com pipelines de CI/CD.
* Geração automática de releases.
* Assinatura digital do firmware.
* Atualização OTA.
* Geração de pacotes para distribuição.

---

# 13. Controle de Versão

| Versão | Data       | Descrição                                                                            |
| ------ | ---------- | ------------------------------------------------------------------------------------ |
| 1.0.0  | 03/07/2026 | Definição inicial da estratégia de produção, configuração e gerenciamento de builds. |

