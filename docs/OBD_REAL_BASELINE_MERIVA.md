# Base OBD real da Meriva

## Equipamento e protocolo observados

- Veículo: Chevrolet Meriva 1.4 Maxx 8V, ECONO.FLEX.
- ELM327 de trabalho: unidade descrita como v1.5.
- Unidade descrita como v2.0: não funcionou na mesma Meriva.
- Protocolo observado no Car Scanner: ISO 14230-4 KWP fast init.
- Comunicação observada: 10,4 kbaud.
- Endereço ECU observado: 11.

## Snapshot de 24/09/2026

| PID / dado | Valor |
|---|---:|
| Coolant | 81 °C |
| STFT B1 | -8,59 % |
| LTFT B1 | +10,94 % |
| MAP | 5,66 psi |
| RPM | 778 |
| Timing | 8 ° |
| IAT | 33 °C |
| MAF | 8,93 kg/h |
| Throttle | 3,53 % |
| O2S1B1 | 0,53 V |

## PIDs prioritários do projeto

| PID | Grandeza |
|---|---|
| 010C | RPM |
| 010D | Velocidade |
| 0105 | Temperatura do líquido |
| 0142 | Tensão ECU |
| 0106 | STFT B1 |
| 0107 | LTFT B1 |
| 0104 | Carga |
| 010B | MAP |
| 0110 | MAF |
| 0111 | Borboleta |
| 010F | Temperatura do ar |
| 012F | Nível de combustível |
| 015E | Fuel rate, quando suportado |

## Regras

1. Não transformar ausência de resposta em zero.
2. Guardar qualidade e idade da leitura.
3. Confirmar cada PID na ECU real.
4. O `012F` não confirma sozinho abastecimento.
5. O `015E` só pode ser usado para consumo quando houver resposta real.
6. Valores históricos são referência de diagnóstico, não constantes de calibração.
