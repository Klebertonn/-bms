# Protection Manager (CORE)

Módulo responsável por proteger a bateria e o hardware contra condições perigosas.

## Funções
- OVP (sobretensão)
- UVP (subtensão)
- OTP (sobretemperatura)
- UTP (subt. de temperatura) (quando aplicável)
- OCC (sobrecorrente de carga)
- OCD (sobrecorrente de descarga)
- SCP (curto-circuito) (placeholder)
- Controle de MOSFETs de carga/descarga (via flags, integração com HAL em etapa seguinte)

## Fluxo
Battery Manager -> PackData -> Protection Manager -> ProtectionState/MOSFET enables

