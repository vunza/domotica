#!/bin/bash

while  true  # Keep an infinite loop to reconnect when connection lost/broker unavailable
do
   mosquitto_sub -h "192.168.0.3" -t "dados/ESP12E_UPS/SENSORES" | while read -r payload
   do        
        echo ${payload} | jq . > tmp.json        
        tensao=$(jq -r .Tensao tmp.json)
		temperatura=$(jq -r .Temperatura tmp.json)
        ref=11.8  
        max=12.5
        min=11.5
        volt=0;
                        
        # Normaliazação para a percentagem
        if awk "BEGIN {exit !($tensao < $min)}"; then
			  volt=$min
		elif awk "BEGIN {exit !($tensao > $min)}"; then
			if awk "BEGIN {exit !($tensao < $max)}"; then
				volt=$tensao
			fi		
		else
			volt=$max
		fi	
		
        # norm = (max' - min') / (max-min) * (value - min) + min';
        #norm=$(echo "100 - 0"/"$max - $min"\*"$volt - $min" | bc -l)
       	
       	var1=$(echo "100-0" | bc -l)
       	var2=$(echo "$max-$min" | bc -l)
       	var3=$(echo "$volt-$min" | bc -l)                
        norm=$(echo "$var1/$var2*$var3" | bc -l)        
        norm=$(printf %.f $(echo "$norm" | bc -l))
           	
		if awk "BEGIN {exit !($tensao <= $ref)}"; then
			#notify-send  --icon=notification-message-IM -t 5000 "Alerta, Baterias Descarregadas!" "<span color='#57dafd' font='26px'><i><b>Tensão: "$(jq -r .Tensao tmp.json)"V</b></i>\nCarga: "$norm"%</span>"   
			notify-send --icon=notification-message-IM "Alerta, Baterias Descarregadas!" "<span color='#57dafd' font='26px'><i><b>Carga: "$norm"%</b></i><span color='orange'>\nTensão: "$(jq -r .Tensao tmp.json)"V</span></span>"      
		else
			#echo "Baterias Carregadas!"
			notify-send --hint int:transient:1 "$(jq -r .Tensao tmp.json)V" "<span color='#57dafd'>$temperatura°C</span>"
			#notify-send --icon=notification-message-IM -t 2500 "Estado das Baterias!" "<span color='#57dafd' font='26px'><i><b>Carga: "$norm"%</b></i><span color='orange'>\nTensão: "$(jq -r .Tensao tmp.json)"V</span></span>"      
		fi	
    done
    sleep 10  # Wait 10 seconds until reconnection
done # &  # Discomment the & to run in background 

