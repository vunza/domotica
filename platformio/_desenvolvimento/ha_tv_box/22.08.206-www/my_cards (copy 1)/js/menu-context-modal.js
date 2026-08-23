// menu-handler.js

import { getToken, pubTopicsWIthWebSocket, ip_e_porta } from './globals.js';

/**
 * Cria um manipulador para as ações do menu contextual.
 * 
 * @param {Object} cardInstance - Instância do card (this).
 * @returns {Function} Função que recebe a ação e executa a lógica correspondente.
 */
export function createMenuHandler(cardInstance) {
  return async function handleMenuAction(action) {
    // Verifica se o card está pronto
    if (!cardInstance._hass || !cardInstance._stateObj) return;
    
    switch (action) {
      case 'upload_sketch': 
        const node_id = cardInstance._stateObj.attributes.mac_address.replace(/:/g, "").toLowerCase(); 
        const saved_device_mac = cardInstance._stateObj.attributes.mac_address;
        const basic_topic = cardInstance._stateObj.attributes.basic_topic;
        const topic = `${basic_topic}${node_id}/set`;
        
        // Obter Token de acesso ao HA
        const api_token = '/local/json_files/token_api.json';             
        const token = await getToken(api_token);  
        
        // Mensagem a ser enviada, usando o formato esperado pelo ESP32, framing (UART robusto).
        const framing_msg = `<OTA_MODE|${saved_device_mac}>`;

        // Publica mensagem        
        pubTopicsWIthWebSocket(token, topic, framing_msg ); 
        
        break;
      case 'rename':
        if (cardInstance._openRenameModal) {          
          cardInstance._openRenameModal();
        } else {
          console.error('_openRenameModal não definido');
        }
        break;
      case 'timer':
        if (cardInstance._openTimerModal) {
          cardInstance._openTimerModal();
        } else {
          console.error('_openTimerModal não definido');
        }
        break;
      case 'delete':
        alert('Remover');
        break;
      case 'close':
        if (cardInstance._contextMenu) {
          cardInstance._contextMenu.close();
        }
        break;
      default:
        break;
    }
  };
}







