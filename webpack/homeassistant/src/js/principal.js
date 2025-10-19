import '../css/styles.scss';
//import img from '../assets/orquidea.jpeg';
import {criar_card} from './cria_cards.js';
import {BottomNavigation} from './menu_inferior.js';


///////////////////////////////////////////
// Espera o carregamento completo do DOM //
///////////////////////////////////////////
document.addEventListener('DOMContentLoaded', function() {

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
    
    // Verifica hash da URL inicial
    const hash = window.location.hash.substring(1);
    if (hash && document.getElementById(hash)) {
        bottomNav.navigateTo(hash);
    }
    
    // Log para debug (opcional)
    document.addEventListener('pageChanged', (e) => {
        console.log('Navegou para:', e.detail.pageId);
    });

});


// Melhoria de performance do touch events no Menu Inferior
document.addEventListener('touchstart', function() {}, { passive: true });


