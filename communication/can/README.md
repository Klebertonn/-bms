# CAN Manager (Communication)

Módulo responsável por comunicação CAN do BMS.

## Responsabilidades
- Inicializar barramento CAN
- Enviar mensagens periódicas (heartbeat, status, medições)
- Receber comandos externos
- Gerenciar filtros/máscaras (placeholder)
- Detectar erros de comunicação (placeholder)
- Publicar dados do BMS (placeholder)
- Fornecer interface para outros módulos (via getters/serviços futuros)

## Status
- Implementação inicial com payloads zerados (stubs) e heartbeat funcional.
- Próximos passos: integrar com Battery/Protection/SOC/Temperature/Current managers e preencher payloads.

