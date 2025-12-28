
import { executaApiServices, getToken, ip_e_porta } from './vars_funcs_globais.js';

// Menu Contextual
const contextMenu = document.getElementById('context-menu');
let currentMenuCardId = null;


// Abrir menu contextual
document.addEventListener('click', function(e) {

    const menuTrigger = e.target.closest('.menu-trigger');
    
    if (menuTrigger) {
        e.preventDefault();
        e.stopPropagation();
        
        const cardId = parseInt(menuTrigger.dataset.id);
        // Verifica o ID do card (adequação para cards sem ID numérico)
        if(cardId === null || isNaN(cardId)){
           currentMenuCardId = menuTrigger.dataset.id;
        }
        else{
            currentMenuCardId = cardId;
        }
                        
        // Posicionar menu próximo ao botão clicado
        const rect = menuTrigger.getBoundingClientRect();
        contextMenu.style.top = `${rect.bottom + window.scrollY + 5}px`;
        contextMenu.style.left = `${rect.left + window.scrollX - 180}px`;

        // Oculta a opção Programar se o card não tem MAC Address
        const cardElement = document.getElementById(currentMenuCardId);
        let saved_device_mac = cardElement.querySelector('.card-header').getAttribute('data-device-mac-address');
        console.log('MAC Address do dispositivo:', saved_device_mac);
        if (!saved_device_mac || saved_device_mac === 'null') {
            document.querySelector('.menu-item[data-action="upload_sketch"]').style.display = 'none';
        }    
        
        
        // Ativar menu
        closeAllMenus();
        contextMenu.classList.add('active');
        return;
    }
    
    // Fechar menu se clicar fora
    if (!e.target.closest('.context-menu')) {
        closeAllMenus();
    }
});



// Ações do menu
contextMenu.addEventListener('click', function(e) {    
    const menuItem = e.target.closest('.menu-item');    
    if (!menuItem || !currentMenuCardId) return;
    
    const action = menuItem.dataset.action;
    const cardId = currentMenuCardId;
    
    switch(action) {
        case 'upload_sketch':
            uploadSketch(cardId);
            break;
        /*case 'duplicate':
            duplicateCard(cardId);
            break;*/
        case 'timer':
            timerCard(cardId);
            break;
        case 'rename':
            renameCard(cardId);
            break;
        case 'delete':
            deleteCard(cardId);
            break;    
        case 'close':            
            closeMenu();
            break;
    }
    
    closeAllMenus();
});




async function uploadSketch(cardId) {
    const cardElement = document.getElementById(cardId);
    if (cardElement) {
        // Salvar nome e ID do dispositivo globalmente
        let saved_device_name = cardElement.querySelector('.card-title').textContent.trim();        
        let saved_device_mac = cardElement.querySelector('.card-header').getAttribute('data-device-mac-address');
        

        // Obter Token de acesso ao HA
        const api_token = '/local/json_files/token_api.json';             
        const token = await getToken(api_token);  

        // Mensagem a ser enviada, usando o formato esperado pelo ESP32, framing (UART robusto).
        const framing_msg = `<OTA_MODE|${saved_device_mac}>`;       

        // Cria mensagem JSON a ser enviada, a um dispositivo EspNow, pela porta serial
        const json_mensagem = JSON.stringify({
            msg: framing_msg            
        });
       

        // Envia mensagem JSON a ser enviada, a um dispositivo EspNow, pela porta serial
        const api_service = "shell_command/esp_serial_write";
        executaApiServices(token, ip_e_porta, api_service, "application/json", json_mensagem);

        
        // Passar Nome e ID do dispositivo para a interface de renomear    
        /*document.querySelector('.config-timers-header h2').textContent = saved_device_name;          
        document.getElementById('timer_save_device_id').value = saved_device_id;
        // Ocultar container de cards
        document.getElementById('cards_main_wrapper').style.display = 'none';        
        // Mostrar interface de renomear
        document.getElementById('config-timers-container').style.display = 'block';*/
    }    
}

function timerCard(cardId) {
    const cardElement = document.getElementById(cardId);
    if (cardElement) {
        // Salvar nome e ID do dispositivo globalmente
        let saved_device_name = cardElement.querySelector('.card-title').textContent.trim();
        let saved_device_id = cardId; 
        // Passar Nome e ID do dispositivo para a interface de renomear    
        document.querySelector('.config-timers-header h2').textContent = saved_device_name;          
        document.getElementById('timer_save_device_id').value = saved_device_id;
        // Ocultar container de cards
        document.getElementById('cards_main_wrapper').style.display = 'none';        
        // Mostrar interface de renomear
        document.getElementById('config-timers-container').style.display = 'block';
    }    
}


function renameCard(cardId) {
    const cardElement = document.getElementById(cardId);
    if (cardElement) {
        // Salvar nome e ID do dispositivo globalmente
        let saved_device_name = cardElement.querySelector('.card-title').textContent.trim();
        let saved_device_id = cardId; 
        // Passar Nome e ID do dispositivo para a interface de renomear    
        document.getElementById('deviceName').value = saved_device_name;   
        document.getElementById('save_device_name').value = saved_device_name;
        document.getElementById('save_device_id').value = saved_device_id;
        // Ocultar container de cards
        document.getElementById('cards_main_wrapper').style.display = 'none';       
        // Mostrar interface de renomear
        document.getElementById('rename-container').style.display = 'block';
    }    
}


function deleteCard(cardId) {
    if (confirm('Tem certeza que deseja excluir este card?')) {
        const cardElement = document.getElementById(cardId);
        if (cardElement) {
            console.log(`Excluir card: ${cardId}`);
            showActionIndicator('Card excluído com sucesso!');
        }  
        
    }
}


function closeMenu() {    
    closeAllMenus();
}


// Fechar menu com ESC
document.addEventListener('keydown', function(e) {
    if (e.key === 'Escape') {
        closeAllMenus();
    }
});


// Fechar todos os menus
function closeAllMenus() {
    contextMenu.classList.remove('active');
    //document.getElementById('views-selector').classList.remove('active'); 
    // Resetar estilos de scroll
    contextMenu.style.maxHeight = '';
    contextMenu.style.overflowY = '';
}

