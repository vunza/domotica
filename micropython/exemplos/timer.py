from machine import Pin, Timer

led = Pin(12, Pin.OUT)

tmr0 = Timer(0) # ESP32 tem 3 Timers (0-)

# Com Lambada
#tmr0.init(period=2000, mode=Timer.PERIODIC, callback=lambda t: led.value(not led.value())) # Cada x milisegundos
tmr0.init(period=2000, mode=Timer.ONE_SHOT, callback=lambda t: led.value(not led.value())) # Uma vez

# Com callback
def handle_timer(timer):
    led.value(not led.value())
    
tmr0.init(period=1000, mode=Timer.PERIODIC, callback=handle_timer)

