import espnow, time, network, uasyncio
from machine import Pin

sta = network.WLAN(network.STA_IF)    # Enable station mode for ESP
sta.active(True)
sta.disconnect()        # Disconnect from last connected WiFi SSID


e = espnow.ESPNow()     # Enable ESP-NOW
e.active(True)

led = Pin(8, Pin.OUT)

peer_mac = b'\x80\x65\x99\xfb\x3d\x62'   # MAC ESP2S2 Client
e.add_peer(peer_mac)

async def main_task():
    while True:
        print("Estado do Pin = %s" % led.value())        
        await uasyncio.sleep(1)  # Espera 1 segundo antes de enviar novamente


async def send_task():
    while True:
        msg = b'walk'
        try:
            e.send(peer_mac, msg)
            led.value(not led.value())
        except OSError as err:
            print("Erro ao enviar mensagem:", err)        
        await uasyncio.sleep(2)  # Espera 1 segundo antes de enviar novamente

uasyncio.create_task(main_task())
uasyncio.run(send_task())

'''
def envia_dados():
    while True:
        e.send(peer_mac, "walk", True)
        led.value(not led.value())     
        time.sleep(2)
        
envia_dados()
'''