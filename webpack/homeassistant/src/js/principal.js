import '../css/styles.scss';
//import img from '../assets/orquidea.jpeg';
import { criar_card} from './cria_cards.js';

///////////////////////////////////////////
// Espera o carregamento completo do DOM //
///////////////////////////////////////////
document.addEventListener('DOMContentLoaded', function() {

    criar_card('lamp', {
        nome: 'Lâmpada Sala',
        historico: '14:30:25, 15/12/2024',
        tipo: 'lampada',
        status: 'online'
    });

    
    criar_card('lamp2', {
        nome: 'Electrobomba Jardim',
        historico: '14:30:25, 15/12/2024',
        tipo: 'bomba',
        status: 'offline'
    });

    criar_card('lamp3', {
        nome: 'Controlo Remoto Cozinha',
        historico: '14:30:25, 15/12/2024',
        tipo: 'hub_zb',
        status: 'warning'
    });

});


