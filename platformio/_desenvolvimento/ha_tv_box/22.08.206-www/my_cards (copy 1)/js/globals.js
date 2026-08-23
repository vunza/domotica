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
 * Publica uma mensagem MQTT através da WebSocket API do Home Assistant
 * 
 * Esta função estabelece uma conexão WebSocket com o Home Assistant, autentica-se
 * usando um token de acesso e publica uma mensagem no tópico MQTT especificado
 * utilizando o serviço interno `mqtt.publish`.
 * 
 * @async
 * @function pubTopicsWIthWebSocket
 * @param {string} token - Token de autenticação do Home Assistant (Long-lived access token)
 * @param {string} topico - Tópico MQTT de destino para publicação
 * @param {string|Object} mensagem - Conteúdo a ser publicado (objetos são convertidos para JSON)
 * @returns {Promise<Object>} Retorna uma Promise que resolve quando a mensagem é publicada
 * @throws {Error} Lança erro se a autenticação falhar, serviço indisponível ou timeout
 * 
 * @example
 * // Publicar uma string simples
 * try {
 *   const resultado = await pubTopicsWIthWebSocket(
 *     'eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9...',
 *     'casa/sensor/temperatura',
 *     '22.5'
 *   );
 *   console.log('Publicado com sucesso:', resultado);
 * } catch (erro) {
 *   console.error('Falha na publicação:', erro.message);
 * }
 * 
 * @example
 * // Publicar um objeto JSON
 * await pubTopicsWIthWebSocket(
 *   'seu_token_aqui',
 *   'homeassistant/switch/meu_switch/config',
 *   {
 *     name: "Meu Interruptor",
 *     state_topic: "casa/interruptor/estado",
 *     command_topic: "casa/interruptor/comando"
 *   }
 * );
 * 
 * @see {@link https://developers.home-assistant.io/docs/api/websocket|Home Assistant WebSocket API}
 * @see {@link https://www.home-assistant.io/integrations/mqtt/|Home Assistant MQTT Integration}
 * 
 * @remarks
 * Esta função requer:
 * 1. Home Assistant com WebSocket API habilitada
 * 2. Token de acesso longo (long-lived access token)
 * 3. Integração MQTT configurada no Home Assistant
 * 4. Permissões adequadas para o token
 * 
 * @warning
 * - Nunca exponha tokens em código client-side em produção
 * - Use HTTPS/WSS em ambientes de produção
 * - Cada chamada cria uma nova conexão WebSocket
 * 
 * @todo
 * - Adicionar suporte a opções de QoS e retain
 * - Implementar pool de conexões WebSocket
 * - Adicionar timeout configurável
 */
async function pubTopicsWIthWebSocket(token, topico, mensagem) {
  return new Promise((resolve, reject) => {
    let HA_URL = null;
    let url = new URL(window.location.href);

    if (url.protocol === 'http:') {
      HA_URL = `ws://${url.host}/api/websocket`;
    } else if (url.protocol === 'https:') {
      HA_URL = `wss://${url.host}/api/websocket`;
    }

    const ws = new WebSocket(HA_URL);
    let authenticated = false;
    let messageSent = false;

    // Timeout para evitar conexões pendentes
    const timeout = setTimeout(() => {
      ws.close();
      reject(new Error("Timeout na conexão WebSocket"));
    }, 10000);

    ws.onopen = () => {
      console.log("Conectado ao WebSocket API");
    };

    ws.onerror = (error) => {
      clearTimeout(timeout);
      reject(new Error(`Erro WebSocket: ${error.message}`));
    };

    ws.onmessage = (event) => {
      try {
        const msg = JSON.parse(event.data);

        if (msg.type === "auth_required" && !authenticated) {
          ws.send(JSON.stringify({
            type: "auth",
            access_token: token,
          }));
        }
        else if (msg.type === "auth_ok") {
          authenticated = true;
          
          ws.send(JSON.stringify({
            id: Date.now(),
            type: "call_service",
            domain: "mqtt",
            service: "publish",
            service_data: {
              topic: topico,
              payload: typeof mensagem === 'object' ? JSON.stringify(mensagem) : mensagem,
              qos: 1,
              retain: false
            }
          }));
        }
        else if (msg.type === "result" && msg.success && !messageSent) {
          messageSent = true;
          clearTimeout(timeout);
          ws.close();
          resolve({
            success: true,
            topic: topico,
            message: mensagem
          });
        }
        else if (msg.type === "result" && !msg.success) {
          clearTimeout(timeout);
          ws.close();
          reject(new Error(`Falha ao publicar: ${msg.error?.message || "Erro desconhecido"}`));
        }
      } catch (error) {
        clearTimeout(timeout);
        ws.close();
        reject(new Error(`Erro ao processar mensagem: ${error.message}`));
      }
    };
  });
}


export { getToken, pubTopicsWIthWebSocket, ip_e_porta };