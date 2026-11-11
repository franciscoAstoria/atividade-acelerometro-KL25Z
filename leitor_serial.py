import csv
import serial

PORTA_SERIAL = 'COM5'
BAUD_RATE = 115200
ARQUIVO_CSV = 'dados_cinematicos.csv'
COLUNAS = ['t_ms', 'ax', 'ay', 'az', 'vx', 'vy', 'vz', 'px', 'py', 'pz']


def iniciar_leitura():
    quantidade = 0
    aguardando_reset = True

    try:
        with serial.Serial(PORTA_SERIAL, BAUD_RATE, timeout=1) as conexao:
            conexao.reset_input_buffer()
            with open(ARQUIVO_CSV, 'w', newline='', encoding='utf-8') as arquivo:
                escritor = csv.writer(arquivo)
                escritor.writerow(COLUNAS)
                arquivo.flush()

                print(f'Conectado em {PORTA_SERIAL} a {BAUD_RATE} baud.')
                print('Pressione RESET na placa para zerar o tempo e os calculos.')
                print(f'Gravando em {ARQUIVO_CSV}. Use Ctrl+C para encerrar.')

                while True:
                    linha = conexao.readline().decode('utf-8', errors='replace').strip()
                    if not linha or linha.startswith('#'):
                        continue

                    campos = linha.split(',')
                    if len(campos) != len(COLUNAS):
                        continue

                    try:
                        valores = [float(campo) for campo in campos]
                    except ValueError:
                        continue

                    if aguardando_reset:
                        if valores[0] != 0:
                            continue
                        aguardando_reset = False

                    escritor.writerow(valores)
                    arquivo.flush()
                    quantidade += 1
                    print(f'Amostras gravadas: {quantidade}', end='\r')

    except serial.SerialException as erro:
        print(f'\nErro na serial: {erro}')
        print('Confira a porta COM e feche o monitor serial do PlatformIO.')
    except OSError as erro:
        print(f'\nErro ao gravar o arquivo: {erro}')
    except KeyboardInterrupt:
        print('\nCaptura encerrada.')

    print(f'Total: {quantidade} amostras em {ARQUIVO_CSV}.')


if __name__ == '__main__':
    iniciar_leitura()
