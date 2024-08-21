from machine import Pin, UART

uart = UART(0, 115200) # 0 = (tx0, rx0)
uart.init(115200, bits=8, parity=None, stop=1)

led = Pin(12, Pin.OUT)

ch = b''
while True:
	if uart.any() > 0:
		ch = uart.readline()
		if ch ==b'on':
			uart.write('Led On!')
			led.on()
		elif ch == b'off':
			uart.write('Led Off!')	
			led.off()
		else:
			uart.write('Recibi: ')	
			uart.write(ch)
