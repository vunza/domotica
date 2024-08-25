import machine, time
p=machine.Pin(1, machine.Pin.OUT)
while 1:
    p.on()
    time.sleep_ms(1000)
    p.off()
    time.sleep_ms(1000)
