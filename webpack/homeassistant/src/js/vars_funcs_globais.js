
/**
 * @author Wnr
 * @date 2024-12-15
 * @description Variáveis e Funções globais usadas no sistema.
 */

/**
 * Token de acesso para autenticação na API do Home Assistant.
 * @type {String}
*/


/**
 * URL da API do Home Assistant para obter estados das entidades e/ou integrações.
 * @type {String}
 */
//const api = '/api/devices';
//const api = 'zigbee2mqtt/bridge/devices';
const api = '/api/states'; 


/**
 * Protocolo HTTP usado (http: ou https:)
 * @type {String} http[s]:
 */
const http_port = window.location.protocol; 

/**
 * IP e porta do servidor Home Assistant.
 * @type {String|Number}  http[s]://...
 */
const ip_e_porta = `${http_port}//${location.host}`; 

/**
 * Tipos de Dispositivos
 * @type {Array<String>}
 */
//const tipos_dispositivos = ["hub_zb", "lampada", "bomba", "switch", "interruptor"];


/**
 * Guarda o Tokem
 * @type {String|Number}  
 */
let cachedToken = null;  



/**
 * Funcao para obter o TOKEN guardado no arquivo json, que recebe o mesmo de uma variavel de ambiente
 * @param {String} api_token API/URL do arquivo que contem o TOKEN (Ex.: '/local/json_files/token_api.json')
 * @returns {Promise<String>} Retorna Promise<String>
 */
async function getToken(api_token) {
    // Se o token já foi carregado, retorna imediatamente
    if (cachedToken) {
        return cachedToken;
    }

    try {
        const response = await fetch(api_token, {
            cache: "no-store"  // evita usar versão antiga do arquivo
        });

        const data = await response.json();
        cachedToken = data.token;   // salva no cache

        return cachedToken;

    } catch (error) {
        console.error("Erro ao obter token:", error);
        return null;
    }
}


/**
 * Função para activar o pareamento com dispositivos zigbee
 * @param {String} HA_URL URL mais a porta do Homeassistant (http://ip:porta)
 * @param {String} api_mqtt_publish api/url/endpoint que recebe mensagem mqtt para actiavar modo de pareamento (Ex.: '/api/services/mqtt/publish')
 * @param {String} TOKEN Token de acesso ao homeassistant 
 * @param {String} topico_permit_join Topico onde publicar mensagem mqtt para permitir pareamento ("zigbee2mqtt/bridge/request/permit_join")
 * @param {Number} tempo_permit_join Tempo, em segundos, para manter o modo pareamento activo
 * @returns {Promise} Retorna Promise apos X segundos
 */
async function enablePermitJoin(HA_URL, api_mqtt_publish, TOKEN, topico_permit_join, tempo_permit_join) {
  
  const response = await fetch(`${HA_URL}${api_mqtt_publish}`, {
    method: "POST",
    headers: {
      "Authorization": `Bearer ${TOKEN}`,
      "Content-Type": "application/json"
    },
    body: JSON.stringify({
      topic: topico_permit_join,
      payload: JSON.stringify({
        value: true,
        time: tempo_permit_join 
      })
    })
  });

  const logsContainerEl = document.getElementById('logs-container');

  if (!response.ok) {
    mostrarLogs(logsContainerEl, `Erro: ${await response.text()}`, 'error');
    throw new Error("Erro ao ativar permit join");
  }

  mostrarLogs(logsContainerEl, 'Processo inicializado.', 'success');
  document.getElementById('connect_btn').disabled = true;

  // 🔥 RETORNAR APÓS X SEGUNDOS
  return new Promise(resolve => {
    setTimeout(() => {
      resolve("Tempo concluído");
    }, tempo_permit_join * 1000);
  });
}



/**
 * Função para obter, através do HA, Dispositivos Mqtt pareados, usando WebSocket
 * @param {String} token Token de acesso ao HA
 * @returns {Promise<object>} Retina um objecto json com os dados dos dspositivos (id, nome, fabricante, marca)
 */
async function getDevicesWIthWebSocket(token) {
   return new Promise((resolve, reject) => {

    let HA_URL =  null; 
    let url = new URL(window.location.href);  

    if (http_port === 'http:') {        
        HA_URL = `ws://${url.host}/api/websocket`;
    }
    else if (http_port === 'https:'){        
        HA_URL = `wss://${url.host}/api/websocket`;
    }      

     const ws = new WebSocket(HA_URL);

     ws.onopen = () => {
       console.log("Conectado ao WebSocket API");
     };

     ws.onmessage = (event) => {
       const msg = JSON.parse(event.data);

       // 1º passo → autenticação
       if (msg.type === "auth_required") {
         ws.send(
           JSON.stringify({
             type: "auth",
             access_token: token,
           })
         );
       }

       // 2º passo → confirmar autenticação
       else if (msg.type === "auth_ok") {

          // pedir a lista de devices do device registry
          ws.send(
            JSON.stringify({
              id: 1,
              type: "config/device_registry/list",
            })
          );

          // Inscrever-se no evento de mudança de estado
          ws.send(JSON.stringify({
              id: 2,
              type: "subscribe_events",
              event_type: "state_changed"
          }));

       }
       // Quando qualquer estado mudar
       else if (msg.type === "event") {
          const e = msg.event.data;            
          // Actualiza estado do Dispositivo
          const card = document.getElementById(e.entity_id);  
          if(card){      
            trocarCorSVG(e.entity_id, e.new_state.state, card.tipo);
          } 
      }
       // 3º passo → resposta com devices
       else if (msg.id === 1) {
         resolve(msg.result);         
       } else if (msg.type === "auth_invalid") {
         reject("Token inválido!");
         ws.close();
       }
     };
   });
}





/**
 * Funcao para exibir mensagens em forma de log, numa div
 * @param {Object} logsContainerEl Variavel com o bjecto do elemento/DIV
 * @param {String} message Mensagem a ser exibida
 * @param {String} type Tipo de mensagem a exibir (info, success, warning e error)
 */
function mostrarLogs(logsContainerEl, message, type = 'info') {
    const time = new Date().toLocaleTimeString();
    const entry = document.createElement('div');
    entry.className = `log-entry log-${type}`;
    entry.innerHTML = `<span class="log-time">[${time}]</span> ${message}`;
    logsContainerEl.appendChild(entry);
    logsContainerEl.scrollTop = logsContainerEl.scrollHeight;
}


/**
 * Obtem os dados (Id, Dominio, Nome, etc.) de todas entidades do Home Assistant.
 * @param {String} token Para autenticação na API do Home Assistant.
 * @param {String} api Endpoint dos estados de todas Entidades do HA ('/api/states' -- em uso)
 * @returns {Promise<Object[]>} Promise que resolve com um array de objetos JSON contendo os dados das entidades.
 */
async function getEntitiesDataWithApi(token, api) {
    const response = await fetch(api, {
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



/**
 * Função para obter estao de uma entidade, requerido pela função "actualizaFriendlyName".
 * @param {String} token Token de acesso ao HA.
 * @param {String} ip_porta URL Base do HA no formato http[s]://ip-do-ha:porta.
 * @param {String} entityId ID da Entidade em questão (Ex.: 'switch.0xa4c138e342bdfb48').
 * @returns {Promise<String>} Promise que resolve com uma string do estado da entidade (on|off).  
 */
async function obterEstadoEntidade(token, ip_porta, entityId) {     
    const url = `${ip_porta}/api/states/${entityId}`;
    
    try {
        const response = await fetch(url, {
            method: "GET",
            headers: {
                "Authorization": "Bearer " + token,
                "Content-Type": "application/json"
            }
        });
        const data = await response.json();        
        return data.state; 
    } catch (error) {
        console.error("Erro ao obter estado:", error);
        throw error; // Propaga o erro
    }
}


/**
 * Função para alterar o friendly_name de uma entidae.
 * @param {String} token Token de acesso ao HA.
 * @param {String} ip_porta URL Base do HA no formato http[s]://ip-do-ha:porta.
 * @param {String} entityId ID a Entidade a alterar o friendly_name.
 * @param {String} friendlyName friendly_name a ser atribuido a entidade.
 * @param {String} entityState String que comtem o estado da entidade (on|off), obtido da função "obterEstadoEntidade".
 * @param {String} deviceType String que comtem o tipo de dispositivo (array "tipos_dispositivos"), para determina o icon para a Card.
* @returns {string} Srting de confirmação do envio do comando.
 */
async function actualizaFriendlyName(token, ip_porta, entityId, friendlyName, entityState, deviceType) {   
   
    const url = `${ip_porta}/api/services/shell_command/atualiza_friendly`;

    fetch(url, {
    method: "POST",
    headers: {
        "Authorization": `Bearer ${token}`,
        "Content-Type": "application/json"
    },
    body: JSON.stringify({
        entity_id: entityId,
        friendly_name: friendlyName,
        entity_state: entityState,
        device_type: deviceType // Ler via: data.attributes.device_type
    })
    })
    .then(response => {
    if (!response.ok) {
        throw new Error("Erro na requisição");
    }
    return response.json();
    })
    .then(data => {
      console.log("Comando enviado com sucesso!");
    })
    .catch(error => {
      console.error("Erro:", error);
    });

}



/** 
 * Determina o tipo de dispositivo Zigbee com base nas entidades associadas.
 * @param {Object} device Objeto do dispositivo contendo suas entidades.
 * @returns {String} Tipo do dispositivo (e.g., 'lampada', 'switch', 'sensor').
 */
/*function getZigbeeDeviceType(device) {
    const domains = device.entities.map(e => e.domain);
    
    if (domains.includes('light')) return 'lampada';
    if (domains.includes('switch')) return 'switch';
    if (domains.includes('sensor')) return 'sensor';
    
    return 'dispositivo';
}*/


/** 
 * Determina o status do dispositivo Zigbee com base nas entidades associadas.
 * @param {Array} entities Array de entidades associadas ao dispositivo.
 * @returns {String} Status do dispositivo ('online' ou 'offline').
 */
/*function getZigbeeDeviceStatus(entities) {
    const availableEntities = entities.filter(entity => 
        entity.state !== 'unavailable' && entity.state !== 'unknown'
    );
    
    return availableEntities.length > 0 ? 'online' : 'offline';
}*/


/**
 * Altera a cor do SVG associado a um dispositivo, com base no estado e tipo.
 *
 * Esta função procura a `<div>` cujo ID é composto por `dev_id + ".img"`,
 * obtém o elemento `<svg>` dentro dela e ajusta a cor (fill) conforme:
 *  - Lâmpada ligada  → amarelo (#e1e437)
 *  - Lâmpada desligada → cinza claro (lightgray)
 *
 * @param {string} dev_id - Identificador base do dispositivo (sem o sufixo `.img`).
 * @param {string} dev_state - Estado atual do dispositivo. Aceita "on" ou "off".
 * @param {string} dev_type - Tipo do dispositivo. Esta função só aplica cores quando `light`.
 *
 * @example
 * trocarCorSVG("switch.0xa4c138e342bdfb48", "on", "light");
 * // Altera o SVG da div com ID "switch.0xa4c138e342bdfb48.img" para amarelo.
 *
 * @example
 * trocarCorSVG("lampada_sala", "off", "light");
 * // Altera o SVG da div "lampada_sala.img" para cinza claro.
 */
function trocarCorSVG(dev_id, dev_state, dev_type){

  if(dev_state === 'on' && dev_type === 'light'){
      const div = document.getElementById(`${dev_id}.img`);
      const svg = div.querySelector('svg');
      svg.style.fill = '#e1e437';
  }
  else if(dev_state === 'off' && dev_type === 'light'){
      const div = document.getElementById(`${dev_id}.img`);
      const svg = div.querySelector('svg');
      svg.style.fill = 'lightgray';
  }   
}



export { 
  api, 
  http_port, 
  ip_e_porta, 
  getToken, 
  enablePermitJoin, 
  mostrarLogs,
  getDevicesWIthWebSocket,
  getEntitiesDataWithApi,
  trocarCorSVG
};