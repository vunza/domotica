import network
import machine, time
p=machine.Pin(2, machine.Pin.OUT)

ssid = 'TPLINK'
passwd = 'gregorio@2012'

wlan = network.WLAN(network.STA_IF)
wlan.active(True)  # activa wifi
wlan.connect(ssid, passwd)

while wlan.isconnected() == False:
    p.on()
    time.sleep_ms(100)
    p.off()
    time.sleep_ms(100)
    print('Conectando...')
    pass

print('Coxeão a rede WiFi %s estabelecida' % ssid)
print(wlan.ifconfig())  # Mostra dados da conexão

p.off()
