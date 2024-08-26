from microdot.microdot_asyncio import Microdot, Response
from websocket import with_websocket
from machine import Pin
import uasyncio
import ujson


# ESP32C3 - LED = GPIO8
# ESP32S2 - LED = GPIO5
# ESP32S3_zero - RGB LED - GPIO21
led = Pin(15, Pin.OUT)
async def piscar_led():    
    while True:
        await uasyncio.sleep(5)
        led.value(not led.value())      
        
uasyncio.create_task(piscar_led())

datajson = {}  # Dicionário vazio

app = Microdot()
connected_clients = []

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
    
    
