# Base validada do aplicativo Meriva Smart Diagnostic V1

> Registro técnico para o Smart MID. Este documento reúne o que já foi implementado e observado no aplicativo Android, para que o firmware e o hardware do MID partam de dados reais.

## Fonte

- Aplicativo: `luizmarquesarruda/meriva-smart-diagnostic`
- Branch de preparação do clone para a futura versão Android Auto: `android-auto-base-v1`
- Ponto de partida do clone: commit `f732b598f470a2175a03dfe63896a464652a9ebc`
- PR aberto relacionado ao build Termux: PR #9

O PR #9 está aberto. O build Android em CI ainda precisa ser confirmado antes de chamar esta base de "V1 congelada".

## O que já existe no aplicativo

### Bluetooth e ELM327

- Bluetooth Classic/SPP.
- Verificação de suporte Bluetooth no Android.
- Solicitação de permissões necessárias.
- Verificação de Bluetooth ligado.
- Orientação para abrir as configurações quando necessário.
- Descoberta de dispositivos pareados.
- Transporte Bluetooth Classic para ELM327.
- Sessão ELM327 real com fila de comandos.
- Leitura terminada pelo prompt `>`.
- Estados explícitos para conexão, inicialização, resposta válida, timeout e erro.

### OBD

A camada TypeScript concentra a lógica de OBD, PID e DTC. A camada nativa Android serve como ponte de transporte e integração, sem duplicar a regra de diagnóstico.

Sequência ELM usada no aplicativo:

`ATZ -> ATE0 -> ATL0 -> ATS0 -> ATH1 -> ATSP0 -> ATDP`

A leitura deve distinguir:

- resposta real;
- timeout;
- erro;
- ausência de resposta;
- dado indisponível.

### GPS

- Permissão de localização em primeiro plano.
- Rastreamento de posição.
- Cálculo de distância por GPS.
- Velocidade normalizada.
- Velocidade máxima da viagem.
- Precisão do GPS.
- Filtros de precisão e velocidade.
- Singleton de rastreamento para evitar múltiplas sessões.

### Autosave e histórico

O aplicativo já possui estrutura de autosave, histórico e exportação. O Smart MID deve reaproveitar o conceito de:

`leitura -> validação -> armazenamento -> histórico`

e não inventar dados ausentes.

## Regra mais importante para o MID

O aplicativo é a referência empírica. O MID deve mostrar somente:

1. valor recebido da ECU;
2. valor recebido do GNSS;
3. valor derivado de fórmula conhecida;
4. valor estimado quando existir fonte e qualidade suficientes.

Nunca transformar `NO DATA`, `ERROR`, `UNABLE` ou timeout em zero.

## Consumo

O aplicativo foi preparado para trabalhar com consumo real baseado em dados OBD quando o veículo fornecer o PID correspondente.

O PR #6 do aplicativo implementa uso do PID `015E` (fuel rate, L/h) para consumo, mas ele está separado da linha de build do PR #9. Portanto:

- PID `015E` ainda precisa ser confirmado fisicamente na ECU da Meriva;
- não assumir suporte só porque o PID é padrão;
- quando houver fuel rate válido, o consumo pode ser calculado por integração no tempo;
- GPS sozinho mede distância, mas não mede litros consumidos.

## Dados reais já observados na Meriva

Registro de referência de 24/09/2026, usando Car Scanner:

| Grandeza | Valor observado |
|---|---:|
| Temperatura do líquido | 81 °C |
| STFT B1 | -8,59 % |
| LTFT B1 | +10,94 % |
| MAP | 5,66 psi |
| Marcha lenta | 778 rpm |
| Avanço | 8 ° |
| IAT | 33 °C |
| MAF | 8,93 kg/h |
| Borboleta | 3,53 % |
| O2S1B1 | 0,53 V |
| O2 STFT | -9,38 % |
| Protocolo | ISO 14230-4 KWP fast init |
| Velocidade de comunicação | 10,4 kbaud |
| Endereço ECU | 11 |

Esses números são um "snapshot" de diagnóstico, não devem ser usados como constantes do firmware.

## ELM327 usado no carro

Observação prática do projeto:

- ELM327 descrito pelo usuário como versão 1.5 funciona na Meriva;
- ELM327 descrito como versão 2.0 não funciona no mesmo veículo.

O firmware deve ser tolerante a clones e confirmar o funcionamento pela resposta real da ECU.

## Histórico mecânico que pode influenciar leituras

A Meriva teve falha de junta do cabeçote e o cabeçote foi posteriormente reparado. Esse histórico deve ser considerado na interpretação de consumo, mistura, lambda, temperatura e correções de combustível.

O sensor do radiador também foi substituído. Após a troca, o acionamento da ventoinha passou a ocorrer próximo de 90 °C em condição quente, devendo essa faixa ser tratada como observação de campo e não como calibração fixa sem novas medições.

## Combustível / boia

O projeto possui uma ressalva conhecida:

- a leitura da boia/nível de combustível é considerada imprecisa no veículo;
- o PID `012F` não confirma sozinho um abastecimento;
- a autonomia deve ter proteção contra saltos causados por leitura ruim da boia;
- o reset de abastecimento deve ser confirmado por regra própria ou acionamento manual enquanto a boia não for corrigida.

## O que deve passar para o Smart MID

### Já aproveitável

- Bluetooth Classic/SPP;
- lógica de sessão ELM327;
- confirmação por resposta real;
- PIDs e fórmulas;
- tratamento de qualidade/frescura;
- GPS;
- distância da viagem;
- horímetro;
- autosave;
- histórico;
- conceito de consumo por fuel rate;
- separação entre dado real e valor estimado.

### Ainda precisa de validação no carro

- PID `015E`;
- suporte de cada PID pela ECU;
- estabilidade de polling contínuo;
- tempo real de resposta;
- leitura final do TID;
- pinagem elétrica do TID;
- comportamento final da boia;
- autonomia calculada em abastecimentos reais.

## Regra de arquitetura

`ECU -> K-Line -> OBD -> ELM327 -> ESP32 -> TID`

O MID deve permanecer somente leitura. Não enviar comandos de controle de atuadores para a ECU.

O aplicativo Android e o firmware do MID devem compartilhar conceitos e fórmulas validadas, mas cada um deve manter sua própria camada de interface.
