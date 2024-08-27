import network
import time

class Rede:
    # Modo = [STA/AP]
    def __init__(self, sssid, pswd):
        self.ssid = sssid 
        self.pwd = pswd
     
    
    #################################
    # Função para conectar ao Wi-Fi #
    #################################
    def conectar_wifi(self, _txpower):       
        sta = network.WLAN(network.STA_IF)
        sta.active(False)
        sta.active(True)
        sta.disconnect()  # Disconnect from last connected WiFi SSID
        sta.config(txpower=_txpower)
         
        if sta.isconnected():
            sta.disconnect()
        
        if not sta.isconnected():
            sta.connect(self.ssid, self.pwd)
            timeout = 10  # Tempo máximo de espera em segundos
            start = time.time()
            while not sta.isconnected() and time.time() - start < timeout:
                print("Conectando-se a Rede: %s" % self.ssid)
                time.sleep(1)

        if sta.isconnected():
            print("Conectado, IP: %s" % sta.ifconfig()[0])
        else:
            print('Falha de conexao!')
        
        
    ############    
    # Criar AP #
    ############
    def criar_ap(self, _hidden, _canal, _txpower):
        ap = network.WLAN(network.AP_IF)
        ap.active(True)
        ap.config(txpower=_txpower, channel=_canal)
        ap.config(essid = self.ssid, password = self.pwd, channel = _canal, authmode=network.AUTH_WPA_WPA2_PSK)
        ap.config(hidden = _hidden)  # Oculta o SSID

        while ap.active() == False:
            print("Criando a Rede: %s" % self.ssid)
            pass
            
        print('SSID: %s\nIP %s' % (self.ssid, ap.ifconfig()[0]))
            
            



