
/**
 * @author Wnr
 * @date 2024-12-15
 * @description Variáveis globais usadas no sistema.
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
const tipos_dispositivos = ["LAMPADA", "TOMADA", "AR_CONDICIONADO", "VENTILADOR", "CONTROLO_REMOTO"];


export { api, http_port, ip_e_porta, tipos_dispositivos };