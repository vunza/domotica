####################
# VARAVEIS GLOBAIS #
####################
porta_http = 8484

rede = 'TPLINK'
senha = 'gregorio@2012'
#rede = 'Welwitschia Mirabilis'
#senha = 'tigre?2018@'

###########################
# Conetar/Criar Rede WiFi #
###########################
import wifi as wf

potencia_tx = 10

newmac = b'\x12\xb2\x03\x04\x05\xb1'

# Conectar-se a Rede WiFi
sta = wf.Rede(rede, senha, newmac)
sta.conectar_wifi(potencia_tx)
canal_wifi = sta.wifi_ch

# Criar AP
'''
ap = wf.Rede('ESP32', '123456789', newmac)
hidden = False
canal = 1
ap.criar_ap(hidden, canal, potencia_tx)
canal_wifi = ap.wifi_ch
'''




