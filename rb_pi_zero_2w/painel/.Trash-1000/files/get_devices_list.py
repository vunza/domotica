import json

device_list = []

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