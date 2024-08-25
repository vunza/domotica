import network

'''
import esp
esp.osdebug(None)

import gc
gc.collect()
'''
# MODO STA
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.scan()                             
sta_if.connect("TPLINK", "gregorio@2012") 
sta_if.isconnected()  


# MODO AP 
ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid='ESP12E', password='123456789')
ap.config(hidden=True)  # Oculta o SSID

while ap.active() == False:
  pass
