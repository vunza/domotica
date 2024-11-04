# import json
from flask import Flask, request, render_template
from modulos.init_mqtt import client, arquive_path, ler_dados_json, device_list, device_state


app = Flask(__name__)

########################################################################
# Conectando ao broker (substitua pelo endereço e porta do seu broker) #
########################################################################
client.connect("localhost", 1883, 60)


#################################
# Obter estado dos dispositivos #
#################################
def ask_dev_state(): 
    for device in device_list: 
        id_orig = device[:-1]
        topic = (F"zigbee2mqtt/${id_orig}/get")
        payload = '{"state":""}'
        client.publish(topic, payload)
       


#####################
# Servir página Web #
#####################
@app.route('/')
def index():
    return render_template('index.html')


#########################################
# Processa dados recebidos dos Browsers #
#########################################
@app.route('/tx_rx__dados', methods=['POST'])
def receber_dados():
    cmnd = request.form['comando']
    if(cmnd == 'DEVS_LIST'):
        # Retorna, ao browser, a lista dos dispositivos
        return ler_dados_json(arquive_path)
    elif(cmnd == 'DEVS_STATE'):
        #return ler_dados_json(arquive_path)
        print(device_state)
        # Retorna, ao browser, o estado dos dispositivos
        return device_state


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080, debug=True)
