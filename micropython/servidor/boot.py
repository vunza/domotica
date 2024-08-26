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

# Conectar-se a Rede WiFi
sta = wf.Rede(rede, senha)
sta.conectar_wifi(potencia_tx)

# Criar AP
'''
ap = wf.Rede('ESP32', '123456789')
hidden = False
canal = 1
ap.criar_ap(hidden, canal, potencia_tx)
'''




