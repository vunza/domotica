
from flask import Flask, request, render_template
from modulos.connect_mqtt import client
import json

app = Flask(__name__)

#####################
# Servir página Web #
#####################
@app.route('/')
def index():
    return render_template('index.html')


############################
# Processa dados recebidos #
############################
@app.route('/receber_dados', methods=['POST'])
def receber_dados():
    dados = request.form['dados']
    print(f'Dados recebidos: {dados}')
    # Publicando uma mensagem no tópico
   
    obj = {'sensor1': 25, 'sensor2': 30}
    salvar_dados_json('dados.json', obj)
    obj_json = ler_dados_json('dados.json')
   
    return 'mqtt.resultado'


# Para salvar dados em formato JSON
def salvar_dados_json(nome_arquivo, dados):
    with open(nome_arquivo, 'w') as arquivo:
        json.dump(dados, arquivo)

# Para ler dados em formato JSON
def ler_dados_json(nome_arquivo):
    with open(nome_arquivo, 'r') as arquivo:
        return json.load(arquivo)
       

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080, debug=True)
