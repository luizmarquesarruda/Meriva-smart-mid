# Android Auto V2 - plano de integração

## Objetivo

Criar uma nova versão do aplicativo a partir da base V1 testada, mantendo o núcleo de diagnóstico e acrescentando uma interface própria para Android Auto.

## Regra de projeto

Não alterar o aplicativo V1 antes de ele estar testado e documentado.

Ordem:

`V1 -> teste físico -> correções -> APK funcionando -> congelar base -> Android Auto V2 -> testes -> Smart MID`

## O que será reaproveitado

- Bluetooth Classic / ELM327;
- sessão e fila de comandos;
- PIDs;
- DTC;
- validação de respostas;
- GPS;
- distância;
- consumo;
- autosave;
- histórico;
- armazenamento;
- regras de qualidade.

## O que muda na V2

A interface do Android Auto será uma camada nativa separada, usando APIs próprias para apps de carro.

Arquitetura alvo:

`núcleo do veículo compartilhado`

`├── interface React Native no telefone`

`└── interface Android Auto`

A UI Android Auto não deve duplicar as regras de OBD, PID, GPS ou autosave.

## Navegação

O app não deve assumir que consegue ler a rota ativa do Waze ou do Google Maps.

A integração só deve usar APIs oficiais quando houver suporte comprovado.

Não usar captura de tela, scraping ou leitura frágil da interface do Waze.

## Critério de entrada

A V2 só começa quando:

- a V1 tiver APK instalado;
- Bluetooth Classic estiver testado no carro;
- ELM327 estiver validado;
- GPS estiver validado;
- autosave estiver validado;
- falhas conhecidas estiverem registradas.

## Estado inicial

Este documento é um plano de arquitetura. Não significa que o Android Auto já esteja implementado.
