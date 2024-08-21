import uasyncio
from microdot_asyncio import Microdot, Response
from machine import Pin
import gc
import esp32
from machine import Pin
from neopixel import NeoPixel


# Servindo arquivos grandes corretamente com o Microdot
Response.default_content_type = "text/html"


async def piscar_led_rgb(): 
    pin = Pin(21, Pin.OUT)                    
    np = NeoPixel(pin, 1)                     
    while (True):
        np[0] = (255,0,0)
        np.write()
        await uasyncio.sleep(1)
        np[0] = (0,255,0)
        np.write()
        await uasyncio.sleep(1)
        np[0] = (0,0,255)
        np.write()
        await uasyncio.sleep(1)        

'''
led = Pin(21, Pin.OUT)
async def piscar_led():    
    while True:
        await uasyncio.sleep(1)
        led.on()  
        await uasyncio.sleep(1)
        led.off()
'''

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
    
#uasyncio.create_task(piscar_led())
uasyncio.create_task(piscar_led_rgb())
uasyncio.create_task(print_memory_stats())

app = Microdot()

# Função para carregar arquivos de forma eficiente
def load_file(filename, content_type):
    try:
        with open(filename, "r") as file:
            return file.read(), 200, {"Content-Type": content_type}
    except Exception as e:
        return "Arquivo nao encontrado", 404


# Servir o arquivo HTML principal
@app.route("/")
def index(request):
    return load_file("static/index.html", "text/html")

@app.route("/update")
def index(request):
    return load_file("static/update.html", "text/html")

# Servir o arquivo CSS
@app.route("/style.css")
def style(request):
    return load_file("static/style.css", "text/css")

# Servir o arquivo JavaScript
@app.route("/script.js")
def script(request):
    return load_file("static/script.js", "application/javascript")

@app.route("/favicon.ico")
def script(request):    
    return load_file("data:image/x-icon;base64,AAAB AAAAA".encode('utf-8'), "image/x-icon")


if __name__ == "__main__":   
    app.run(host="0.0.0.0", port=porta_http)
    
    

