# Smart MID Rev A — esquema lógico pino a pino

## Objetivo
Definir o que cada conexão da Rev A deve fazer antes do desenho final no KiCad. Este documento não congela valores que dependam de medição.

## Regra de segurança
Nenhuma tensão automotiva de 12 V entra diretamente no ESP32. K-Line não entra diretamente no ESP32. SDA, SCL e MRQ do TID não recebem níveis ou pull-ups arbitrários.

## J1 — lado do veículo
| Função | Destino Rev A | Tipo | Estado |
|---|---|---|---|
| +30 permanente | proteção → alimentação TID validada + regulador 3V3 | alimentação | validar |
| +15 pós-chave | proteção/adequação → GPIO35 | entrada | validar |
| GND | plano GND | retorno | definido |
| K-Line | proteção → transceptor K-Line → GPIO16/17 | comunicação | definido conceitualmente |

## J2 — TID original
| Pino | Função de referência | Tratamento Rev A |
|---:|---|---|
| 1 | +15 pós-chave | alimentação/passagem + monitoramento IGN |
| 2 | rádio / AA-DIS | preservar/pass-through; não ligar ao ESP32 sem análise |
| 3 | +30 permanente | alimentação protegida |
| 4 | iluminação/dimmer | pass-through, validar |
| 5 | temperatura | pass-through, validar |
| 6 | GND | plano GND, confirmar |
| 7 | temperatura | pass-through, validar |
| 8 | diagnóstico | reservar até caracterização |
| 9 | velocidade | pass-through/monitoramento futuro |
| 10 | SCL | interface protegida → GPIO32 |
| 11 | SDA | interface protegida → GPIO33 |
| 12 | MRQ | interface protegida → GPIO27 |

A tabela é referência de engenharia. A pinagem física deve ser confirmada no TID real antes da fabricação.

## ESP32-WROOM-32E
- GPIO16 = K-Line RX.
- GPIO17 = K-Line TX.
- GPIO32 = TID SCL.
- GPIO33 = TID SDA.
- GPIO27 = TID MRQ.
- GPIO35 = IGN sense, somente entrada e protegido.
- GPIO26 = driver do buzzer.
- GPIO4 = GNSS PPS.
- GPIO21/22 = I2C geral reservado.
- GPIO18/19/23 = SPI do SD.
- GPIO5 = SD CS, sujeito à revisão por strap de boot.

## K-Line
J1 K-Line → proteção → transceptor automotivo → UART ESP32.

Parâmetros iniciais: 10400 baud, 8N1, ISO 14230-4 KWP Fast Init e ECU address 0x11 como referência de campo.

Primeiro teste funcional: consulta real de RPM `010C`, esperando resposta validada equivalente a `41 0C 1A F8` quando o veículo reproduzir a condição conhecida.

## Alimentação
+30 → fusível/proteção contra inversão → TVS/transientes → filtro → alimentação TID validada.

Alimentação protegida → buck/regulador automotivo → 3V3 → ESP32.

+15 → proteção/adequação de nível → GPIO35.

Valores de TVS, resistores, capacitores, buck e demais componentes devem ser calculados e validados antes de serem congelados.

## Pontos de teste
+30, +15/IGN, 3V3, GND, TID SDA, TID SCL, TID MRQ, K-Line veículo, K-Line TX e K-Line RX.

## Medições obrigatórias antes da PCB definitiva
1. tensão de repouso das linhas TID;
2. corrente do TID;
3. pull-ups existentes;
4. níveis lógico alto/baixo;
5. direção das linhas;
6. frequência e temporização;
7. sequência de inicialização;
8. comportamento do MRQ;
9. quadros reais durante operação;
10. comportamento no desligamento;
11. interação do rádio com o TID;
12. ruído e transientes relevantes na alimentação.

## Critério de liberação
A Rev A não deve ser liberada para fabricação enquanto as linhas TID e os valores de proteção/alimentação não forem validados por medição.