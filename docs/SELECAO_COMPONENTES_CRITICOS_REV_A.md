# Rev A — seleção preliminar de componentes críticos

## Decisão de engenharia

Após revisão de datasheets de fabricantes, os candidatos principais para a Rev A são:

| Ref. | Função | Componente | Estado |
|---|---|---|---|
| U2 | K-Line | TI TLIN2029D-Q1 / TLIN2029-Q1 | **RECOMENDADO** |
| U3 | Buck 3V3 | TI LM536033QPWPRQ1 | **RECOMENDADO** |
| Q1 | proteção contra inversão | Diodes DMPH6050SPDWQ | **RECOMENDADO** |
| D2 | TVS de entrada | ST SMBJ24A | **CANDIDATO** |
| F1 | proteção de sobrecorrente | fusível automotivo, valor TBD | **CALCULAR** |

## U2 — K-Line

### TI TLIN2029-Q1

O TLIN2029-Q1 suporta ISO 9141, que inclui a camada física usada pela K-Line, trabalha em alimentação de 4 V a 36 V, possui proteção de barramento de ±45 V e é qualificado AEC-Q100. A taxa de transmissão chega a 20 kbps, suficiente para a referência de 10,4 kbps usada pela Meriva.

A saída RXD é open-drain e pode ser puxada para 3,3 V, permitindo conexão adequada ao ESP32. A TI especifica resistor externo de pull-up de 1 kΩ a 10 kΩ quando necessário.

### Part number

**TLIN2029DQ1**, encapsulamento SOIC-8, é a opção preferida para a primeira PCB por facilitar inspeção e retrabalho.

Alternativa de menor área: **TLIN2029DRBRQ1**, VSON-8.

### Motivo

É uma escolha mais adequada que um transceptor LIN genérico sem verificação, porque a própria TI declara suporte a ISO 9141/K-Line e operação automotiva.

### Atenção

O TLIN2029 não transforma sozinho o protocolo KWP em uma biblioteca OBD. Ele é a camada física. O firmware continua responsável pelo wake-up/Fast Init, temporização, enquadramento e protocolo.

## U3 — alimentação 3,3 V

### TI LM536033QPWPRQ1

Buck síncrono automotivo de 3 A, saída fixa de 3,3 V, entrada de 3,5 V a 36 V e tolerância a transientes até 42 V. É AEC-Q1 e possui proteção de corrente, térmica e UVLO.

Encapsulamento: HTSSOP-16.

### Por que este foi escolhido

O ESP32 e os periféricos precisam de uma alimentação 3,3 V estável. O LM536033-Q1 fornece margem de corrente muito superior à necessidade esperada do Smart MID e foi desenvolvido para aplicações automotivas de 12 V.

O circuito deve seguir o layout e os componentes recomendados pelo datasheet. Não substituir por módulo buck genérico na placa final.

### Part number

**LM536033QPWPRQ1**

## Q1 — proteção contra inversão

### Diodes DMPH6050SPDWQ

MOSFET P-channel de 60 V, AEC-Q101, PPAP-capable, destinado a aplicações automotivas. A versão especificada apresenta RDS(on) máximo de 60 mΩ a VGS = 4,5 V e capacidade de corrente de 6,3 A a 25 °C.

Será usado como elemento série de proteção contra polaridade reversa.

### Atenção de projeto

O circuito de gate ainda precisa ser fechado. O VGS máximo do MOSFET deve ser protegido durante transientes. O layout deve prever resistor de gate e clamp apropriado.

## D2 — TVS

### ST SMBJ24A

TVS bidirecional de 24 V, 600 W, encapsulamento SMB.

Foi escolhido como **candidato inicial**, não como valor final liberado.

O motivo é preservar a operação normal de uma rede automotiva de 12 V sem fazer o TVS conduzir continuamente. Entretanto, a coordenação entre TVS, fusível, MOSFET e limite de entrada do LM536033-Q1 precisa ser verificada contra os transientes reais/adotados no projeto.

**Não liberar D2 sem cálculo de clamp e energia.**

## F1 — sobrecorrente

O valor do fusível permanece TBD.

Antes de escolher:

1. medir corrente do Smart MID completo;
2. estimar corrente de partida;
3. considerar ESP32, TID, SD, GNSS, buzzer e perdas;
4. definir corrente nominal;
5. definir capacidade de interrupção;
6. validar coordenação com o TVS e Q1.

## Arquitetura preliminar

```
J1 +30
  |
 F1
  |
 Q1  DMPH6050SPDWQ
  |
 D2  SMBJ24A
  |
 filtro de entrada
  |
 U3  LM536033QPWPRQ1
  |
  +---- 3V3 ---- ESP32
  |             |
  |             +---- U2 logic
  |             +---- SD
  |             +---- TID interface
  |
 U2 TLIN2029DQ1
  |
 K-Line veículo
```

A posição física exata de D2, filtro e desacoplamentos será definida pelo esquema/layout e pelas recomendações dos datasheets. A proteção deve ser organizada para que o caminho de transiente seja curto.

## Estado de liberação

### Pode avançar para esquemático
- U2: TLIN2029DQ1
- U3: LM536033QPWPRQ1
- Q1: DMPH6050SPDWQ

### Ainda não liberar para fabricação
- D2: SMBJ24A
- F1
- valores do gate de Q1
- indutor do buck
- capacitores do buck
- filtro de entrada
- proteção de IGN

Esses últimos componentes dependem do cálculo final e das redes recomendadas pelos datasheets.

## Fontes técnicas

- TI TLIN2029-Q1: suporte ISO 9141/K-Line, 4–36 V e proteção de barramento.
- TI LM53603-Q1: buck automotivo 3,3 V, 3 A, 36 V e transientes de 42 V.
- Infineon/Diodes: MOSFETs P-channel 60 V para proteção de polaridade reversa.
- ST SMBJ24A: TVS 24 V, 600 W.

**Classificação:** engenharia preliminar. Não é autorização de fabricação.
