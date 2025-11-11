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
import {getZigbeeDevices} from './get_devs_entities_data.js';
import {BottomNavigation} from './menu_inferior.js';
import {SubmenuOverlay} from './submenu_overlay.js';
import {api, tipos_dispositivos as devType } from './vars_globais.js';



document.addEventListener("DOMContentLoaded", function () {

    // Busca dispositivos Zigbee e cria cards dinamicamente.
    getZigbeeDevices(api).then((data) => {

        data.forEach((device) => {              
            
            if( (device.tipo == 'lampada' || device.tipo == 'switch') && !device.id.includes('child_lock')) {
                
                const entityId = device.main_entity.entity_id; // Ex.: light.tz3000_5ftkaulg_ts0011
                criar_card(entityId, {
                    nome: device.nome,
                    historico: device.historico,
                    tipo: device.tipo,
                    status: device.status,
                });                  
            }

            // Ignora dispositivos de sensor
            /*if(device.id.includes('_lux')|| device.id.includes('_motion')|| 
                    device.id.includes('_battery')|| device.id.includes('_temperature')||
                    device.id.includes('_learn')|| device.id.includes('_indicator')) {
                    return;
                }
                else if(device.tipo == 'lampada' || device.tipo == 'switch') {
                    criar_card(device.id, {
                        nome: device.nome,
                        historico: device.historico,
                        tipo: device.tipo,
                        status: device.status
                    });
                }*/
        });
        })
        .catch((error) => {
        console.error("Erro ao obter dados dos dispositivos:", error);
        return [];
        });

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
        const response = await fetch('/local/json_files/token_api.json');
        const { token } = await response.json();

        // Agora use o token para fazer uma requisição à API
        const result = await fetch(`/api/services/${dominio}/toggle`, {
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${token}`,
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ entity_id: entityId })
        });

        //console.log('Resultado:', result);

    } catch (error) {
        console.error('Erro:', error);
    }
}