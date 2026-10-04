# Referência técnica Meriva: manual + Bíblia + evidência de campo

## Aplicação do projeto

Veículo de referência do Smart MID:

- Chevrolet Meriva Maxx 1.4 8V ECONO.FLEX
- ano/modelo 2011/2012
- 1389 cm³
- 4 cilindros

Este documento define a hierarquia das fontes usadas para o hardware físico.

## 1. Manual do proprietário Chevrolet Meriva 2012

Fonte oficial GM:

https://meu.chevrolet.com.br/content/dam/gmownercenter/gmsa/gmbr/dynamic/manuals/2012/chevrolet/Meriva/pt/om_ng-chevrolet_Meriva_my12-pt_BR.pdf

O manual é a referência para operação do veículo, alimentação/ignição em contexto de uso, equipamentos, advertências, manutenção e características do modelo.

O manual do proprietário não deve ser tratado como esquemático elétrico completo. Para pinagem de TID, sinais de rádio e detalhes de interface eletrônica, usamos documentação elétrica específica e medição no veículo.

## 2. Bíblia / referência de pinagem TID

A referência de pinagem usada no projeto descreve o TID de 12 vias:

| Pino TID | Função de referência |
|---:|---|
| 1 | +12 V pós-chave |
| 2 | AA/DIS / rádio |
| 3 | +12 V permanente |
| 4 | iluminação |
| 5 | sensor de temperatura |
| 6 | terra |
| 7 | sensor de temperatura |
| 8 | diagnóstico |
| 9 | velocidade |
| 10 | SCL |
| 11 | SDA |
| 12 | MRQ |

Uma fonte pública que reproduz essa tabela também identifica cores e sinais de rádio/chicote:

https://pt.scribd.com/document/1007708049/464279769-Pinagem-TID-Corsa-Vectra-Meriva-Astra

Essa fonte é **referência secundária**, não autorização para ligar o ESP32 diretamente ao chicote.

## 3. Evidência de instalação real

Relatos de instalação de computador de bordo através do TID mostram que os sinais SCL/SDA/MRQ são usados pelo TID e que modificações no fio de rádio podem ser necessárias em determinadas instalações.

Referência:

https://corsaclube.com.br/viewtopic.php?sid=9a68267cdaaef23779be311443b71968&t=104366

Há um ponto crítico nessa referência: ela alerta que o sinal do rádio pode carregar tensão que danificaria uma eletrônica de baixa tensão. Portanto, no Smart MID não vamos reproduzir cortes/derivações diretamente. A interface será protegida e definida pela medição do TID real.

## 4. Evidência técnica do protocolo

Projetos independentes de computadores de bordo para Opel/GM documentam TID/MID usando uma interface baseada em SDA/SCL/MRQ.

Referência técnica:

https://sklep.avt.pl/pl/products/tidex-komputer-pokladowy-opel-diesel-pcb-i-mikroprocesor-do-projektu-avt-5395-167248.html

Outra documentação técnica descreve a mesma família de interface e inclui Meriva entre os veículos compatíveis:

https://serwis.avt.pl/manuals/AVT5562.pdf

Isso reforça a existência da interface SDA/SCL/MRQ, mas **não substitui a captura elétrica do nosso TID**.

## 5. O que fica confirmado para o projeto

### Confirmado como arquitetura

- TID original será mantido.
- ESP32-WROOM-32E ficará na PCB Smart MID.
- A PCB será instalada como interface entre veículo e TID.
- A alimentação será protegida.
- K-Line terá transceptor automotivo dedicado.
- O MID será somente leitura em relação à ECU.
- Android não é requisito para funcionamento do MID.

### Referência de GPIO do ESP32

| Função | GPIO |
|---|---:|
| TID SCL | 32 |
| TID SDA | 33 |
| TID MRQ | 27 |
| IGN sense | 35 |
| Buzzer | 26 |
| SD CS | 5 |
| SD SCK | 18 |
| SD MISO | 19 |
| SD MOSI | 23 |
| GNSS PPS | 4 |

GPIO16/17 ficam reservados para K-Line na nova arquitetura. O UART do GNSS deve ser remapeado.

## 6. O que NÃO fica confirmado somente pela Bíblia/manual

Não congelar sem medição:

- tensão lógica do TID;
- tensão das linhas SDA/SCL/MRQ;
- resistores de pull-up;
- direção das linhas;
- endereço I2C;
- temporização;
- sequência de inicialização;
- formato exato dos frames;
- comportamento do pino MRQ;
- corrente consumida pelo TID;
- comportamento do sinal de rádio;
- proteção necessária para cada linha.

## 7. Regra de integração

A documentação do veículo define o contexto.

A Bíblia de pinagem fornece uma referência física.

As instalações reais mostram como outras pessoas resolveram o problema.

A medição do TID físico decide o circuito final.

Portanto:

**Manual → referência do veículo**

**Bíblia → referência de pinagem**

**Instalações reais → referência de implementação**

**Medição no nosso TID → decisão elétrica final**

## 8. Próxima melhoria da Rev A

Antes de fabricar a PCB definitiva:

1. identificar exatamente o TID instalado na Meriva;
2. confirmar os 12 pinos;
3. medir +30, +15 e GND;
4. medir SDA/SCL/MRQ em repouso;
5. capturar a comunicação sem transmitir;
6. identificar níveis e temporização;
7. definir proteção/driver;
8. somente então fechar o esquemático;
9. testar em bancada;
10. testar no veículo.

## Regra de segurança

Nenhum sinal de 12 V será conectado diretamente ao ESP32.

Nenhuma K-Line será conectada diretamente ao ESP32.

Nenhum comando desconhecido será transmitido ao TID.

Nenhuma informação de outra versão GM será considerada automaticamente válida para a Meriva 2011/2012.
