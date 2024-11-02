import paho.mqtt.client as mqtt

#from  modulos.devs_list import get_devs_array

import json

device_list = []

client = None
# qtt_msg = None


####################################
# Definindo as funções de callback #
####################################
def on_connect(client, userdata, flags, rc):   
    client.subscribe("zigbee2mqtt/#")

def on_message(client, userdata, msg):
    # mqtt_msg = (f"{msg.topic}: {msg.payload.decode()}")  
    #print(mqtt_msg)    
    get_devs_array(msg)


          
######################
# Obter dispositivos #
######################
def get_devs_array(mensagem):  
    if(mensagem.topic == "zigbee2mqtt/bridge/devices"):        
        devices = mensagem.payload.decode()
        devs_json = json.loads(devices)
        # total_devs = len(devs_json)
        for index, device in enumerate(devs_json):
            # Descarta o coordenador
            if device in devs_json and device.get('friendly_name') != 'Coordinator':
                # Cria lista de devices (p/ devs simple) e subdevices (p/ devs duplo..)
                total_gangs = len(device.get('endpoints'))
                for cont in range(total_gangs):
                    device_list.append({
                        "ieeeAddress": f"{device['ieee_address']}{cont + 1}",
                        "friendlyName": device["friendly_name"]
                    }) 

        print(device_list)                


###########################
# Criando um cliente MQTT #
###########################
client = mqtt.Client()


#####################################
# Atribuindo as funções de callback #
#####################################
client.on_connect = on_connect
client.on_message = on_message


########################################################################
# Conectando ao broker (substitua pelo endereço e porta do seu broker) #
########################################################################
client.connect("localhost", 1883, 60)

###########################################################
# Mantendo o cliente em execução para processar mensagens #
###########################################################
client.loop_start()