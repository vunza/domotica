import json, os
import paho.mqtt.client as mqtt

device_list = []
device_state = []
arquive_path = 'static/json/lista_dispositivos.json'
client = None
#mqtt_msg = None


############################
# Callback subscribe topic #
############################
def on_connect(client, userdata, flags, rc):   
    client.subscribe("zigbee2mqtt/#")


#############################
# Callback receber ensagens #
#############################
def on_message(client, userdata, msg):
    # mqtt_msg = (f"{msg.topic}: {msg.payload.decode()}")  
    #mqtt_msg = msg    
    get_mqtt_messages(msg)


###################################################
# Para salvar, num arquivo, dados em formato JSON #
###################################################
def update_lista_dispositivos(nome_arquivo, dados):
    # Criar arquivo se nao existe   
    if not os.path.exists(nome_arquivo):
        salvar_dados_json(nome_arquivo, dados) 
    # Se existe e contem uma lista, actualiza os dados    
    elif isinstance(ler_dados_json(arquive_path), list):       
        old_lista = ler_dados_json(arquive_path)
        new_list = dados
        # Actualiza lista antiga, com novos dados recebidos
        for new_item in new_list:
            ieee_existe = any(old_item['ieeeAddress'] == new_item['ieeeAddress'] for old_item in old_lista)
            if(ieee_existe == False):
                old_lista.append(new_item)               
                # Guarda lista actualizada
                salvar_dados_json(nome_arquivo, old_lista)


###################################################
# Para salvar, num arquivo, dados em formato JSON #
###################################################
def salvar_dados_json(nome_arquivo, dados):       
    with open(nome_arquivo, 'w') as arquivo:
        json.dump(dados, arquivo)
     


############################################
# Ler dados, de um aquivo, em formato JSON #
############################################
def ler_dados_json(nome_arquivo):
    with open(nome_arquivo, 'r') as arquivo:
        return json.load(arquivo)
    
          
######################
# Obter dispositivos #
######################
def get_mqtt_messages(mensagem):  
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

                    # Cria array dosestados dos dispositivos
                    ieee2check = f"{device['ieee_address']}{cont + 1}"
                    ieee_existe = any(obj['ieeeAddress'] == ieee2check for obj in device_state)
                    if(ieee_existe == False):
                            device_state.append({
                                "ieeeAddress": f"{device['ieee_address']}{cont + 1}",
                                "friendlyName": f"{device['friendly_name']}",
                                "deviceState": ""
                            })  
                
                #print(device_state)



        # Actualiza a lista de dispositivos
        update_lista_dispositivos(arquive_path, device_list)  

    elif(mensagem.topic != "zigbee2mqtt/bridge/devices" and ('zigbee2mqtt/0x' in mensagem.topic) and ('TOGGLE' not in mensagem.payload.decode()) and ('get' not in mensagem.topic) and ('availability' not in mensagem.topic)  and ('Cordinator' not in mensagem.topic) and ('online' not in mensagem.topic) and ('TOGGLE' not in mensagem.topic)):    
        '''
            topico, payload => zigbee2mqtt/0xa4c138e342bdfb48, {"state":"OFF"}
            topico, payload => zigbee2mqtt/0xa4c138164da7cfb7 {"state_left":"OFF","state_right":"ON"}
        '''
        guardar_estado_devs(mensagem.topic, mensagem.payload)



##################################
# Guarda estado dos dispositivos #
##################################
def guardar_estado_devs(topico, payload):
    substring = topico.split('/')[-1]
    #print(substring)
    #zigbee2mqtt/0xa4c138e342bdfb48 {"state":"ON"}
    #zigbee2mqtt/0xa4c138164da7cfb72 {"state_left":"OFF","state_right":"ON"}
    for cont in range(len(device_state)):
        objecto = json.loads(payload) 
        device = device_state[cont]
        if( substring in device['ieeeAddress'] and "state" in objecto):
            device['deviceState'] = objecto["state"]
        elif (f"{substring}{1}" == device['ieeeAddress'] and "state_left" in objecto):  
            device['deviceState'] = objecto["state_left"]  
        elif (f"{substring}{2}" == device['ieeeAddress'] and "state_center" not in objecto):
            device['deviceState'] = objecto["state_right"]  
        elif (f"{substring}{2}" == device['ieeeAddress'] and "state_center" in objecto):
            device['deviceState'] = objecto["state_center"]   
        elif (f"{substring}{3}" == device['ieeeAddress'] and "state_center" in objecto):
            device['deviceState'] = objecto["state_right"]      

    #print(device_state)




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
''' (*J*) ESTE METODO É CHAMADO NO MODULO index.py 
client.connect("localhost", 1883, 60)
'''

###########################################################
# Mantendo o cliente em execução para processar mensagens #
###########################################################
client.loop_start()