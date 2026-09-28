# v2.2 Core OBD-II Real

## Objetivo

Criar um núcleo OBD que converse com o ELM327 real da Meriva, seja tolerante a clones e não invente dados.

## Regras

- Bluetooth Classic/SPP
- inicialização tolerante
- timeout por comando
- reconexão sem reset do ESP32
- 0100 não é a única porta de entrada
- confirmar sessão por resposta real de PID prioritário
- guardar qualidade e frescura de cada leitura
- NO DATA, ERROR e UNABLE não viram zero

## Sequência

ATZ -> ATE0 -> ATL0 -> ATS0 -> ATH0 -> ATSP0 -> ATDP -> teste real de RPM (010C)

## PIDs prioritários

| PID | Grandeza | Fórmula |
|---|---|---|
| 010C | RPM | (A*256+B)/4 |
| 010D | velocidade | A |
| 0105 | temperatura líquido | A-40 |
| 0142 | tensão ECU | (A*256+B)/1000 |
| 0106 | STFT B1 | (A-128)*100/128 |
| 0107 | LTFT B1 | (A-128)*100/128 |
| 0104 | carga | A*100/255 |
| 010B | MAP | A |
| 0110 | MAF | (A*256+B)/100 |
| 0111 | borboleta | A*100/255 |
| 010F | temperatura admissão | A-40 |
| 012F | nível combustível | A*100/255 |

O PID 012F não pode sozinho confirmar abastecimento porque a leitura atual da boia é considerada incorreta no projeto.

## Estados

OFFLINE -> CONNECTED -> INITIALIZING -> PROTOCOL_READY -> POLLING -> OFFLINE

## Fora do escopo imediato

- driver final do TID
- tela HOME e ciclo automático
- modelo final de consumo/autonomia
- calibração da boia
- app completo
- PCB final de fabricação
