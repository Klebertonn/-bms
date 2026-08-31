# ICD-019 – Security Manager API

**Projeto:** BMS Firmware ESP32

**Versão:** 1.0.0

**Status:** Interface Pública

**Dependências:** ICD-006, ICD-010, ICD-012, ICD-013, ICD-014, ICD-016, ICD-018

---

# 1. Objetivo

O Security Manager é responsável por proteger o firmware do BMS contra acessos não autorizados, alterações indevidas de configuração e atualizações maliciosas.

O módulo fornece autenticação, autorização, criptografia, gerenciamento de chaves e auditoria de segurança.

---

# 2. Responsabilidades

O Security Manager deverá:

* Autenticar usuários e dispositivos.
* Autorizar operações conforme o nível de acesso.
* Gerenciar credenciais e chaves criptográficas.
* Validar assinaturas digitais em atualizações OTA.
* Registrar eventos de segurança.
* Bloquear tentativas repetidas de acesso inválido.

---

# 3. Níveis de Acesso

## SecurityAccessLevel

```cpp id="sec001"
enum class SecurityAccessLevel
{
    GUEST,
    OPERATOR,
    ENGINEER,
    ADMIN
};
```

---

# 4. Estruturas

## SecuritySession

```cpp id="sec002"
struct SecuritySession
{
    SecurityAccessLevel level;
    bool authenticated;
    uint32_t sessionTimeoutMs;
    uint32_t loginAttempts;
};
```

---

## SecurityStatus

```cpp id="sec003"
struct SecurityStatus
{
    bool initialized;
    bool secureBootEnabled;
    bool flashEncryptionEnabled;
    uint32_t failedAuthentications;
    uint32_t activeSessions;
};
```

---

# 5. Interface Pública

## Inicialização

```cpp id="sec004"
bool securityInit();
```

---

## Autenticar

```cpp id="sec005"
bool securityAuthenticate(const char* user,
                          const char* credential);
```

---

## Encerrar Sessão

```cpp id="sec006"
bool securityLogout();
```

---

## Verificar Permissão

```cpp id="sec007"
bool securityHasPermission(SecurityAccessLevel level);
```

---

## Obter Status

```cpp id="sec008"
SecurityStatus securityGetStatus();
```

---

## Alterar Credenciais

```cpp id="sec009"
bool securityChangeCredential(
    const char* current,
    const char* next);
```

---

## Executar Autoteste

```cpp id="sec010"
bool securitySelfTest();
```

---

# 6. Políticas de Segurança

O firmware deverá:

* Exigir autenticação para comandos administrativos.
* Bloquear alterações críticas sem autorização.
* Limitar tentativas consecutivas de login.
* Encerrar sessões inativas automaticamente.
* Registrar todos os eventos relevantes no Logger.

---

# 7. Proteção de Interfaces

## CLI

* Leitura pública.
* Escrita restrita por nível de acesso.

## BLE

* Pareamento seguro.
* Criptografia de comunicação.
* Escrita autenticada.

## CAN

* Validação de comandos críticos.
* Filtragem por identificador.
* Registro de comandos administrativos.

## OTA

* Verificação de assinatura digital.
* Integridade da imagem.
* Bloqueio de firmware não autorizado.

---

# 8. Fluxo de Operação

```text id="sec011"
Solicitação de acesso
        │
        ▼
Autenticação
        │
   ┌────┴────┐
   │         │
Falha     Sucesso
   │         │
   ▼         ▼
Registrar   Criar sessão
evento          │
                ▼
       Validar permissões
                │
                ▼
       Executar operação
```

---

# 9. Integração

O Security Manager interage com:

* CLI Manager.
* BLE Service.
* CAN Protocol.
* OTA Update.
* Logger.
* Configuration Manager.
* Storage Manager.

---

# 10. Tratamento de Erros

Em caso de:

* Credenciais inválidas.
* Sessão expirada.
* Firmware não autenticado.
* Excesso de tentativas.

O sistema deverá:

* Registrar o evento.
* Bloquear a operação.
* Informar o usuário pela interface correspondente.

---

# 11. Requisitos

* Compatível com ESP32 Secure Boot e Flash Encryption (quando habilitados).
* Gerenciamento seguro de credenciais.
* Auditoria completa de eventos.
* Baixo impacto no desempenho.
* Interface extensível para novos mecanismos de autenticação.

---

# 12. Critérios de Aceitação

O módulo será considerado aprovado quando:

* Controlar corretamente os níveis de acesso.
* Proteger comandos administrativos.
* Validar atualizações OTA.
* Registrar eventos de segurança.
* Integrar-se às interfaces BLE, CAN e CLI.

---

# 13. Evoluções Futuras

* Autenticação baseada em certificados.
* Integração com HSM/TPM.
* Autenticação multifator para ferramentas de manutenção.
* Rotação automática de chaves.
* Provisionamento seguro em fábrica.

---

# 14. Controle de Versão

| Versão | Data       | Descrição                                   |
| ------ | ---------- | ------------------------------------------- |
| 1.0.0  | 04/07/2026 | Primeira definição da Security Manager API. |

