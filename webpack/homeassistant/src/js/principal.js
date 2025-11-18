/**
 * @file principal.js
 * @description Arquivo principal que inicializa a aplicação, cria cards dinâmicos,
 *              gerencia a navegação inferior e o submenu overlay.
 * @author Zambo
 * @date 2024-12-15
*/

import '../css/styles.scss';
//import img from '../assets/orquidea.jpeg';
import {criar_card} from './cria_cards.js';
import {BottomNavigation} from './menu_inferior.js';
import {SubmenuOverlay} from './submenu_overlay.js';
import {api, getDevicesWIthWebSocket, getToken, getEntitiesDataWithApi } from './vars_funcs_globais.js';



document.addEventListener("DOMContentLoaded", async function () {

    // Cria Cards em função dos dispositivos do HA    
    try {
        // Obter Token de acesso ao HA
        const api_token = '/local/json_files/token_api.json';
        const token = await getToken(api_token);

        // Guarda lista de objectos com os dados das Entidades, para ser comparada com a lista de ispositivos
        let entities_list = [];
        getEntitiesDataWithApi(token, api).then( (entidades)=>{   
            //console.log(entidades);
            entidades.forEach((entity) => {
                entities_list.push({
                    id: entity.id,
                    friendly_name: entity.nome,
                    historico: entity.historico,
                    status: entity.status, // online|offline
                    state: entity.state, // on|off
                    device_id: entity.device_id
                });
            });              
                        
        });

       
       
        // Obter Dispositivos
        getDevicesWIthWebSocket(token).then( (devices) => {   
                   
            // Criar Crads dos dispositivos
            devices.forEach(element => {
                  
                // Filtra elementos por id, dominio, 
                const ieee = element.name;
                const dominios = ["switch", "light", "sensor", "text"]; // Para entidaes com os dominios indicados
                const posicoes = ["left", "center", "right"];  // Para Dispositivos com mais de um canal
                const hubsir = ["learn_ir_code", "battery", "ir_code_to_send", "learned_ir_code"];  // Para Hubs ir

                const resultado = entities_list.filter(item => {
                    const temIEEE = item.device_id.includes(ieee);
                    const temDominio = dominios.some(d => item.id.startsWith(d + "."));
                    const temPosicao = posicoes.some(p => item.id.endsWith("_" + p)) || 
                                       //hubsir.some(l => item.id.endsWith("_" + l)) || 
                                    !item.id.includes("_"); // permite sem left/center/right

                    return temIEEE && temDominio && temPosicao;
                });  

              
                // Criar Cards dos Dispositivos
                resultado.forEach( (item) => {
                    //console.log(item.id);
                    criar_card(item.id, {
                        nome: item.friendly_name,
                        historico:  item.historico,               
                        tipo: 'picture', // Força icon de imagem, tipo deve ser definido manualmenete
                        status: item.status
                    });
                });              
               
            });         
        });
        
    } catch (error) {
        console.log(`Erro ao criar Cards: ${error}`);
    }
    

    // Inicializa a navegação inferior
    const bottomNav = new BottomNavigation();
    const submenuOverlay = new SubmenuOverlay();

    // Verifica hash da URL inicial
    const hash = window.location.hash.substring(1);
    if (hash && document.getElementById(hash)) {
        bottomNav.navigateTo(hash);
    }
});





/**
 * Função que processa click sobre a imagem da card em questao.
 * Alterna o estado do dispositivo
 * @param {String} id Id da imagem clicada
 */
window.click_on_image_card = async function(id){
    
    // Retira "".img" do Id (id = light.tz3000_5ftkaulg_ts0011.img)
    const entityId = id.replace(".img", "");

     // obtem o dominio "light" do Id (id = light.tz3000_5ftkaulg_ts0011.img)
    const dominio = id.split(".")[0];

    try {
        // Buscar o token do arquivo JSON local
        const api_token = '/local/json_files/token_api.json';
        const token = await getToken(api_token);

        // Agora use o token para fazer uma requisição à API
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
}

