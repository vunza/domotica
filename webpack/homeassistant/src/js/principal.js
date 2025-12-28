/**
 * @file principal.js
 * @description Arquivo principal que inicializa a aplicação, cria cards dinâmicos.
 * @author Zambo
 * @date 2024-12-15
*/

import '../css/styles.scss';
//import img from '../assets/orquidea.jpeg';
import {createCardElement} from './cria_cards.js';
//import {BottomNavigation} from './menu_inferior.js';
//import {SubmenuOverlay} from './submenu_overlay.js';
import {api, getDevicesWIthWebSocket, getToken, getEntitiesDataWithApi, trocarCorSVG } from './vars_funcs_globais.js';
import {  } from './rename_devices.js';
import {  } from './devices_timers.js';
import {  } from './menu_contextual.js';


// Lista das entidades
let entities_list = [];


document.addEventListener("DOMContentLoaded", async function () {

    // Cria Cards em função dos dispositivos do HA    
    try {
        // Obter Token de acesso ao HA
        const api_token = '/local/json_files/token_api.json';
        const token = await getToken(api_token);

        // Guarda lista de objectos com os dados das Entidades, para ser comparada com a lista de dispositivos        
        getEntitiesDataWithApi(token, api).then( (entidades)=>{   
            //console.log(entidades);
            entidades.forEach((entity) => {  
                // Se tiver o atributo mac_address, guarda na variavel                
                let macAddr = null;
                if (entity.attributes.mac_address){
                    macAddr = entity.attributes.mac_address;
                }     

                entities_list.push({
                    id: entity.id, // dominio.ID (Ex.: sensor.0xa4c138237471c56a_current)
                    friendly_name: entity.nome,
                    historico: entity.historico,
                    status: entity.status, // online|offline
                    state: entity.state, // on|off
                    device_id: entity.device_id, // Apenas ID (Ex.: 0xa4c138237471c56a_current)
                    mac_address: macAddr // MAC Address 
                });
            });              
                        
        });
       
       
        // Obter Dispositivos
        getDevicesWIthWebSocket(token).then( (devices) => {             
                   
            // Criar Crads dos dispositivos
            devices.forEach(element => {
               /* if(element.id.includes("wemos"))*/
                // (*J*) Ponto para ver o tipo de Dispositivo
                //console.log(element.name + " - " + element.model);
                
                // Filtra elementos por id, dominio, 
                const ieee = element.name.toLowerCase(); // Adequar para IEEE do Dispositivo      
                const dominios = ["switch", "light", "sensor", "text"]; // Para entidaes com os dominios indicados
                const posicoes = ["left", "center", "right"];  // Para Dispositivos com mais de um canal
                const hubsir = ["learn_ir_code", "battery", "ir_code_to_send", "learned_ir_code"];  // Para Hubs ir                            
                const espnow = ["serial_espnow"];
                const wemos = ["wemos", "mini", "d1", "led"];
               
                const resultado = entities_list.filter(item => {  
                                      
                    // Fil;tra ID das entidades vinculadas a Devices e as criadas por via de template                                    
                    const temIEEE = item.device_id.includes(ieee) || item.id.includes('serial_espnow');  
                   
                    // Filtra os dominios indicados
                    const temDominio = dominios.some(d => item.id.startsWith(d + "."));
                    
                    // Filtra dispositivos com mais de uma entidade, hubs ir e entidades criadas por template
                    const temPosicao = posicoes.some(p => item.id.endsWith("_" + p)) || 
                    hubsir.some(l => item.id.endsWith("_" + l)) ||  
                    espnow.some(e => item.id.includes(e)) ||     
                    wemos.some(w => item.id.includes(w)) ||                                               
                    // permite sem left/center/right
                    !item.id.includes("_");  
                    
                    return temIEEE && temDominio && temPosicao;                    
                });               
              

                // Confere os Tipos de Dispositivo    
                const texto = element.model;          
                let tipo = null;
                if (texto != null && texto.includes('switch')) {
                    tipo = 'light';
                } else if (texto != null && texto.includes('Wemos D1 Mini')) {
                    tipo = 'light';
                } else if (texto != null && texto.includes('Plug')) {
                    tipo = 'plug';
                } else if (texto != null && texto.includes('smart IR remote')) {
                    tipo = 'hub_zb';
                } else {
                    tipo = 'picture';
                }

                    
                // Criar Cards dos Dispositivos
                resultado.forEach( (item) => {                       

                    const card = {
                        id: item.id,
                        title: item.friendly_name,
                        content: item.historico,
                        type: tipo,
                        mac_address: item.mac_address
                    };
                    
                    // Cria Card
                    createCardElement(card);

                    // Troca a cor do SVG, em funcao do estado do item criado
                    trocarCorSVG(item.id, item.state, tipo);

                });              
               
            });         
        });
        
    } catch (error) {
        console.log(`Erro ao criar Cards: ${error}`);
    }

    
    // TODO: Tratar o Menu contextual
    // Inicializa a navegação inferior
    /*const bottomNav = new BottomNavigation();
    const submenuOverlay = new SubmenuOverlay();*/

    // Verifica hash da URL inicial
    const hash = window.location.hash.substring(1);
    if (hash && document.getElementById(hash)) {
        bottomNav.navigateTo(hash);
    }
});





/**
 * Manipula o clique em um card contendo um SVG, executa o serviço `toggle`
 * no Home Assistant para a entidade associada e atualiza visualmente a cor
 * do ícone conforme o novo estado do dispositivo.
 *
 * O ID do elemento recebido segue o padrão: `dominio.entidade.img`
 * (ex.: `"light.tz3000_5ftkaulg_ts0011.img"`).  
 * A função remove o sufixo `.img`, identifica o domínio e dispara
 * a ação correspondente via API do Home Assistant.
 *
 * Em seguida, após o toggle, o estado real da entidade é obtido usando WebSocket na funcao
 * `getDevicesWIthWebSocket()`, garantindo que o estado refletido no frontend
 * esteja sincronizado com o backend.  
 * Finalmente, `trocarCorSVG()` aplica a cor correta ao SVG de acordo com
 * o novo estado.
 *
 * @async
 * @function click_on_image_card
 *
 * @param {string} id - ID completo do card clicado, incluindo o sufixo `.img`.
 *                      Exemplo: `"light.lampada_cozinha.img"`.
 *
 * @returns {Promise<void>} Não retorna valores diretamente, mas atualiza a UI.
 *
 * @example
 * <!-- HTML -->
 * <div id="light.lampada_sala.img" onclick="click_on_image_card(this.id)">
 *     <svg>...</svg>
 * </div>
 *
 * @example
 * // JS - chamada manual
 * click_on_image_card("switch.tomada_tv.img");
 *
 * @description
 * Fluxo da função:
 *  1. Remove `.img` para obter o entity_id real.
 *  2. Extrai o domínio (ex.: "light", "switch").
 *  3. Carrega o token da API a partir do JSON local.
 *  4. Envia um comando `toggle` para o Home Assistant.
 *  5. Se o toggle for aceito:
 *       - Obtém a lista de entidades via API.
 *       - Localiza a entidade correspondente.
 *       - Obtém o novo estado real (com polling inteligente).
 *       - Atualiza a cor do SVG no card.
 */
window.click_on_image_card = async function(id){

    // Retira ".img" do Id (ex.: id = "light.tz3000_5ftkaulg_ts0011.img")
    const entityId = id.replace(".img", "");

    // Obtém o domínio a partir do ID (ex.: "light")
    const dominio = id.split(".")[0];

    try {
        // Buscar o token do arquivo JSON local
        const api_token = '/local/json_files/token_api.json';
        const token = await getToken(api_token);

        // Envia o comando toggle para a API
        const result = await fetch(`/api/services/${dominio}/toggle`, {
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${token}`,
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ entity_id: entityId })
        });        

    } catch (error) {
        console.error('Erro:', error);
    }
};


