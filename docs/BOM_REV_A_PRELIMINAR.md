# Smart MID Rev A — BOM técnica preliminar

**Status:** BOM preliminar de engenharia. Não é BOM de compra nem autorização de fabricação.

Valores marcados como TBD dependem de medição, cálculo ou seleção final.

## 1. Núcleo
| Ref. | Componente | Especificação preliminar | Qtd. | Estado |
|---|---|---|---:|---|
| U1 | ESP32-WROOM-32E | módulo ESP32, 3,3 V | 1 | definido |
| U2 | Transceptor K-Line | automotivo, ISO 9141/ISO 14230, lógica compatível com ESP32 | 1 | selecionar |
| U3 | Regulador/buck automotivo | entrada compatível com 12 V automotivo, saída 3,3 V | 1 | selecionar |
| U4 | Driver de buzzer | transistor/MOSFET conforme corrente do buzzer | 1 | selecionar |

## 2. Entrada de alimentação
| Ref. | Componente | Especificação preliminar | Qtd. | Estado |
|---|---|---|---:|---|
| F1 | Fusível/proteção de entrada | corrente TBD após análise de consumo | 1 | calcular |
| D1/Q1 | Proteção contra inversão | diodo ou MOSFET de proteção | 1 | selecionar |
| D2 | TVS automotivo | tensão/standoff TBD | 1 | calcular |
| L1 | Indutor/filtro | corrente e valor TBD | 1 | calcular |
| C1 | Capacitor entrada | tensão e capacitância TBD | 1 | calcular |
| C2 | Capacitor filtro | valor TBD | 1 | calcular |

## 3. Alimentação 3,3 V
| Ref. | Componente | Especificação preliminar | Qtd. | Estado |
|---|---|---|---:|---|
| C3 | Capacitor de entrada U3 | conforme datasheet do regulador | 1 | TBD |
| C4 | Capacitor de saída U3 | conforme datasheet do regulador | 1 | TBD |
| C5-C7 | Desacoplamento local | 100 nF, próximo aos ICs | 3 | preliminar |
| C8 | Bulk 3V3 | valor TBD após orçamento de carga | 1 | calcular |

## 4. IGN / +15
| Ref. | Componente | Especificação preliminar | Qtd. | Estado |
|---|---|---|---:|---|
| R1/R2 | Divisor/adequação de nível | valores calculados para GPIO35 | 1 conjunto | calcular |
| D3 | Proteção da entrada IGN | conforme análise | 1 | selecionar |
| C9 | Filtro IGN | valor TBD | 1 | calcular |

GPIO35 é somente entrada. A rede deve garantir que nenhuma condição automotiva coloque tensão acima do limite permitido pelo ESP32.

## 5. K-Line
| Ref. | Componente | Especificação preliminar | Qtd. | Estado |
|---|---|---|---:|---|
| U2 | Transceptor K-Line automotivo | compatível com ISO 9141/ISO 14230 e lógica do ESP32 | 1 | selecionar |
| R3-R6 | Resistores da interface | conforme datasheet U2 | 1 conjunto | TBD |
| C10-C11 | Desacoplamento U2 | conforme datasheet U2 | 2 | TBD |
| D4 | Proteção K-Line | conforme aplicação e datasheet U2 | 1 | selecionar |
| TP8 | Test point K-Line | acesso ao sinal automotivo | 1 | definido |

Parâmetros de referência: 10400 baud, 8N1, ISO 14230-4 KWP Fast Init, ECU 0x11. Primeiro teste: 010C.

## 6. Interface TID
**Nenhum componente de interface está congelado.**

| Ref. | Componente | Função | Estado |
|---|---|---|---|
| U5 | Interface/level shifting TID | SDA/SCL/MRQ | TBD |
| R7-R12 | Resistores de interface/pull-up | somente após medição | TBD |
| C12-C14 | filtros/EMI | somente se necessários | TBD |
| D5-D7 | proteção ESD | avaliar após caracterização | TBD |

Antes de escolher U5 e R7-R12: medir níveis alto/baixo, pull-ups, direção, timing, tensão lógica, comportamento com rádio, inicialização e desligamento.

## 7. Buzzer
| Ref. | Componente | Especificação | Qtd. | Estado |
|---|---|---|---:|---|
| Q2 | Transistor/MOSFET | corrente adequada ao buzzer | 1 | selecionar |
| R13 | Resistor de acionamento | conforme Q2 | 1 | calcular |
| R14 | Pull-down/pull-up | conforme Q2 | 1 | calcular |
| D6 | Diodo flyback | somente se carga indutiva | 1 | condicional |
| J3 | Conector buzzer | conforme montagem | 1 | definir |

## 8. SD
| Ref. | Componente | Especificação | Qtd. | Estado |
|---|---|---|---:|---|
| J4 | Conector microSD/SD | conforme mecânica | 1 | definir |
| C15 | Desacoplamento SD | valor TBD | 1 | calcular |
| D7 | ESD SD | proteção para cartão removível | 1 | avaliar |
| R15-R18 | resistores SPI | conforme integridade de sinal | 1 conjunto | TBD |

GPIO de referência: CS=5, SCK=18, MISO=19, MOSI=23. GPIO5 precisa ser revisado antes do layout por sua relação com boot/strapping.

## 9. GNSS
GPIO16/17 estão reservados para K-Line. A UART GNSS deve ser remapeada ou o GNSS fica fora do primeiro bring-up. PPS de referência: GPIO4.

## 10. Conectores e PCB
| Ref. | Item | Estado |
|---|---|---|
| J1 | conector/chicote lado veículo | definir pelo conector real |
| J2 | conector TID original | definir pelo conector real |
| PCB1 | PCB Rev A | 90 × 50 mm alvo |
| TP1-TP10 | pontos de teste | obrigatórios |

## 11. Não congelar ainda
TVS, divisor de IGN, buck, transceptor K-Line, interface TID, pull-ups TID, filtros TID, resistores SD, ESD, conectores e fusível permanecem sujeitos a datasheet, cálculo e/ou medição.

## 12. Critério de BOM liberada
A BOM só passa de PRELIMINAR para RELEASED quando o esquema estiver fechado, componentes críticos selecionados, cálculos concluídos, TID caracterizado, ERC e DRC aprovados, disponibilidade verificada e bancada de bring-up documentada.