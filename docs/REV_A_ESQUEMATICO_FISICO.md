# Smart MID Rev A — Arquitetura elétrica física

## Objetivo

Definir a Rev A do Smart MID da Chevrolet Meriva com ESP32-WROOM-32E, TID original, proteção automotiva integrada e transceptor K-Line integrado na própria PCB.

O aplicativo Android é um projeto separado.

## Arquitetura

CHICOTE MERIVA
→ J1
→ proteção e alimentação
→ ESP32-WROOM-32E
→ interface TID / transceptor K-Line
→ J2
→ TID ORIGINAL

Não haverá adaptador externo durante o uso normal.

## Referência de conexão do TID

| Pino | Função de referência | Tratamento |
|---:|---|---|
| 1 | +15 pós-chave | entrada IGN protegida |
| 2 | rádio / informação | preservar e validar |
| 3 | +30 permanente | alimentação protegida |
| 4 | iluminação/dimmer | pass-through, validar |
| 5 | temperatura | pass-through, validar |
| 6 | GND | terra comum |
| 7 | temperatura | pass-through, validar |
| 8 | diagnóstico | reservar conforme medição |
| 9 | velocidade | pass-through/monitoramento |
| 10 | SCL | interface GPIO32 |
| 11 | SDA | interface GPIO33 |
| 12 | MRQ | interface GPIO27 |

A pinagem deve ser conferida no TID físico antes da primeira energização.

## Alimentação

+30 permanente:

1. proteção de entrada;
2. proteção contra inversão;
3. proteção contra transientes;
4. filtragem;
5. alimentação do TID conforme validação;
6. regulador para 3,3 V do ESP32.

+15 pós-chave:

- usado como IGN_SENSE;
- GPIO35 é somente entrada no ESP32 clássico;
- usar proteção e adequação de nível;
- nunca aplicar 12 V diretamente ao GPIO35.

Os valores finais dos componentes só serão congelados após validação elétrica e térmica.

## Interface TID

GPIO de referência:

- SDA = 33
- SCL = 32
- MRQ = 27

A interface elétrica fica na PCB.

Antes de definir pull-ups, níveis e temporização, medir:

1. tensão de repouso;
2. pull-ups;
3. direção das linhas;
4. frequência;
5. sequência de inicialização;
6. endereço;
7. formato das mensagens;
8. comportamento no desligamento.

Não enviar bytes inventados ao TID.

## K-Line

A K-Line da ECU deve usar transceptor automotivo dedicado instalado na PCB.

ECU K-Line
→ proteção
→ transceptor K-Line
→ UART ESP32

Nunca conectar K-Line diretamente ao ESP32.

Referência:

- RX = GPIO16
- TX = GPIO17
- 10400 baud
- 8N1
- ISO 14230-4 KWP Fast Init

## Conflito GNSS

GPIO16/17 ficam reservados para K-Line.

O UART histórico do GNSS também usava esses pinos. Portanto, o GNSS UART deve ser remapeado antes do esquemático final.

## Outros periféricos

| Função | GPIO | Observação |
|---|---:|---|
| Buzzer | 26 | usar driver adequado |
| IGN | 35 | entrada protegida |
| SD CS | 5 | revisar strap de boot |
| SD SCK | 18 | SPI |
| SD MISO | 19 | SPI |
| SD MOSI | 23 | SPI |
| GNSS PPS | 4 | entrada |
| I2C geral | 21/22 | reservado |

GPIO5 deve ser revisado porque é pino de strap do ESP32 clássico.

## Pontos de teste

Prever test points para:

- +30
- +15/IGN
- 3V3
- GND
- TID SDA
- TID SCL
- TID MRQ
- K-Line
- UART K-Line TX
- UART K-Line RX

## Teste de bancada

1. inspeção visual;
2. continuidade;
3. teste de curto;
4. fonte limitada;
5. validar 3V3;
6. validar ESP32;
7. validar IGN;
8. capturar TID de forma passiva;
9. validar transceptor K-Line;
10. só depois conectar ao veículo.

## Ordem de desenvolvimento

1. fechar arquitetura elétrica;
2. validar alimentação;
3. validar interface TID;
4. capturar protocolo real do TID;
5. fechar K-Line;
6. implementar KWP Fast Init;
7. validar ECU;
8. implementar primeiro PID real: RPM;
9. exibir dados reais no TID;
10. resolver GNSS;
11. integrar SD;
12. Bluetooth como interface complementar.

## Regra

Sem dado real, o MID não inventa valor.
