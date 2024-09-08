from microdot.microdot_asyncio import Microdot, Response
from websocket import with_websocket
from machine import Pin
import uasyncio, ujson, espnow
import ubinascii


# ESP32C3 - LED = GPIO8
# ESP32S2 - LED = GPIO15
# ESP32S3_zero - RGB LED - GPIO21
led = Pin(8, Pin.OUT)

print('Canal atual:', canal_wifi)


###########
# ESP-NOW #
###########
e = espnow.ESPNow()     
e.active(True)
peer_mac = b'\x80\x65\x99\xfb\x3d\x62'   # MAC ESP2S2 Client
e.add_peer(peer_mac) 

######################
# Variaveis/Objectos #
######################
datajson = {}  
app = Microdot()
websocket_clients = []
esp_now_clients = []

################################
# CONVERTE MAC STRING PARA HEX #
################################
def mac_str_to_hex(mac_str):   
    hex_pairs = [mac_str[i:i+2] for i in range(0, len(mac_str), 2)]
    formatted_mac = '\\x' + '\\x'.join(hex_pairs)    
    return formatted_mac


########################
# RECEBE DADOS ESP-NOW #
########################
mac_str = ''
msg_aswr = ''
sender_mac = ''
def recv_cb(e):
    while True:  # Read out all messages waiting in the buffer
        mac_hex, msg = e.irecv(0)  # Don't wait if no messages left
        if mac_hex is None:
            return
        elif msg == b'PAIR_CHN':
            led.value(not led.value())
            mac_str = ubinascii.hexlify(mac_hex).decode()
            sender_mac = bytes.fromhex(mac_str)
            print(mac_str)
            # Guardar cliete esp-nao
            if sender_mac not in esp_now_clients:
                esp_now_clients.append(sender_mac)                
                e.add_peer(sender_mac)
            # Envia resposta esp-now para emparelhamento
            respjson = {}
            respjson['CHN'] = canal_wifi
            respjson['MSG'] = 'PAIRED'
            dadosjson = str(ujson.dumps(respjson))
            e.send(sender_mac, dadosjson, True)                

e.irq(recv_cb)


##########################
# ENVIO DE DADOS ESP-NOW #
##########################
async def send_msg_aswr():
    while True:
        if True:#msg_aswr == 'PAIRING_DONE':            
            respjson = {}
            respjson['CMD'] = 'TGL'
            respjson['MSG'] = 'CMD'
            dadosjson = str(ujson.dumps(respjson))
            e.send(sender_mac, dadosjson, True)
            await uasyncio.sleep(1)  
                
uasyncio.create_task(send_msg_aswr())      
                
        

#######################
# MANIPULAR WEBSOCKET #
#######################
@app.route('/')
@with_websocket
async def handle_websocket(request, ws: WebSocket):
    websocket_clients.append(ws)
    print(f'Cliente conectado: {len(websocket_clients)} total')

    try:
        while True:
            message = await ws.receive()            
            datajson['Estado'] = led.value()
            datajson['Id'] = message
            json_data = str(ujson.dumps(datajson))
            #print(f'Recebido: {message}')           
            for client in connected_clients:                
                await client.send(json_data)                
    except Exception as e:
        print(f'Erro: {e}')
    finally:
        websocket_clients.remove(ws)
        print(f'Cliente desconectado: {len(websocket_clients)} restantes')

if __name__ == '__main__':    
    app.run(host='0.0.0.0', debug=True, port=porta_http)
    
    
