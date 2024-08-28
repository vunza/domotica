from microdot.microdot_asyncio import Microdot, Response
from websocket import with_websocket
from machine import Pin
import uasyncio, ujson, espnow


# ESP32C3 - LED = GPIO8
# ESP32S2 - LED = GPIO15
# ESP32S3_zero - RGB LED - GPIO21
led = Pin(8, Pin.OUT)
'''async def piscar_led():    
    while True:
        await uasyncio.sleep(5)
        led.value(not led.value())      
        
uasyncio.create_task(piscar_led())'''

###########
# ESP-NOW #
###########
espnow = espnow.ESPNow()     
espnow.active(True)
peer_mac = b'\x80\x65\x99\xfb\x3d\x62'   # MAC ESP2S2 Client
espnow.add_peer(peer_mac)

######################
# Variaveis/Objectos #
######################
datajson = {}  
app = Microdot()
connected_clients = []


##########################
# ENVIO DE DADOS ESP-NOW #
##########################
async def send_task():
    while True:
        msg = b'walk'
        try:
            espnow.send(peer_mac, msg, True)
            led.value(not led.value())
        except OSError as err:
            print("Erro ao enviar mensagem:", err)        
        await uasyncio.sleep(5)  

uasyncio.create_task(send_task())      
        

def envia_dados():
    espnow.send(peer_mac, "walk", True)
    led.value(not led.value()) 
        
        

#######################
# MANIPULAR WEBSOCKET #
#######################
@app.route('/')
@with_websocket
async def handle_websocket(request, ws: WebSocket):
    connected_clients.append(ws)
    print(f'Cliente conectado: {len(connected_clients)} total')

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
        connected_clients.remove(ws)
        print(f'Cliente desconectado: {len(connected_clients)} restantes')

if __name__ == '__main__':    
    app.run(host='0.0.0.0', debug=True, port=porta_http)
    
    
