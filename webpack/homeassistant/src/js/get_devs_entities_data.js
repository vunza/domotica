
/**
 * Obtém dispositivos Zigbee do Home Assistant.
 * @param {String} token variável para autenticação ao HA
 * @returns {Promise<Array>} Uma promessa que resolve para um array de dispositivos Zigbee.
 */
async function getZigbeeDevices(token) {
    const response = await fetch('/api/states', {
        headers: {
            'Authorization': 'Bearer ' + token
        }
    });
    
    if (!response.ok) {
        throw new Error('Falha ao obter entidades');
    }
    
    const entities = await response.json();

     // Sem filro de 0x
    /*const zigbeeEntities = entities.filter(entity => 
        entity.entity_id
    );*/
    
    // Filtra apenas dispositivos Zigbee (normalmente começam com 0x)
    const zigbeeEntities = entities.filter(entity => 
        entity.entity_id.includes('0x') && 
        (entity.entity_id.startsWith('switch.') || 
         entity.entity_id.startsWith('light.') ||
         entity.entity_id.startsWith('sensor.'))
    );
   
    
    // Agrupa por dispositivo (baseado no ID do dispositivo)
    const devicesMap = new Map();
    
    zigbeeEntities.forEach(entity => {
        const [domain, deviceId] = entity.entity_id.split('.');
        
        if (!devicesMap.has(deviceId)) {
            devicesMap.set(deviceId, {
                device_id: deviceId,
                entities: [],
                friendly_name: entity.attributes.friendly_name || 
                              deviceId.replace(/_/g, ' ').replace(/\b\w/g, l => l.toUpperCase())
            });
        }
        
        devicesMap.get(deviceId).entities.push({
            entity_id: entity.entity_id,
            domain: domain,
            state: entity.state,
            attributes: entity.attributes
        });
    });
    
    return Array.from(devicesMap.values()).map(device => {
        const mainEntity = device.entities.find(e => 
            e.domain === 'switch' || e.domain === 'light'
        ) || device.entities[0];
        
        return {
            id: device.device_id,
            nome: device.friendly_name,
            tipo: getZigbeeDeviceType(device),
            status: getZigbeeDeviceStatus(device.entities),
            historico: new Date().toLocaleString('pt-PT'),
            entities: device.entities,
            main_entity: mainEntity
        };
    });
}


/** 
 * Determina o tipo de dispositivo Zigbee com base nas entidades associadas.
 * @param {Object} device Objeto do dispositivo contendo suas entidades.
 * @returns {String} Tipo do dispositivo (e.g., 'lampada', 'switch', 'sensor').
 */
function getZigbeeDeviceType(device) {
    const domains = device.entities.map(e => e.domain);
    
    if (domains.includes('light')) return 'lampada';
    if (domains.includes('switch')) return 'switch';
    if (domains.includes('sensor')) return 'sensor';
    
    return 'dispositivo';
}


/** 
 * Determina o status do dispositivo Zigbee com base nas entidades associadas.
 * @param {Array} entities Array de entidades associadas ao dispositivo.
 * @returns {String} Status do dispositivo ('online' ou 'offline').
 */
function getZigbeeDeviceStatus(entities) {
    const availableEntities = entities.filter(entity => 
        entity.state !== 'unavailable' && entity.state !== 'unknown'
    );
    
    return availableEntities.length > 0 ? 'online' : 'offline';
}


/**
 * Função para obter dados (Id, Dominio e Nome) das entidades do Home Assistant.
 * @param {String} token Para autenticação na API do Home Assistant.
 * @returns {Promise<Array>} Uma promessa que resolve para um array de objetos contendo os dados das entidades.
 */
async function getEntitiesData(token) {
    const response = await fetch('/api/states', {
        headers: {
            'Authorization': 'Bearer ' + token
        }
    });
    
    if (!response.ok) {
        throw new Error('Falha ao obter entidades');
    }
    
    const entities = await response.json();
    
    return entities.map(entity => {
        const [domain, deviceId] = entity.entity_id.split('.');
        
        return {
            // Para usar no criar_card()
            id: entity.entity_id,
            nome: entity.attributes.friendly_name || 
                  deviceId.replace(/_/g, ' ').replace(/\b\w/g, l => l.toUpperCase()),
            tipo: domain,
            status: entity.state === 'unavailable' ? 'offline' : 'online',
            historico: new Date(entity.last_updated).toLocaleString('pt-PT'),
            
            // Dados completos se precisar
            entity_id: entity.entity_id,
            domain: domain,
            device_id: deviceId,
            state: entity.state,
            attributes: entity.attributes
        };
    });
}


export { getZigbeeDevices, getEntitiesData};
