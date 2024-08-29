import network, espnow, uasyncio, time
from machine import Pin

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
sta.config(channel=4)
sta.disconnect()   # apena para ESP8266 


#######################
# INICIALIZAR ESP-NOW #
#######################
e = espnow.ESPNow()
e.active(True)

#mac_servidor = b'\xff\xff\xff\xff\xff\xff'  # MAC Broadcast
mac_servidor = b'\x12\xb2\x03\x04\x05\xb1'   # MAC Server
e.add_peer(mac_servidor)

##################
# EMPARELHAMENTO #
##################
async def send_pairing():
    while True:
        e.send( mac_servidor, b'ASK_PAIRING', True)        
        await uasyncio.sleep(2) 


async def piscar_led():
    while True:
        led.value(not led.value())   
        await uasyncio.sleep_ms(100)  

uasyncio.create_task(send_pairing())
uasyncio.run(piscar_led())
        


