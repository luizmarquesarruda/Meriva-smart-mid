# Smart MID Rev A — revisão de engenharia elétrica

## Status
**Fase:** pré-esquemático  
**Liberação para fabricação:** BLOQUEADA

Este documento define os critérios de engenharia para transformar a arquitetura lógica da Rev A em um circuito físico seguro. Nenhum valor de componente é congelado aqui sem cálculo ou medição.

## 1. Arquitetura elétrica obrigatória

### Entrada automotiva
```
J1 +30
  -> proteção contra inversão
  -> fusível/proteção de entrada
  -> proteção contra transientes
  -> filtro
  -> distribuição

J1 +15
  -> proteção/adequação de nível
  -> GPIO35 somente como entrada de IGN

J1 GND
  -> plano GND
```

### K-Line
```
J1 K-Line
  -> proteção
  -> transceptor K-Line automotivo
  -> UART ESP32

ESP32 GPIO16 = RX
ESP32 GPIO17 = TX
```

**Proibição:** K-Line não pode ser ligada diretamente a GPIO.

### TID
```
TID SDA -> interface elétrica validada -> GPIO33
TID SCL -> interface elétrica validada -> GPIO32
TID MRQ -> interface elétrica validada -> GPIO27
```

A interface deve ser definida somente depois das medições do TID real. Não adicionar pull-up, divisor, level shifter ou driver arbitrário.

## 2. Alimentação do ESP32

A fonte de 3,3 V deve ser adequada para ambiente automotivo e para os picos de consumo do ESP32.

Antes de escolher o regulador devem ser definidos:
- tensão mínima/máxima real da entrada;
- transientes observados ou adotados para projeto;
- corrente máxima do ESP32;
- corrente dos periféricos;
- temperatura ambiente e temperatura interna da caixa;
- margem térmica;
- dissipação;
- sequência de desligamento.

O regulador final deve ser selecionado por especificação, cálculo térmico e disponibilidade. Não usar módulo buck genérico como solução final sem análise.

## 3. Proteções

A Rev A deve prever, conforme o resultado da análise:
- proteção contra inversão de polaridade;
- fusível ou proteção equivalente;
- TVS automotivo;
- filtragem de entrada;
- desacoplamento local;
- proteção ESD nas interfaces externas quando necessária;
- separação física entre sinais agressivos e sinais de baixa amplitude.

Os componentes exatos ficam **TBD** até fechar os requisitos elétricos.

## 4. GPIO críticos

| GPIO | Função | Risco/revisão |
|---:|---|---|
| 16 | K-Line RX | reservado; não usar para GNSS |
| 17 | K-Line TX | reservado; não usar para GNSS |
| 32 | TID SCL | depende da caracterização elétrica |
| 33 | TID SDA | depende da caracterização elétrica |
| 27 | TID MRQ | direção/timing ainda desconhecidos |
| 35 | IGN | entrada-only; nunca receber 12 V |
| 26 | buzzer | usar estágio de acionamento, não carga direta |
| 5 | SD CS | revisar impacto no boot |
| 18/19/23 | SD SPI | validar layout e boot |
| 21/22 | I2C | reservado |

## 5. Buzzer

GPIO26 não deve alimentar diretamente um buzzer que exceda a capacidade elétrica do GPIO.

Arquitetura preferencial:
```
GPIO26 -> resistor/base/gate -> estágio de acionamento -> buzzer
```

O componente do estágio depende do tipo de buzzer e da corrente real.

## 6. SD

O SD é periférico secundário.

Critérios:
- não comprometer o boot do ESP32;
- alimentação estável;
- desacoplamento próximo ao conector;
- sinais SPI curtos;
- proteção/ESD quando o cartão for acessível externamente;
- revisar GPIO5 por ser pino relacionado ao boot/strapping.

Se houver conflito, a função deve ser remapeada antes da PCB.

## 7. GNSS

GPIO16/17 estão reservados para K-Line.

Portanto, qualquer UART GNSS deve ser remapeada para outros GPIO disponíveis ou o GNSS deve ser temporariamente retirado da Rev A.

Não congelar o layout com UART compartilhada.

## 8. TID — bloqueio principal

Antes de transmitir qualquer sinal ao TID:

1. identificar fisicamente o conector;
2. confirmar pinagem;
3. medir tensão de cada linha em repouso;
4. medir com TID ligado;
5. medir com rádio ligado;
6. medir durante mudança de informação;
7. capturar SDA/SCL/MRQ passivamente;
8. identificar pull-ups;
9. identificar níveis lógico alto/baixo;
10. identificar direção das linhas;
11. medir frequência e tempos;
12. documentar sequência de inicialização;
13. verificar comportamento no desligamento.

**Nenhum frame será inventado.**

## 9. K-Line — primeiro marco

Depois da alimentação validada:

1. validar UART do ESP32;
2. validar transceptor;
3. validar nível físico da K-Line;
4. validar wake-up/KWP Fast Init;
5. validar comunicação com ECU;
6. executar leitura real de RPM;
7. registrar resposta bruta.

Referência de campo já conhecida:

```
010C
-> 41 0C 1A F8
-> 1726 rpm
```

Essa sequência é uma referência de teste real, não uma garantia de que todo ciclo retornará os mesmos bytes.

## 10. Pontos de teste obrigatórios

- TP1: +30 protegido
- TP2: +15/IGN protegido
- TP3: 3V3
- TP4: GND
- TP5: TID SDA
- TP6: TID SCL
- TP7: TID MRQ
- TP8: K-Line veículo
- TP9: K-Line TX
- TP10: K-Line RX

Os pontos devem permitir diagnóstico sem desmontar o circuito principal.

## 11. Estratégia de bring-up

### Etapa A — alimentação
Sem TID e sem ECU:
- verificar curto para GND;
- energizar com fonte limitada;
- validar proteção;
- validar 3V3;
- verificar temperatura.

### Etapa B — ESP32
- gravar firmware mínimo;
- validar boot;
- validar GPIO;
- validar UART.

### Etapa C — K-Line
- validar transceptor;
- validar comunicação;
- executar primeiro PID real.

### Etapa D — TID
- somente após caracterização passiva;
- implementar recepção;
- validar sincronismo;
- somente então testar transmissão.

### Etapa E — periféricos
- GNSS;
- SD;
- buzzer;
- RTC;
- funções secundárias.

## 12. Critérios de liberação Rev A

A placa somente poderá ser liberada para fabricação quando:

- [ ] pinagem TID confirmada no hardware real;
- [ ] alimentação calculada;
- [ ] proteção de entrada definida;
- [ ] regulador 3V3 selecionado;
- [ ] K-Line transceiver selecionado;
- [ ] interface TID definida por medição;
- [ ] IGN protegido;
- [ ] buzzer com estágio de acionamento;
- [ ] conflito GNSS/K-Line resolvido;
- [ ] SD/strap de boot resolvido;
- [ ] test points definidos;
- [ ] esquema elétrico revisado;
- [ ] ERC executado no KiCad;
- [ ] PCB layout revisado;
- [ ] DRC executado no KiCad;
- [ ] BOM revisada;
- [ ] teste de bancada definido.

**Sem esses itens, Rev A continua sendo projeto, não placa pronta para fabricação.**
