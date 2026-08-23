import { getToken, pubTopicsWIthWebSocket } from './globals.js';

const API_TOKEN_PATH = '/local/json_files/token_api.json';

/**
 * Cria um manipulador para as ações do menu contextual.
 * 
 * @param {Object} cardInstance - Instância do card (this).
 * @returns {Function} Função que recebe a ação e executa a lógica correspondente.
 */
export function createMenuHandler(cardInstance) {
  return async function handleMenuAction(action) {
    if (!cardInstance._hass || !cardInstance._stateObj) return;

    switch (action) {
      case 'upload_sketch': {
        const { mac_address: savedMac, basic_topic: basicTopic } = cardInstance._stateObj.attributes || {};

        if (!savedMac || !basicTopic) {
          console.warn('MenuHandler: Atributos "mac_address" ou "basic_topic" não encontrados na entidade.');
          break;
        }

        const nodeId = savedMac.replace(/:/g, '').toLowerCase();
        const topic = `${basicTopic}${nodeId}/set`;
        const framingMsg = `<OTA_MODE|${savedMac}>`;

        try {
          const token = await getToken(API_TOKEN_PATH);
          pubTopicsWIthWebSocket(token, topic, framingMsg);
        } catch (error) {
          console.error('MenuHandler: Erro ao enviar comando de OTA/Upload Sketch:', error);
        }
        break;
      }

      case 'rename':
        if (cardInstance._openRenameModal) {
          cardInstance._openRenameModal();
        } else {
          console.error('MenuHandler: _openRenameModal não definido');
        }
        break;

      case 'timer':
        if (cardInstance._openTimerModal) {
          cardInstance._openTimerModal();
        } else {
          console.error('MenuHandler: _openTimerModal não definido');
        }
        break;

      case 'delete':
        alert('Remover');
        break;

      case 'close':
        cardInstance._contextMenu?.close();
        break;

      default:
        break;
    }
  };
}