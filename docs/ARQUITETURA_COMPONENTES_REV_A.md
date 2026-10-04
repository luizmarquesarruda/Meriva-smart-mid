# Smart MID Rev A — arquitetura de componentes

## Status

**Arquitetura de engenharia Rev A — pré-esquemático.**

Objetivo: transformar a BOM preliminar em blocos elétricos claros, com fronteiras de proteção, alimentação, comunicação e controle.

## 1. Diagrama de blocos

```
                 VEÍCULO MERIVA
                      │
        ┌─────────────┼─────────────┐
        │             │             │
       +30           +15          GND
        │             │             │
        ▼             ▼             │
  [F1 + proteção] [proteção IGN]    │
        │             │             │
        ▼             ▼             │
 [Q1 reversão]     GPIO35           │
        │                           │
        ▼                           │
 [TVS + filtro]                     │
        │                           │
        ├───────────────┐           │
        │               │           │
        ▼               ▼           │
 [U3 BUCK 3V3]      [U2 K-LINE]     │
        │               │           │
       3V3          RX/TX ESP32     │
        │               │           │
        └───────┬───────┘           │
                ▼                   │
         [U1 ESP32-WROOM] ◄─────────┘
                │
       ┌────────┼─────────┐
       │        │         │
       ▼        ▼         ▼
    TID I/F     SD       GNSS
       │
       ▼
   TID ORIGINAL
```

## 2. U1 — ESP32-WROOM-32E

Função: controlador principal.

### Reservas

- GPIO16: K-Line RX
- GPIO17: K-Line TX
- GPIO32: TID SCL
- GPIO33: TID SDA
- GPIO27: TID MRQ
- GPIO35: IGN sense
- GPIO26: buzzer driver
- GPIO4: GNSS PPS
- GPIO21/22: I2C geral
- GPIO18/19/23: SD SPI
- GPIO5: SD CS, pendente de revisão de boot

GPIO16/17 não podem ser reutilizados pelo GNSS.

## 3. U2 — transceptor K-Line

**Componente selecionado:** TI TLIN2029DQ1, SOIC-8.

O fabricante informa suporte a ISO 9141/K-Line, alimentação de 4 a 36 V, proteção de barramento de ±45 V e taxa de até 20 kbps. É AEC-Q100. citeturn0search0turn0search6

### Arquitetura

```
ESP32 GPIO17 TX
      │
      ▼
    U2 TXD
      │
      ▼
 U2 LIN/K-Line
      │
      ├── proteção/roteamento
      │
      ▼
 J1 K-Line
      │
      ▼
 ECU

J1 K-Line
      │
      ▼
 U2 receptor
      │
      ▼
 U2 RXD
      │
      ▼
 ESP32 GPIO16
```

RXD é open-drain. O pull-up para 3,3 V deve ser definido conforme o datasheet e a interface real do ESP32.

**Nota:** U2 é camada física. KWP Fast Init, endereçamento, checksum e temporização continuam no firmware.

## 4. U3 — fonte 3,3 V

**Componente selecionado:** TI LM536033QPWPRQ1, HTSSOP-16.

Características relevantes: saída fixa de 3,3 V, até 3 A, entrada de 3,5 a 36 V, transientes até 42 V, operação automotiva e proteção térmica/corrente/UVLO. citeturn0search3

### Arquitetura

```
+30 protegido
      │
      ▼
    VIN U3
      │
      ├── Lx / indutor conforme datasheet
      │
      ▼
   VOUT 3V3
      │
      ├── ESP32
      ├── U2 lógica
      ├── TID interface
      ├── SD
      └── GNSS
```

O indutor, capacitores, frequência/configuração e layout devem seguir a aplicação do fabricante. Não substituir por módulo buck genérico na PCB final.

## 5. Proteção de entrada

### F1

Fusível automotivo ou proteção eletrônica equivalente.

**Valor: TBD.**

Deve ser escolhido depois de medir a corrente total e considerar corrente de partida, periféricos e margem.

### Q1

**Candidato selecionado:** DMPH6050SPDWQ, MOSFET P-channel 60 V.

O fabricante informa AEC-Q101/PPAP, 60 V e VGS máximo de ±20 V. O part number selecionado é dual P-channel. citeturn0search48turn0search2

**Ponto de engenharia:** por ser dual P-channel, o esquema deve usar apenas a configuração necessária e deixar a segunda seção tratada conforme recomendação do fabricante. Não deixar gate/drain flutuando.

A rede de gate ainda precisa ser calculada para limitar VGS durante transientes.

## 6. D2 — TVS

**Candidato:** ST SMBJ24A.

É um TVS de 24 V, 600 W em SMB. citeturn0search1turn0search4

### Estado

**NÃO LIBERADO.**

Antes do congelamento:
- verificar tensão de stand-off;
- tensão de clamp;
- energia;
- coordenação com F1;
- limite de entrada de U3;
- comportamento do Q1;
- transientes automotivos adotados no projeto.

A posição final deve privilegiar caminho curto entre entrada protegida e retorno.

## 7. IGN

```
J1 +15
  │
  ▼
proteção
  │
  ▼
adequação de nível
  │
  ▼
GPIO35
```

GPIO35 é entrada-only.

A rede deve incluir:
- limitação de tensão;
- proteção contra transientes;
- filtragem;
- referência GND adequada.

Valores TBD.

## 8. TID

O TID fica atrás de uma fronteira elétrica própria:

```
TID SDA ──► interface TBD ──► GPIO33
TID SCL ──► interface TBD ──► GPIO32
TID MRQ ──► interface TBD ──► GPIO27
```

Não congelar U5, resistores ou pull-ups antes das medições no TID real.

## 9. Buzzer

```
GPIO26
  │
 resistor
  │
  ▼
Q2 driver
  │
  ▼
Buzzer
```

Se a carga for indutiva, prever proteção de flyback.

## 10. SD

SPI:

- CS = GPIO5
- SCK = GPIO18
- MISO = GPIO19
- MOSI = GPIO23

GPIO5 permanece **pendente de revisão por boot/strapping** antes de congelar a PCB.

## 11. GNSS

GPIO16/17 estão permanentemente reservados para K-Line.

O GNSS deverá:
- usar outra UART/GPIO; ou
- ficar fora da primeira Rev A.

PPS permanece em GPIO4 como referência.

## 12. Domínios de layout

A PCB deve ser dividida em áreas:

1. entrada automotiva;
2. proteção;
3. conversor buck;
4. ESP32;
5. K-Line;
6. TID;
7. SD/GNSS;
8. conectores/test points.

A área de entrada e K-Line deve ficar afastada das linhas sensíveis do TID.

O caminho de corrente de transientes deve ser curto.

## 13. Pontos de teste

TP1 +30 protegido  
TP2 +15/IGN  
TP3 3V3  
TP4 GND  
TP5 TID SDA  
TP6 TID SCL  
TP7 TID MRQ  
TP8 K-Line veículo  
TP9 K-Line TX  
TP10 K-Line RX

## 14. Estado de congelamento

### 🟢 Arquitetura definida
- ESP32-WROOM-32E
- TLIN2029DQ1
- LM536033QPWPRQ1
- GPIO map
- fronteira K-Line
- fronteira TID
- pontos de teste

### 🟡 Seleção sujeita a cálculo
- Q1
- D2
- F1
- filtro de entrada
- gate network
- componentes do buck
- proteção IGN

### 🔴 Bloqueado por medição
- interface TID
- pull-ups TID
- níveis lógicos TID
- timing TID
- frames TID

## 15. Critério de entrada no KiCad

O esquemático elétrico pode começar com estes blocos e referências.

Não pode ser considerado RELEASED até:
- valores calculados;
- datasheets arquivados;
- TID caracterizado;
- ERC aprovado;
- DRC aprovado;
- revisão térmica;
- revisão de proteção;
- revisão de fabricação.

## Referências técnicas

- TI TLIN2029-Q1. citeturn0search0turn0search6
- TI LM536033QPWPRQ1. citeturn0search3
- Diodes DMPH6050SPDWQ. citeturn0search48
- ST SMBJ24A. citeturn0search1turn0search4
