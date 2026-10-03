# Arquitetura consolidada v2

## Fluxo

ECU Meriva -> K-Line -> OBD -> ELM327 Bluetooth -> ESP32 -> TID

O Smart MID é somente leitura. Não transmite comandos de controle para a ECU e não aciona atuadores.

## Base validada no aplicativo

O aplicativo Android `luizmarquesarruda/meriva-smart-diagnostic` passa a ser a referência prática para o comportamento do núcleo OBD do MID.

A ponte de arquitetura é:

`ECU -> K-Line -> OBD -> ELM327 -> camada de transporte -> núcleo OBD -> dados validados`

No aplicativo, a lógica de OBD/PID/DTC fica no TypeScript e o Android nativo funciona como ponte de transporte e integração. No MID, a mesma separação deve ser mantida entre transporte Bluetooth, leitura OBD, validação e interface TID.

Documento principal: `docs/VALIDATED_DIAGNOSTIC_APP_V1_BASE.md`

## Regras herdadas do aplicativo

- Bluetooth Classic/SPP.
- ELM327 real, sem dados simulados no caminho de produção.
- Confirmar sessão por resposta real da ECU.
- Distinguir timeout, erro, ausência de dados e leitura válida.
- Guardar qualidade e idade da leitura.
- Não converter `NO DATA`, `ERROR`, `UNABLE` ou timeout em zero.
- GPS separado da lógica OBD.
- Consumo baseado em fuel rate somente quando o PID `015E` existir e responder de forma real.
- O PID `012F` não é suficiente para confirmar abastecimento na Meriva enquanto a boia estiver imprecisa.

## Dados de campo

Snapshot de 24/09/2026:

- coolant: 81 °C
- STFT B1: -8,59 %
- LTFT B1: +10,94 %
- MAP: 5,66 psi
- RPM: 778
- timing: 8 °
- IAT: 33 °C
- MAF: 8,93 kg/h
- throttle: 3,53 %
- O2S1B1: 0,53 V
- protocolo: ISO 14230-4 KWP fast init
- comunicação: 10,4 kbaud
- ECU address: 11

Esses dados são referência de bancada/veículo e não constantes do firmware.

## Hardware de referência

| Função | GPIO |
|---|---:|
| IGN sense | 35 |
| Buzzer | 26 |
| SD CS | 5 |
| SPI SCK | 18 |
| SPI MISO | 19 |
| SPI MOSI | 23 |
| I2C SDA | 21 |
| I2C SCL | 22 |
| TID SCL | 32 |
| TID SDA | 33 |
| TID MRQ | 27 |
| GNSS RX | 16 |
| GNSS TX | 17 |
| GNSS PPS | 4 |

A pinagem deve ser conferida fisicamente antes da fabricação da PCB. Existem versões históricas de pinagem na documentação.

## Alimentação

Arquitetura prevista: alimentação permanente protegida, IGN como entrada de detecção, espera de aproximadamente 30 s após IGN OFF, gravação dos dados e deep sleep.

## Camadas

- OBD/ELM327
- validação
- trip e horímetro
- combustível e autonomia
- tanque e abastecimento
- DNA estatístico
- display TID
- alertas
- SD
- GNSS/RTC
- interface do telefone

## Ordem de evolução

`aplicativo V1 -> testes reais -> correções -> APK funcionando -> base congelada -> Android Auto V2 -> Smart MID`

A integração Android Auto fica separada do firmware principal do MID e não deve quebrar a base já validada.
