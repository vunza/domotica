#def run(service_data):
#    with open("/config/automations.yaml", "a") as f:
#        f.write("\n- alias: Teste\n  trigger:\n    - platform: time\n      at: '12:00:00'\n  action:\n    - service: persistent_notification.create\n      data:\n        message: Funcionou!")
#   hass.services.call("homeassistant", "reload_config_entry", {})


# Obtém os parâmetros passados
entity_id = data.get('entity_id')
action = data.get('action')  # 'turn_on' ou 'turn_off'
domain = entity_id.split('.')[0]  # Extrai o domínio (light, switch, etc)

try:
    # Chama o serviço diretamente usando a API interna
    service_data = {
        'entity_id': entity_id
    }
    
    # Executa o serviço
    hass.services.call(domain, action, service_data, blocking=True)
        
    # Opcional: Verifica o estado após a mudança
    state = hass.states.get(entity_id)    

except Exception as error:
    logger.error(f"Falha ao controlar {entity_id}: {error}")
    raise