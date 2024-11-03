# import json
from flask import Flask, request, render_template
from modulos.init_mqtt import client, arquive_path, ler_dados_json


app = Flask(__name__)

########################################################################
# Conectando ao broker (substitua pelo endereço e porta do seu broker) #
########################################################################
client.connect("localhost", 1883, 60)


#####################
# Servir página Web #
#####################
@app.route('/')
def index():
    return render_template('index.html')


############################
# Processa dados recebidos #
############################
@app.route('/tx_rx__dados', methods=['POST'])
def receber_dados():
    cmnd = request.form['comando']
    if(cmnd == 'DEVS_LIST'):
        return ler_dados_json(arquive_path)



if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080, debug=True)
