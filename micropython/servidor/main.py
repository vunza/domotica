from microdot.microdot_asyncio import Microdot, Response
from websocket import with_websocket
from machine import Pin
import uasyncio
import ujson


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
            print(message)
            #led.value(not led.value())
            #datajson['Estado'] = led.value()
            #datajson['Id'] = message
            print(f'Recebido: {message}')
            for client in connected_clients:                
                #json_data = str(json.dumps(datajson))
                await client.send(message)
    except Exception as e:
        print(f'Erro: {e}')
    finally:
        connected_clients.remove(ws)
        print(f'Cliente desconectado: {len(connected_clients)} restantes')

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True, port=porta_http)
    
    
