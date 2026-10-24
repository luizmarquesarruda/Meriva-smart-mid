# Redefinição do projeto — Meriva Smart MID cabeado

**Estado:** proposta de arquitetura Rev B; não liberada para fabricação.  
**Veículo:** Chevrolet Meriva Maxx 1.4 8V ECONO.FLEX, ano/modelo 2011/2012.  
**Controlador:** ESP32-WROOM-32E.  
**Display:** TID original GM.  
**Requisito fixo:** funcionamento exclusivamente cabeado; sem Bluetooth, sem ELM327, sem aplicativo Android e sem rádio original GM.

## 1. Objetivo

Construir um computador de bordo físico e simples. O ESP32 consulta a ECU pelo conector de diagnóstico, valida cada resposta e apresenta no TID somente dados realmente fornecidos e confirmados. Cálculos auxiliares serão habilitados apenas quando houver dados suficientes e comprovados.

### Incluído no primeiro marco
- Comunicação direta por fios, através de transceptor automotivo adequado.
- Descoberta do suporte OBD e validação individual de PIDs.
- Exibir primeiro a rotação do motor; acrescentar parâmetros só após confirmação no veículo.
- Driver de escrita do TID, implementado após medição e captura do protocolo real.
- Estados por parâmetro: válido, desatualizado, sem resposta, inválido e não suportado.
- Botões físicos simples para navegar pelas páginas, escolhidos após verificar o chicote e a viabilidade mecânica.

### Fora do escopo inicial
- Bluetooth, Wi-Fi, web, OTA, aplicativo Android ou dependência de ELM327.
- Rádio original GM: o veículo usará a multimídia aftermarket.
- Derivação do sinal do injetor, boia ou sensor de velocidade como substituto de PID ausente.
- GNSS, microSD, RTC externo, lógica “IA”, histórico complexo e diagnóstico DTC avançado.
- Limpeza de DTC, serviços de escrita na ECU ou testes de atuadores.

## 2. O que a documentação comercial do TID GM ensina

Fontes públicas consultadas:
- Anúncio do kit GM: https://produto.mercadolivre.com.br/MLB-2666486572-kit-computador-de-bordo-no-tid-astra-corsa-vectra-meriva-mid-_JM
- Manual de instalação v2 (setembro de 2021): https://ro.scribd.com/document/559320851/Instalacao-Kit-Computador-de-Bordo-v2
- Manual de uso e calibração (março de 2021): https://www.scribd.com/document/559320853/Instrucoes-de-uso-e-calibracao-Kit-Computador-de-Bordo-v2
- Tutorial Arduino + TID: https://corsaclube.com.br/viewtopic.php?t=104366
- Tutorial de desbloqueio mencionado pelo manual: https://www.youtube.com/watch?v=4NyPkAGoM6U

Os manuais descrevem uma placa acoplada ao/atrás do TID, navegação por botões R/S (alavanca com comandos ou alternativa por push buttons), desbloqueio/ativação do display e recursos como velocidade, distância, tempo, tensão, consumo, autonomia, temperatura interna, alertas e calibração.

**Diferença essencial:** a instalação documentada pelo kit usa sinal do nível de combustível e sinal do injetor, além de sensor de temperatura e botões. O manual alerta que, em Meriva com BCM, o sinal da boia pode não estar no chicote do painel. Portanto, o produto comercial não demonstra que todos esses parâmetros sejam obtidos por PIDs da ECU.

Para o nosso projeto, esses documentos servem como referência de experiência de uso, navegação, montagem e ativação do display. Não vamos replicar a leitura de combustível pela boia nem consumo por pulso de injetor, porque o requisito é mostrar o que a ECU fornecer.

Os manuais e a descrição pública encontrados não especificam claramente o microcontrolador da revisão comercial atual. Eles **não comprovam, por si só, que esse kit comercial seja a placa ESP32 mencionada pelo usuário**. A placa ESP32 exata continua sendo uma lacuna documental; não copiar firmware/circuito proprietário nem afirmar qual MCU ela usa sem confirmação.

## 3. Referências com ESP32

### EHU32
- Repositório: https://github.com/PNKP237/EHU32
- Esquema: https://github.com/PNKP237/EHU32/blob/main/EHU32_wiring.pdf
- Demonstração de dados: https://www.youtube.com/watch?v=uxLYr1c_TJA

A documentação inclui Meriva A na lista de veículos-alvo, descreve ESP32, transceptor CAN e exibição de dados no display integrado. Porém, a solução depende do rádio original compatível, entrada AUX e integração de mensagens CAN; também usa Bluetooth para áudio e Wi-Fi para atualização. Isso não se encaixa na instalação definida, que usa multimídia aftermarket e operação somente cabeada.

**Decisão:** usar o esquema como referência de integração automotiva, não portar o projeto completo nem depender do rádio original.

### FISCuntrolCAN
- Repositório: https://github.com/adamforbes92/FISCuntrolCAN

Projeto ESP32 que separa aquisição, protocolo, menus e renderização no display. Foi projetado para o display FIS de VW/Audi, com protocolo gráfico e dados específicos; não é um driver compatível automaticamente com o TID GM.

**Decisão:** reutilizar a ideia de modularizar o firmware, não os drivers FIS ou os identificadores CAN/KWP1281 como se fossem da Meriva.

### Biblioteca OBD2 K-Line
- Biblioteca e esquemas: https://github.com/muki01/OBD2_KLine_Library
- Leitor de referência: https://github.com/muki01/OBD2_K-line_Reader

A biblioteca documenta ISO 9141-2 e ISO 14230/KWP2000, incluindo inicialização, quadros, checksums e temporização. A referência genérica é uma conexão física pela K-Line do conector OBD com transceptor entre a linha automotiva e a UART do ESP32.

**Decisão:** avaliar a biblioteca como referência ou dependência isolada para transporte cabeado. Está sob GPL-3.0; antes de incorporar/distribuir código dela, avaliar as obrigações da licença ou considerar uma implementação independente baseada na especificação. Não copiar código sem resolver a licença.

## 4. Arquitetura Rev B

    ECU da Meriva
       │ conexão de diagnóstico cabeada; protocolo a confirmar
       ▼
    Conector OBD/chicote
       ▼
    Proteção + transceptor automotivo K-Line
       ▼ UART com nível lógico compatível com 3,3 V
    ESP32-WROOM-32E
       ├─ descoberta dos PIDs e consultas de leitura
       ├─ parser e fórmulas portados do aplicativo
       ├─ validação, timestamp e plausibilidade
       ├─ páginas e navegação por botões
       └─ driver de escrita do TID (após caracterização)
       ▼
    Interface elétrica TID protegida
       ▼
    TID original GM

A comunicação com a ECU e a escrita no TID são dois subsistemas diferentes. SCL/SDA/MRQ são nomes de sinais de referência, não prova de que o TID use I²C convencional. O driver de escrita fica bloqueado até medir níveis, pull-ups, direções, temporização, sequência de inicialização e quadros reais.

## 5. Comunicação com a ECU

O repositório registra como referência de campo ISO 14230-4 KWP Fast Init, 10.400 baud e endereço ECU 0x11. Esses dados são uma hipótese inicial documentada, não garantia universal para todas as variantes. A identificação precisa ser repetida na ECU real.

A K-Line é uma linha automotiva; nunca deve ser ligada diretamente a GPIO de 3,3 V. A interface deve incluir transceptor adequado, proteção, alimentação protegida e validação em bancada.

Primeiro teste funcional: consulta somente de leitura 01 0C (rotação). O inventário do aplicativo registra a resposta histórica 41 0C 1A F8, equivalente a 1.726 rpm. É um exemplo registrado, não o valor esperado em qualquer teste.

Uma requisição por vez, timeout explícito e resposta bruta preservada para auditoria. Não implementar comandos para limpar DTCs ou controlar atuadores.

## 6. Reaproveitamento do aplicativo de diagnóstico

Reaproveitar a lógica de conhecimento, não a arquitetura Android:
- Catálogo de PIDs, comprimentos de payload, unidades e IDs de fórmulas.
- Fórmulas determinísticas portadas de C++ sem eval.
- Regras do parser para validar serviço/PID, comprimento, bytes, valor finito e plausibilidade.
- Descoberta por bitmaps OBD (0100, 0120, 0140, 0160), com consulta individual após o anúncio.
- Estados e testes de regressão para resposta truncada, PID ausente, timeout, valor inválido e reconexão.

Não portar a sessão Elm327, comandos AT, Bluetooth, telas Android, serviços de GPS ou banco de dados do aplicativo.

### Hierarquia de evidência para cada PID
1. **Catalogado:** definição e fórmula existem.
2. **Anunciado:** bitmap da ECU indica suporte.
3. **Respondido:** consulta individual retorna resposta positiva.
4. **Validado:** comprimento, fórmula e plausibilidade passam.
5. **Confirmado na Meriva:** resposta bruta registrada em teste repetível no carro.
6. **Exibido:** só após confirmação e com timestamp recente.

O inventário local do app tem 34 PIDs no catálogo, mas o arquivo meriva_confirmed_pids.json registra apenas o PID 010C com evidência RAW_ECU. Os outros parâmetros continuam candidatos até nova captura cabeada; dados de Car Scanner servem como pistas, não confirmação atual.

## 7. Páginas iniciais propostas

A lista abaixo é a ordem de investigação, não promessa de compatibilidade:
1. Estado da ligação e da leitura, sem valores fictícios.
2. RPM (010C), primeiro PID.
3. Temperatura do líquido de arrefecimento (0105), se confirmada.
4. Velocidade (010D), se confirmada.
5. Carga (0104), MAP (010B), temperatura de admissão (010F) ou borboleta (0111), conforme suporte.
6. Tensão informada pela ECU (0142), se suportada e plausível.
7. Nível de combustível (012F) e taxa (015E), somente se confirmados; autonomia/consumo ficam N/D se não houver dados suficientes.

Cada parâmetro terá status e timestamp próprios. Valor antigo não pode aparecer como atual; falha de um PID não deve invalidar automaticamente todos os demais. Priorizar poucas páginas, legíveis no TID, e não excesso de funções.

## 8. Conflitos identificados no firmware atual

- O arquivo firmware/v2.0/Meriva_Smart_MID_v2_0_RevA.ino usa BluetoothSerial, declara BluetoothSerial ELM, inicia ELM.begin("MERIVA_MID") e consulta via ELM.hasClient(). Essa camada precisa ser substituída por UART + transceptor direto.
- A arquitetura reserva GPIO16/17 para K-Line, mas o firmware usa os mesmos pinos na UART do GNSS. GNSS sai do primeiro marco para eliminar o conflito.
- O firmware consulta PIDs fixos e marca a telemetria global como válida mesmo quando leituras individuais podem falhar. O novo modelo terá estado por PID e timestamp.
- O código de viagens possui campos para consumo/combustível sem uma aquisição válida demonstrada no trecho auditado. Não importar consumo/autonomia sem validação.
- O firmware declara GPIOs para o TID, mas não implementa o protocolo completo de escrita do display.

Esses pontos precisam de correção antes de chamar o firmware de Rev B funcional.

## 9. Segurança de hardware

- Fonte automotiva protegida com fusível, proteção contra inversão, TVS/transientes, filtragem e regulador especificado para uso veicular.
- Transceptor K-Line dedicado; nenhum sinal de 12 V chega diretamente ao ESP32.
- Interface TID e adaptação de nível definidas por medição, não por tentativa.
- Não cortar/ligar o fio AA/DIS ou outros pinos do TID sem identificar a variante e medir a tensão. A ausência do rádio OEM não determina o comportamento elétrico do display.
- Preferir suporte mecânico removível; não furar o TID por padrão.
- Botões com proteção elétrica e debounce.
- Primeiro teste com fonte de bancada limitada em corrente; depois, teste controlado no veículo.

## 10. Plano de execução e aceite

### Marco A — caracterização
- Identificar variante física do TID, conector e pinagem.
- Confirmar a interface de diagnóstico e protocolo da ECU.
- Capturar passivamente as linhas TID; não transmitir comandos desconhecidos.
- Confirmar como o TID é ativado/desbloqueado e se funciona sem rádio original.

### Marco B — ECU cabeada
- Remover Bluetooth/ELM do runtime.
- Integrar UART e transceptor.
- Confirmar resposta válida de RPM, registrando bytes brutos e tempo.
- Descobrir bitmaps e validar PIDs individualmente.
- Testar payload truncado, PID não suportado, timeout e reconexão.

### Marco C — display
- Documentar níveis, pull-ups, direção, temporização e frames.
- Implementar driver TID com base em capturas validadas.
- Testar em bancada e só depois no carro com página mínima.

### Marco D — integração
- Exibir RPM real; acrescentar um PID por vez.
- Confirmar troca de páginas, perda/reconexão e ignição.
- Garantir ausência de comandos de controle/limpeza para a ECU.
- Só depois avaliar consumo, autonomia e alertas, conforme os PIDs efetivamente disponíveis.

**Rev B será considerada funcional apenas quando:** o firmware compilar sem dependência Bluetooth/ELM, a alimentação e transceptor passarem nos testes, RPM for validado por fio, a escrita do TID for comprovada na variante real e nenhum valor inventado for exibido.

## 11. Referências
- Kit GM TID — instalação: https://ro.scribd.com/document/559320851/Instalacao-Kit-Computador-de-Bordo-v2
- Kit GM TID — uso/calibração: https://www.scribd.com/document/559320853/Instrucoes-de-uso-e-calibracao-Kit-Computador-de-Bordo-v2
- Anúncio do kit: https://produto.mercadolivre.com.br/MLB-2666486572-kit-computador-de-bordo-no-tid-astra-corsa-vectra-meriva-mid-_JM
- Tutorial Arduino/TID: https://corsaclube.com.br/viewtopic.php?t=104366
- EHU32 + esquema: https://github.com/PNKP237/EHU32
- FISCuntrolCAN: https://github.com/adamforbes92/FISCuntrolCAN
- OBD2 K-Line Library: https://github.com/muki01/OBD2_KLine_Library
- OBD2 K-Line Reader: https://github.com/muki01/OBD2_K-line_Reader
- SAE J1979: https://saemobilus.sae.org/standards/j1979_202505-e-e-diagnostic-test-modes

## Limitações da pesquisa

Não foi possível confirmar, nos documentos públicos consultados, o modelo de microcontrolador da revisão comercial atual do kit GM nem identificar sem ambiguidade uma segunda placa ESP32 específica do Mercado Livre. Os manuais foram resumidos para análise; circuitos e firmware proprietários não foram reproduzidos. Nenhuma pinagem, forma de onda, protocolo de display ou circuito definitivo é considerado validado antes de medições no veículo real.
