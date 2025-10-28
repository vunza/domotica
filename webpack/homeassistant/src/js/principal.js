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
import { tkn, tipos_dispositivos as devType } from './vars_globais.js';


///////////////////////////////////////////
// Espera o carregamento completo do DOM //
///////////////////////////////////////////
document.addEventListener('DOMContentLoaded', function() {

    getZigbeeDevices(tkn).then(data => {
        data.forEach(device => {

            criar_card(device.id, {
                nome: device.nome,
                historico: device.historico,
                tipo: device.tipo,
                status: device.status
            });

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

            console.log(device.id, device.nome, device.tipo, device.status, 
                device.historico/*, device.main_entity, device.entities*/);          
        });
    }).catch(error => {
        console.error('Erro ao obter dados dos dispositivos:', error);
        return [];
    });


    /*for (let i = 1; i <= 3; i++) {
        criar_card(`lamp${i}`, {
            nome: `Lâmpada ${i}`,
            historico: '14:30:25, 15/12/2024',
            tipo: 'lampada',
            status: 'online'
        });
    }*/   

   
    // Inicializa a navegação inferior
    const bottomNav = new BottomNavigation();
    bottomNav.handleBackButton();
    const submenuOverlay = new SubmenuOverlay();
    
    // Verifica hash da URL inicial
    const hash = window.location.hash.substring(1);
    if (hash && document.getElementById(hash)) {
        bottomNav.navigateTo(hash);
    }
});


// Melhoria de performance do touch events no Menu Inferior
document.addEventListener('touchstart', function() {}, { passive: true });


