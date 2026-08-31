# DDS-010 – Configuration Manager

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Especificação de Implementação

**Dependências:** DDS-001 até DDS-009

---

# 1. Objetivo

O Configuration Manager centraliza todos os parâmetros configuráveis do firmware, permitindo selecionar diferentes perfis de execução e manter um único código-fonte para todos os ambientes suportados.

---

# 2. Objetivos

* Eliminar valores fixos ("hardcoded").
* Permitir perfis de compilação.
* Facilitar manutenção.
* Garantir consistência entre módulos.
* Possibilitar configuração persistente.

---

# 3. Perfis de Compilação

O firmware deverá suportar os seguintes perfis:

## MOCK

Destinado a testes em computador.

Características:

* Sensores simulados.
* Comunicação simulada.
* Dashboard via console.
* Sem acesso ao hardware.

---

## WOKWI

Destinado à simulação online.

Características:

* GPIO simulados.
* ADC simulados.
* UART para monitor serial.
* Compatível com o ambiente Wokwi.

---

## HARDWARE

Destinado ao ESP32 físico.

Características:

* HAL real.
* Drivers reais.
* Comunicação CAN/TWAI.
* BLE.
* Display OLED.
* Memória NVS.

---

# 4. Organização dos Arquivos

Estrutura sugerida:

```text
config/
├── build_config.h
├── battery_config.h
├── protection_config.h
├── balance_config.h
├── communication_config.h
├── logger_config.h
├── display_config.h
├── storage_config.h
└── hardware_config.h
```

Cada arquivo será responsável por um conjunto específico de parâmetros.

---

# 5. Configuração da Bateria

Parâmetros:

* Número de células.
* Capacidade nominal.
* Química da bateria.
* Tensão nominal.
* Tensão máxima.
* Tensão mínima.
* Corrente máxima de carga.
* Corrente máxima de descarga.

---

# 6. Configuração das Proteções

Parâmetros:

* OVP.
* UVP.
* OCP.
* SCP.
* OTP.
* UTP.
* Histerese.
* Tempo de confirmação.

---

# 7. Configuração do Balanceamento

Parâmetros:

* Delta para iniciar.
* Delta para finalizar.
* Número máximo de células simultâneas.
* Tempo máximo de balanceamento.
* Estratégia (Sequencial, Round Robin ou Simultâneo).

---

# 8. Configuração da Comunicação

Parâmetros:

* Baud rate UART.
* Bitrate CAN.
* Nome BLE.
* UUIDs BLE.
* Intervalos de atualização.
* Endereço I²C do OLED.

---

# 9. Configuração do Logger

Parâmetros:

* Nível mínimo de log.
* Frequência do dashboard.
* Quantidade máxima de registros.
* Persistência habilitada.
* Formato de exportação.

---

# 10. Configuração do Display

Parâmetros:

* Brilho.
* Tempo entre telas.
* Modo de rotação.
* Telas habilitadas.

---

# 11. Configuração de Armazenamento

Itens persistentes:

* Configurações do usuário.
* Limites personalizados.
* Histórico de falhas.
* Estatísticas.
* Contador de ciclos.

Armazenamento previsto:

* NVS (ESP32).
* EEPROM (quando aplicável).

---

# 12. Validação

Todas as configurações deverão ser verificadas durante a inicialização.

Exemplos:

* Tensão máxima > tensão mínima.
* Corrente máxima positiva.
* Delta de balanceamento válido.
* Endereços de comunicação sem conflito.

Configurações inválidas deverão impedir a inicialização normal do sistema e gerar log de erro.

---

# 13. Interface de Configuração

As configurações poderão ser alteradas por:

* CLI.
* BLE.
* CAN (quando autorizado).
* Arquivo de configuração (Mock/Wokwi).
* Atualização de firmware (futuro).

---

# 14. Integração

O Configuration Manager fornecerá parâmetros para todos os módulos:

* Battery Manager.
* Protection Manager.
* Balance Manager.
* Communication Manager.
* Logger.
* Display.
* Storage.
* Task Manager.

---

# 15. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Os perfis de compilação funcionarem corretamente.
* Os parâmetros forem carregados sem inconsistências.
* Alterações persistentes forem recuperadas após reinicialização.
* O firmware operar corretamente em Mock, Wokwi e ESP32 físico sem modificações no código-fonte.

---

# 16. Evoluções Futuras

* Perfis dinâmicos.
* Importação/exportação de configurações.
* Criptografia das configurações sensíveis.
* Backup e restauração.
* Interface gráfica de configuração.

---

# 17. Controle de Versão

| Versão | Data       | Descrição                                       |
| ------ | ---------- | ----------------------------------------------- |
| 1.0.0  | 03/07/2026 | Especificação inicial do Configuration Manager. |

