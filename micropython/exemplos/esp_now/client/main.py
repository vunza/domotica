import network, espnow, uasyncio
from machine import Pin

# A WLAN interface must be active to send()/recv()
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()   # Because ESP8266 auto-connects to last Access Point

e = espnow.ESPNow()
e.active(True)

#peer = b'\x34\xb7\xda\x53\x1c\x18'   # MAC ESP2S3 Server
#e.add_peer(peer)                     

led = Pin(15, Pin.OUT)
        
def recv_cb(e):
    while True:  # Read out all messages waiting in the buffer
        mac, msg = e.irecv(0)  # Don't wait if no messages left
        if mac is None:
            return
        elif msg == b'walk':
            led.value(not led.value())

e.irq(recv_cb)

async def main_task():
    while True:
        print("Esta do Pin = %s" % led.value())
        await uasyncio.sleep(1)

uasyncio.run(main_task())

