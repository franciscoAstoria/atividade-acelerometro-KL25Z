import csv
import matplotlib.pyplot as plt

ARQUIVO_CSV = 'dados_cinematicos.csv'
ARQUIVO_GRAFICO = 'graficos_atividade2.png'
COLUNAS = ['t_ms', 'ax', 'ay', 'az', 'vx', 'vy', 'vz', 'px', 'py', 'pz']


def gerar_graficos():
    dados = {coluna: [] for coluna in COLUNAS}

    try:
        with open(ARQUIVO_CSV, newline='', encoding='utf-8') as arquivo:
            leitor = csv.DictReader(arquivo)
            if leitor.fieldnames != COLUNAS:
                print('Cabecalho diferente do esperado. Use o leitor_serial.py atualizado.')
                return

            for linha in leitor:
                try:
                    valores = [float(linha[coluna]) for coluna in COLUNAS]
                except (ValueError, TypeError):
                    continue
                for coluna, valor in zip(COLUNAS, valores):
                    dados[coluna].append(valor)
    except OSError as erro:
        print(f'Erro ao abrir o CSV: {erro}')
        return

    if len(dados['t_ms']) < 2:
        print('Capture pelo menos duas amostras antes de gerar os graficos.')
        return

    tempo_inicial = dados['t_ms'][0]
    tempo = [(valor - tempo_inicial) / 1000.0 for valor in dados['t_ms']]
    fig, eixos = plt.subplots(3, 1, figsize=(10, 10), sharex=True)

    grupos = [
        (['ax', 'ay', 'az'], 'Aceleracao medida (inclui gravidade)', 'Aceleracao (m/s²)'),
        (['vx', 'vy', 'vz'], 'Integral da aceleracao bruta: velocidade estimada', 'Velocidade (m/s)'),
        (['px', 'py', 'pz'], 'Segunda integral: posicao estimada', 'Posicao (m)'),
    ]

    for eixo, (colunas, titulo, unidade) in zip(eixos, grupos):
        for coluna, cor in zip(colunas, ['tab:red', 'tab:green', 'tab:blue']):
            eixo.plot(tempo, dados[coluna], label=coluna, color=cor)
        eixo.set_title(titulo)
        eixo.set_ylabel(unidade)
        eixo.legend(loc='best')
        eixo.grid(True, alpha=0.3)

    eixos[2].set_xlabel('Tempo aproximado desde o inicio da captura (s)')
    fig.suptitle('MMA8451Q — integracao pela regra dos trapezios')
    fig.tight_layout()
    fig.savefig(ARQUIVO_GRAFICO, dpi=200)
    print(f'Grafico salvo em {ARQUIVO_GRAFICO}.')
    print(f'Amostras utilizadas: {len(tempo)}.')
    plt.show()


if __name__ == '__main__':
    gerar_graficos()
