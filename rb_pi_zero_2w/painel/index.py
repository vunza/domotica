import json, asyncio, time
from flask import Flask, request, render_template, Response, stream_with_context
from modulos.init_mqtt import client, arquive_path, device_list, device_state
from modulos.init_mqtt import ler_dados_json, renomear_devs

app = Flask(__name__)


########################################################################
# Conectando ao broker (substitua pelo endereço e porta do seu broker) #
########################################################################
client.connect("localhost", 1883, 60)



######################################################
# Enviar dados, cada X segundos, para todos clientes #
######################################################
''' Função para enviar dados a cada X segundos '''
def generate_data():
    while True:
        time.sleep(5)  # Intervalo de 5 segundos
        #yield f"data: Atualização do servidor em {time.ctime()}\n\n"  
        jsonobj = json.dumps(device_state)   
        yield f"data: {jsonobj}\n\n"

''' Endpoint de SSE para os clientes se conectarem '''
@app.route('/events') 
def events():
    return Response(stream_with_context(generate_data()), content_type='text/event-stream')
    


#################################
# Obter estado dos dispositivos #
#################################
async def ask_dev_state(): 
    for device in device_list: 
        id_orig = device["ieeeAddress"][:-1]
        topic = (F"zigbee2mqtt/{id_orig}/get")
        payload = '{"state":""}'
        client.publish(topic, payload)        
        await asyncio.sleep(1/2)

      


#####################
# Servir página Web #
#####################
@app.route('/')
def index():
    asyncio.run( ask_dev_state() ) 
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
        # Retorna, ao browser, o estado dos dispositivos
        return device_state
    elif( '{"state":"TOGGLE"}' in cmnd):
        msg = cmnd.split(", ", 1)
        client.publish(msg[0], msg[1]) 
        #print(device_state)         
        return device_state
    elif( '{"from":' in cmnd and '"to":' in cmnd):       
        renomear_devs(cmnd)       
        return device_state
    


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080, debug=True)
