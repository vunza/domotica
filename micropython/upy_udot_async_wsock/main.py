import uasyncio
from microdot_asyncio import Microdot, Response
from websocket import with_websocket
from machine import Pin
import gc
import esp32
from neopixel import NeoPixel


# Servindo arquivos grandes corretamente com o Microdot
Response.default_content_type = "text/html"

# S3_zero RGB LED - GPIO21
async def piscar_led_rgb(): 
    pin = Pin(18, Pin.OUT)                    
    np = NeoPixel(pin, 1)                     
    while (True):
        np[0] = (127,0,0)
        np.write()
        await uasyncio.sleep(1)
        np[0] = (0,127,0)
        np.write()
        await uasyncio.sleep(1)
        np[0] = (0,0,127)
        np.write()
        await uasyncio.sleep(1)        


led = Pin(8, Pin.OUT)
async def piscar_led():    
    while True:
        await uasyncio.sleep(1)
        led.on()  
        await uasyncio.sleep(1)
        led.off()


# Obtener estadísticas de la memoria
async def print_memory_stats():
    while True:
        gc.collect()  # Ejecuta una recolección de basura
        mem_free = gc.mem_free()  # Memoria libre en bytes
        mem_alloc = gc.mem_alloc()  # Memoria usada en bytes
        mem_total = mem_free + mem_alloc  # Memoria total
        
        print(f"Total: {mem_total/1024} Kb")
        print(f"Usada: {mem_alloc/1024} Kb")
        print(f"Livre: {mem_free/1024} Kb")
        print('Temperatura Interna: %s°C' % esp32.mcu_temperature())           
        await uasyncio.sleep(1)
    
uasyncio.create_task(piscar_led())
#uasyncio.create_task(piscar_led_rgb())
# uasyncio.create_task(print_memory_stats())

app = Microdot()

@app.route("/")
def index(request):
    return "", 200, {"Content-Type": "text/plain"}


@app.route('/ws')
@with_websocket
async def echo(request, ws):
    while True:
        data = await ws.receive()
        print(data)
        dados = ('Temperatura Interna: %s°C' % esp32.mcu_temperature()) 
        await ws.send(dados)


if __name__ == "__main__":   
    app.run(host="0.0.0.0", port=porta_http)
    
    


