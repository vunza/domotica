# Envia comandos atraves da porta serial/uart

import json
class Comando
    var dev_id
    def init(arg) # Constructo, deve chamar-se "init"
        self.dev_id = arg
    end
    def SendZbStatus3()
        tasmota.cmd("ZbStatus3 " + self.dev_id)        
    end
    
    def SendZbPing()
        tasmota.cmd("ZbPing " + self.dev_id)        
    end
end


def get_ZbDevices() 
	# Obtem Dispositivos emparelhados
	var data = tasmota.cmd("ZbStatus")	
	var indx = data['ZbStatus1'].size()
	var tmr = 0
	
	tasmota.cmd("ZbStatus1")
	
	# Envia comandos ao dispositivos ca X tempo
	for i : 0..(indx - 1)
		var dev_id = data['ZbStatus1'][i]['Device']
		var tmr_id = data['ZbStatus1'][i]['Name']	
		var cmnd = Comando(dev_id)		
		tmr = (i + 1)*1500	
		tasmota.set_timer(tmr,  /-> cmnd.SendZbStatus3(), tmr_id)			
		tasmota.set_timer(tmr + 300,  /-> cmnd.SendZbPing(), tmr_id)					
	end
end

tasmota.add_cron("*/30 * * * * *", get_ZbDevices, "get_zb_devs") # Inicializa cron


#tasmota.remove_cron("get_zb_devs") # Termina o cron

