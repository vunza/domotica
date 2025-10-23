
/**
 * @author Wnr
 * @date 2024-12-15
 * @description Variáveis globais usadas no sistema.
 */

/**
 * Token de acesso para autenticação na API do Home Assistant.
 * @type {String}
*/
const tkn = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJjMDUwYTVkNTMzZDU0ZjBmODJlY2NjZmQyZDgwZDRlYyIsImlhdCI6MTczNjcxNzU1NSwiZXhwIjoyMDUyMDc3NTU1fQ.oLIiyFN8c4lPCAgIPL_sFEWI12P3IcsLsid4KL8A2D0';

/**
 * Protocolo HTTP usado (http: ou https:)
 * @type {String}
 */
const http_port = window.location.protocol; 

/**
 * IP e porta do servidor Home Assistant.
 * @type {String|Number}
 */
const ip_e_porta = `${http_port}//${location.host}`; 

export { tkn, http_port, ip_e_porta };