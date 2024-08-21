from machine import Pin

led = Pin(12, Pin.OUT)
sw = Pin(0, Pin.IN)

def handle_interrupt(pin):
	led.value(not led.value())

sw.irq(trigger=Pin.IRQ_FALLING, handler=handle_interrupt)
