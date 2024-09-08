'''
import network, espnow, uasyncio, ujson
from machine import Pin


###########################
# DECLARACAO DE VARIAVEIS #
###########################
ch_counter = 1
max_ch = 12
get_channel = 0
get_cmd = ''
get_msg = ''

#mac_broadcast = b'\xff\xff\xff\xff\xff\xff'   # MAC Broadcast
mac_servidor = b'\x12\xb2\x03\x04\x05\xb1'     # MAC Server


#########################
# CONFIGURACAO DOS GPIO #
#########################
# ESP32C3 - LED = GPIO8
# ESP32S2 - LED = GPIO15
# ESP32S3_zero - RGB LED - GPIO21
led = Pin(15, Pin.OUT)


####################################################
# A WLAN interface must be active to send()/recv() #
####################################################
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()   


#######################
# INICIALIZAR ESP-NOW #
#######################
objespnow = espnow.ESPNow()
objespnow.active(True)
objespnow.add_peer(mac_servidor)


########################
# RECEBE DADOS ESP-NOW #
########################
def recv_cb(e):
    global get_cmd
    global get_msg
    global get_channel
    while True:          
        mac_hex, msg = e.irecv(100)  
        if msg != None:
            # Converter o bytearray para string
            data_str = msg.decode('utf-8')            
            # Desserializar a string JSON em um dicionário Python
            json_dict = ujson.loads(data_str)
            
            # Verifica o Tipo de mensagem
            msg_type = json_dict["MSG"]
            
            if msg_type == 'PAIRED':
                get_channel = json_dict["CHN"] # Guarda o canal em uso                  
            elif msg_type == 'CMD':
                get_cmd = json_dict["CMD"]     # Guarda o comando recebido
                print(get_cmd)
            elif msg_type == 'MSG':
                get_msg = json_dict["MSG"]     # Guarda a mensagem recebida
                
# Regista callback para receber mensangens esp-now
objespnow.irq(recv_cb)


##################
# EMPARELHAMENTO #
##################
async def send_pairing():
    
    global ch_counter
    
    while ch_counter <= max_ch:
        # Envia dados pelos canais disponiveis
        sta.config(channel=ch_counter)
        objespnow.send(mac_servidor, b'PAIR_CHN')
        
        # Percorrer os canais WiFi disponiveis
        ch_counter += 1 
        if ch_counter > 12:            
            ch_counter = 1
        await uasyncio.sleep(1) 


#################################
# PISCAR LED ENQUANTO EMPARELHA #
#################################
async def piscar_led():    
    while True:
        led.value(not led.value())   
        await uasyncio.sleep_ms(100)



####################################################        
# Loop principal para rodar as funções assíncronas #
####################################################
async def main():
    task_pairing = uasyncio.create_task(send_pairing())
    task_led = uasyncio.create_task(piscar_led())
    
    while True:
        if get_channel != 0:             
             # Configurar Canal WiFi
             sta.config(channel=get_channel)
             
             # Terminar Tarefas de emparelhamento
             task_pairing.cancel()
             task_led.cancel()             
             
             # Terminar o Ciclo             
             break
        await uasyncio.sleep(1)          

# Executa o loop principal
uasyncio.run(main())'''

        