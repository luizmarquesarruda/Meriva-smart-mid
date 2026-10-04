# Pesquisa de instalação TID/MID na Meriva

Data da pesquisa: 2026-10-04

## Objetivo

Pesquisar instalações reais de computador de bordo usando o TID original em veículos GM, com prioridade para Meriva, e usar as práticas encontradas para melhorar o Smart MID físico.

## Fonte principal encontrada

### YouTube — Corsa Upgrade

**MERIVA 2007: COMPUTADOR DE BORDO INSTALADO ATRAVÉS DO TID**

Vídeo:
https://www.youtube.com/watch?v=82cszH7ZMQY

A descrição informa:

- desbloqueio do TID;
- instalação de kit de computador de bordo através do TID;
- adaptação da alavanca com botões R/S em carros com BCM.

O vídeo confirma que existe uma instalação prática de computador de bordo utilizando o TID original da Meriva.

## Outra referência do mesmo conjunto de vídeos

**CLASSIC 2016: DÚVIDAS SOBRE A INSTALAÇÃO DO COMPUTADOR DE BORDO MID NO CORSA B**

https://www.youtube.com/watch?v=8W3sPmaLVHc

A descrição aponta uma série de vídeos sobre:

- instalação de computador de bordo através do TID;
- instalação de TID;
- identificação de chicote pré-disposto;
- BCM;
- sensor de temperatura externa;
- aplicação em Meriva.

## Evidência complementar de instalação física

Um relato no Clube do Vectra documenta a instalação de uma placa de computador de bordo em um veículo GM que usa TID compatível com Meriva/Corsa/Montana/Vectra C.

A instalação descrita envolve:

- comandos na chave do limpador;
- derivação de sinal no painel de instrumentos;
- sinal relacionado ao bico injetor;
- chicote adicional;
- passagem organizada dos fios por conduíte e fita de tecido.

A referência também informa que o kit é compatível com TID utilizado em Meriva, Corsa, Montana e Vectra C.

## Evidência importante sobre o TID

Documentação comercial de kits de computador de bordo para GM descreve uma placa instalada junto ao TID, com chicote separado para comandos e sinais.

Também diferencia TIDs de gerações/modelos diferentes e recomenda atenção à compatibilidade física e elétrica.

Isso reforça uma decisão importante para o nosso projeto:

**não assumir que todo TID GM possui exatamente o mesmo comportamento elétrico ou protocolo.**

## O que vamos aproveitar

### 1. Instalação física limpa

A instalação comercial/real usa:

- chicote dedicado;
- conexões organizadas;
- fixação da eletrônica;
- passagem protegida dos fios.

### 2. TID original

Não precisamos substituir o display.

Nosso projeto mantém o TID original e coloca a eletrônica na PCB Smart MID.

### 3. Comandos

As instalações existentes usam comandos R/S ou equivalentes para navegar nas funções.

No Smart MID, isso pode virar uma entrada física simples ou comandos configuráveis.

Não precisamos copiar a solução mecânica exatamente.

### 4. Dados do veículo

Soluções existentes calculam funções como:

- autonomia;
- tensão da bateria;
- distância;
- consumo;
- temperatura;
- velocidade;
- tempo de viagem.

No nosso projeto, a prioridade será obter dados reais da ECU pela K-Line/ISO 14230 e usar GNSS somente quando fizer sentido.

Isso evita depender de derivações improvisadas do chicote do bico injetor para obter consumo.

## Melhorias propostas para o Smart MID

### Melhoria A — PCB integrada

Em vez de uma placa externa presa ao TID com vários fios:

**veículo → PCB Smart MID → TID**

A PCB concentra:

- ESP32;
- proteção automotiva;
- alimentação;
- interface TID;
- K-Line;
- entradas;
- armazenamento;
- futuras interfaces.

### Melhoria B — K-Line real

As soluções pesquisadas não são nosso modelo de comunicação com a ECU.

O Smart MID terá:

**K-Line → transceptor automotivo → ESP32**

Nunca K-Line diretamente no ESP32.

### Melhoria C — dados reais da ECU

Prioridade:

1. comunicação ECU;
2. RPM;
3. velocidade;
4. temperatura;
5. carga/MAP/MAF conforme PID disponível;
6. combustível/consumo calculado;
7. autonomia;
8. registros.

### Melhoria D — não inventar consumo

Se o PID necessário não estiver disponível ou confiável:

- marcar como indisponível;
- usar outra fonte validada;
- nunca mostrar número fabricado.

### Melhoria E — instalação reversível

A PCB deve funcionar como interface pass-through sempre que possível.

A instalação deve permitir retornar ao TID original sem cortar o chicote principal do veículo.

### Melhoria F — proteção automotiva

A eletrônica será protegida contra:

- inversão de polaridade;
- transientes;
- ruído;
- ESD;
- falhas de periféricos.

Os valores finais dos componentes serão definidos após validação elétrica.

## O que NÃO vamos copiar

Não vamos copiar automaticamente:

- pinagens de outro modelo GM;
- protocolo de outro TID;
- valores de resistores;
- comandos de display;
- sinais de bico injetor;
- velocidade analógica;
- qualquer ligação descrita para outro ano/modelo.

Tudo isso será tratado como referência até ser confirmado no TID e na Meriva reais.

## Conclusão

A pesquisa mostra que a ideia é viável e já existe na prática: o TID original pode ser usado como interface de computador de bordo em Meriva e outros GM compatíveis.

O Smart MID vai melhorar esse conceito:

**instalação original + eletrônica integrada + ECU real + proteção automotiva + TID original + funcionamento independente do Android.**

A próxima etapa técnica é capturar o comportamento elétrico e o protocolo do TID real antes de fechar o circuito de comunicação.

## Fontes

- YouTube / Corsa Upgrade: MERIVA 2007: COMPUTADOR DE BORDO INSTALADO ATRAVÉS DO TID
- YouTube / Corsa Upgrade: CLASSIC 2016: DÚVIDAS SOBRE A INSTALAÇÃO DO COMPUTADOR DE BORDO MID NO CORSA B
- Clube do Vectra: relato de instalação de placa de computador de bordo em TID compatível com Meriva
- Documentação pública de kits de computador de bordo para GM/TID
