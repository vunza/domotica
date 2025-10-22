/**
 * @file principal.js
 * @description Arquivo principal que inicializa a aplicação, cria cards dinâmicos,
 *              gerencia a navegação inferior e o submenu overlay.
 * @author Zambo
 * @date 2024-12-15
*/

import '../css/styles.scss';
//import img from '../assets/orquidea.jpeg';
import {criar_card, getDevicesData} from './cria_cards.js';
import {BottomNavigation} from './menu_inferior.js';
import {SubmenuOverlay} from './submenu_overlay.js';
import { tkn } from './vars_globais.js';




///////////////////////////////////////////
// Espera o carregamento completo do DOM //
///////////////////////////////////////////
document.addEventListener('DOMContentLoaded', function() {

    getDevicesData(tkn).then(data => {
        data.forEach(device => {

            if(device.tipo === 'light' || device.tipo === 'switch'){
                 console.log(device.id, device.nome, device.tipo);
            }           

            /*criar_card(device.id, {
                nome: device.nome,
                tipo: device.tipo,
                status: device.status,
                historico: device.historico
            });*/
        });
    }).catch(error => {
        console.error('Erro ao obter dados dos dispositivos:', error);
        return [];
    });


    for (let i = 1; i <= 3; i++) {
        criar_card(`lamp${i}`, {
            nome: `Lâmpada ${i}`,
            historico: '14:30:25, 15/12/2024',
            tipo: 'lampada',
            status: 'online'
        });
    }   

    for (let i = 4; i <= 6; i++) {
        criar_card(`lamp${i}`, {
            nome: `Lâmpada ${i}`,
            historico: '14:30:25, 15/12/2024',
            tipo: 'bomba',
            status: 'warning'
        });
    }

    for (let i = 7; i <= 9; i++) {
        criar_card(`lamp${i}`, {
            nome: `Lâmpada ${i}`,
            historico: '14:30:25, 15/12/2024',
            tipo: 'hub_zb',
            status: 'error'
        });
    }

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


