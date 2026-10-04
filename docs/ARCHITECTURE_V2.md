# Arquitetura consolidada v2

## Objetivo
O Smart MID físico é independente do Android. O ESP32 é o núcleo, o TID original é o display principal e a ECU é acessada por K-Line através de transceptor automotivo instalado na própria PCB.

## Fluxo físico Rev A
ECU Meriva → K-Line → proteção → transceptor K-Line na PCB → UART ESP32 → núcleo OBD → dados validados → TID original

ELM327 Bluetooth não é requisito do funcionamento do MID. Ele permanece como ferramenta externa de referência e teste.

O Smart MID é somente leitura. Não transmite comandos de controle para a ECU e não aciona atuadores.

## Relação com o aplicativo Android
O repositório `luizmarquesarruda/meriva-smart-diagnostic` continua sendo referência prática para comportamento OBD, PID, DTC e tratamento de erros. É um projeto separado e não é dependência de runtime do MID.

## Regras
- Confirmar sessão por resposta real da ECU.
- Distinguir timeout, erro, ausência de dados e leitura válida.
- Não converter NO DATA, ERROR, UNABLE ou timeout em zero.
- Não usar informação da boia como fonte de autonomia ou diagnóstico.
- Manter GPS separado da lógica OBD.

## GPIO Rev A
| Função | GPIO |
|---|---:|
| K-Line RX | 16 |
| K-Line TX | 17 |
| IGN sense | 35 |
| Buzzer | 26 |
| SD CS | 5 |
| SD SCK | 18 |
| SD MISO | 19 |
| SD MOSI | 23 |
| I2C SDA | 21 |
| I2C SCL | 22 |
| TID SCL | 32 |
| TID SDA | 33 |
| TID MRQ | 27 |
| GNSS PPS | 4 |

GPIO16/17 ficam reservados para K-Line. O UART do GNSS deve ser remapeado. GPIO35 é somente entrada no ESP32 clássico e deve receber sinal protegido. GPIO5 é strap de boot e precisa ser revisado.

## TID
SDA, SCL e MRQ ficam na própria PCB. Antes de definir pull-ups, níveis, drivers ou temporização, medir o TID físico. Não enviar bytes inventados.

## K-Line
Usar transceptor automotivo dedicado na PCB. Referência inicial: 10400 baud, 8N1 e ISO 14230-4 KWP Fast Init. Validar com a ECU real.

## Ordem de evolução
arquitetura elétrica → medições TID → esquemático Rev A → ERC → bancada → K-Line real → primeiro PID real (RPM) → TID real → GNSS/SD → Bluetooth complementar