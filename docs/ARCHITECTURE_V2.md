# Arquitetura consolidada v2

## Fluxo

ECU Meriva -> K-Line -> OBD -> ELM327 Bluetooth -> ESP32 -> TID

O Smart MID é somente leitura. Não transmite comandos de controle para a ECU e não aciona atuadores.

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
