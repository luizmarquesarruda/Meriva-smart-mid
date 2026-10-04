# Hardware Rev A

## Referências

A Rev A usa quatro níveis de evidência:

1. Manual do proprietário Chevrolet Meriva 2012: referência do veículo.
2. Bíblia/pinagem TID: referência secundária dos 12 pinos.
3. Instalações reais: referência de implementação.
4. Medição no TID físico: autoridade para fechar a eletrônica.

Documento consolidado:
`docs/REFERENCIA_MERIVA_BIBLIA_MANUAL.md`

## Núcleo

- ESP32-WROOM-32E, 3,3 V
- TID original atrás da PCB
- RTC RV-3028-C7
- GNSS
- microSD 16 GB
- buzzer com driver
- transceptor K-Line automotivo integrado

## Referência física do TID

| Pino | Função |
|---:|---|
| 1 | +15 pós-chave |
| 2 | rádio / AA-DIS |
| 3 | +30 permanente |
| 4 | iluminação |
| 5 | temperatura |
| 6 | GND |
| 7 | temperatura |
| 8 | diagnóstico |
| 9 | velocidade |
| 10 | SCL |
| 11 | SDA |
| 12 | MRQ |

Esta tabela é referência de engenharia e deve ser confirmada no TID real antes da PCB definitiva.

## Interface ESP32 ↔ TID

- SCL → GPIO32
- SDA → GPIO33
- MRQ → GPIO27

A interface elétrica deve ser integrada à PCB.

Não assumir 3,3 V diretamente nas linhas do TID.

Antes de fechar pull-ups, drivers e proteção, medir o TID.

## K-Line

A K-Line da ECU deve passar por transceptor automotivo dedicado.

Referência:

- RX ESP32: GPIO16
- TX ESP32: GPIO17
- 10400 baud
- 8N1
- ISO 14230-4 KWP Fast Init

A K-Line não pode ser ligada diretamente ao ESP32.

## IGN

- entrada: GPIO35
- proteção e adequação de nível obrigatórias
- 12 V não pode chegar diretamente ao GPIO35

## Periféricos

| Função | GPIO |
|---|---:|
| Buzzer | 26 |
| SD CS | 5 |
| SD SCK | 18 |
| SD MISO | 19 |
| SD MOSI | 23 |
| GNSS PPS | 4 |
| I2C geral | 21/22 |

GPIO5 deve ser revisado por ser pino de strap do ESP32.

GPIO16/17 ficam reservados para K-Line. O UART do GNSS deve ser remapeado.

## Proteção

A placa deve prever:

- fusível;
- proteção contra inversão;
- TVS automotivo;
- filtragem EMI;
- regulador/buck automotivo;
- desacoplamento local;
- plano de GND;
- proteção ESD nas interfaces externas;
- test points.

Valores finais dos componentes só devem ser congelados após cálculo e validação.

## Test points

- +30
- +15/IGN
- 3V3
- GND
- TID SDA
- TID SCL
- TID MRQ
- K-Line
- K-Line TX
- K-Line RX

## Regra de bancada

Não assumir a pinagem elétrica do TID sem medição no veículo.

Confirmar:

- alimentação;
- terra;
- níveis;
- pull-ups;
- sinais;
- dimensões mecânicas;
- comportamento do rádio;
- sequência de comunicação.

A Rev A definitiva só será liberada depois dessa validação.
