# Atividade Acelerometro - Serial

FRDM-KL25Z com PlatformIO e Zephyr 4.2.

## Etapa 1: leitura do sensor

- MMA8451Q no I2C0, endereco 0x1d, PTE24 (SCL) e PTE25 (SDA).
- Driver FXOS8700 em modo somente acelerometro.
- Sensor configurado em 100 Hz pela API do Zephyr.
- O programa le, envia os valores e espera 50 ms para repetir (aproximadamente
  20 leituras por segundo; a leitura e a impressao tambem levam um pequeno tempo).
- Serial em 115200 baud. Formato: `t_ms,ax,ay,az`.
- Tempo aproximado por um contador: 0, 50, 100, 150 ms, como no exemplo do
  professor. O contador nao inclui o tempo gasto lendo e enviando os dados.
- Aceleracoes em m/s2, incluindo gravidade.
- Ainda nao ha threads adicionais, calibracao, velocidade ou posicao nesta etapa.

No terminal PlatformIO do VS Code:

```powershell
pio run --target clean
pio run
pio run --target upload
pio device monitor --baud 115200
```

A limpeza inicial descarta os arquivos de compilacao copiados do projeto antigo.
Conecte o cabo na USB OpenSDA. Abra o monitor e pressione RESET para ver
o cabecalho. Com a placa parada e aproximadamente horizontal, X e Y devem
ficar proximos de zero e o modulo de Z proximo de 9,81 m/s2. Ao virar a placa,
a gravidade muda de eixo/sinal. Pequenos desvios sao esperados.

Se `pio` nao estiver no PATH, neste computador pode ser usado assim:

```powershell
& 'C:\.platformio\penv\Scripts\platformio.exe' run
```

O usuario confirmou a leitura funcionando na placa apos a correcao da
configuracao de frequencia para `SENSOR_CHAN_ALL`. A mudanca mais recente
substitui o relogio por um contador simples e ainda precisa ser compilada e
enviada novamente. Nesta sessao a compilacao automatica foi bloqueada pela
permissao de escrita em `C:\.platformio\platforms.lock`.

Esta copia nao possui historico Git nem remoto configurado. Inicialize o
repositorio pelo Controle do Codigo-Fonte do VS Code e crie o branch
`atividade/acelerometro`. Sugestao para o primeiro commit apos o teste:
`Configura leitura do MMA8451Q e envio serial`. Nao inclua `.pio` no commit.

## Proximas etapas

1. Validar esta leitura na placa e registrar a base no GitHub.
2. Separar aquisicao, integracao por trapezios e transmissao em tres threads.
3. Remover o vetor medio de repouso antes da integracao, mantendo a orientacao
   da placa durante o movimento. Velocidade e posicao iniciais serao zero.
4. Adaptar os scripts Python para aceleracao, velocidade e posicao.
5. Capturar dados reais e gerar os graficos para entrega.

A integracao acumula erros do sensor. A calibracao inicial vale para uma
orientacao fixa; inclinar a placa depois dela introduz gravidade na estimativa.

## Scripts fornecidos pelo professor

Os scripts foram conferidos, mas ainda nao foram adicionados/adaptados nesta etapa.
O leitor fornecido espera exatamente nove colunas:
`ax,ay,az,vx,vy,vz,px,py,pz`. Ele ainda nao corresponde a esta etapa.
O gerador de graficos usa um intervalo fixo de 50 ms, a mesma aproximacao
escolhida para o contador do programa.

Para preparar um ambiente Python separado, no terminal deste projeto:

```powershell
& 'C:\.platformio\penv\Scripts\python.exe' -m venv .venv
.\.venv\Scripts\python.exe -m pip install pyserial pandas matplotlib numpy
.\.venv\Scripts\python.exe -m serial.tools.list_ports
```

Depois de os scripts estarem na pasta e o firmware correspondente estar pronto:

```powershell
.\.venv\Scripts\python.exe leitor_serial.py
.\.venv\Scripts\python.exe gerar_graficos.py
```

Edite `PORTA_SERIAL` para a porta listada da placa e confira `ARQUIVO_CSV`.
Feche o monitor serial do PlatformIO antes de abrir a porta no Python.
O leitor original abre o CSV em modo `w`, substituindo um arquivo de mesmo nome.
Use Ctrl+C para encerrar. Nao use a captura nesta etapa esperando
velocidade/posicao: a adaptacao sera feita depois da versao com threads.
