
// Elementos principais
const contextMenu = document.getElementById('context-menu');
let currentMenuCardId = null;

// Abrir menu contextual ao clicar nos botões de menu
document.addEventListener('click', function(e) {
    const menuTrigger = e.target.closest('.menu-trigger');
    
    if (menuTrigger) {
        e.preventDefault();
        e.stopPropagation();
        
        const cardId = parseInt(menuTrigger.dataset.id);
        currentMenuCardId = cardId;
        
        // Posicionar o menu contextual
        positionContextMenu(menuTrigger);
        
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

// Função para posicionar o menu contextual corretamente
function positionContextMenu(menuTrigger) {
    const rect = menuTrigger.getBoundingClientRect();
    const MARGIN = 10;
    const MENU_WIDTH = 240; // Largura do menu do CSS
    const MENU_HEIGHT = contextMenu.offsetHeight || 250; // Altura estimada
    
    // 1. POSICIONAMENTO HORIZONTAL
    let left = rect.left + window.scrollX - 180;
    
    // Verificar se o menu sai pela direita
    if (left + MENU_WIDTH > window.innerWidth + window.scrollX) {
        left = window.innerWidth + window.scrollX - MENU_WIDTH - MARGIN;
    }
    // Verificar se o menu sai pela esquerda
    if (left < window.scrollX) {
        left = window.scrollX + MARGIN;
    }
    
    // 2. POSICIONAMENTO VERTICAL
    let top;
    const spaceBelow = window.innerHeight + window.scrollY - (rect.bottom + MARGIN);
    
    if (spaceBelow >= MENU_HEIGHT) {
        // Cabe abaixo do botão
        top = rect.bottom + window.scrollY + MARGIN;
    } else {
        // Não cabe abaixo - posicionar acima do botão
        const spaceAbove = rect.top + window.scrollY - MARGIN;
        if (spaceAbove >= MENU_HEIGHT) {
            // Cabe acima do botão
            top = rect.top + window.scrollY - MENU_HEIGHT - MARGIN;
        } else {
            // Não cabe nem acima nem abaixo - usar espaço abaixo mesmo que com scroll
            top = rect.bottom + window.scrollY + MARGIN;
            // Limitar altura do menu ao espaço disponível
            contextMenu.style.maxHeight = `${spaceBelow - 10}px`;
            contextMenu.style.overflowY = 'auto';
        }
    }
    
    // 3. APLICAR POSIÇÕES
    contextMenu.style.top = `${top}px`;
    contextMenu.style.left = `${left}px`;
}

// Ações do menu
contextMenu.addEventListener('click', function(e) {
    const menuItem = e.target.closest('.menu-item');
    if (!menuItem || !currentMenuCardId) return;
    
    const action = menuItem.dataset.action;
    const cardId = currentMenuCardId;
    
    // Executar ação baseada no data-action
    switch(action) {
        case 'schedule':
            console.log(`Programar card ${cardId}`);
            alert(`Programação do card ${cardId} iniciada.`);
            break;
        case 'rename':
            renameCard(cardId);
            break;
        case 'timer':
            console.log(`Temporizar card ${cardId}`);
            alert(`Temporizador configurado para o card ${cardId}.`);
            break;
        case 'delete':
            if (confirm(`Tem certeza que deseja remover o card ${cardId}?`)) {
                console.log(`Remover card ${cardId}`);
                alert(`Card ${cardId} removido com sucesso!`);
            }
            break;
        case 'close':
            closeAllMenus();
            break;
    }
    
    closeAllMenus();
});

// Fechar menu com ESC
document.addEventListener('keydown', function(e) {
    if (e.key === 'Escape') {
        closeAllMenus();
    }
});

// Fechar todos os menus
function closeAllMenus() {
    contextMenu.classList.remove('active');
    // Resetar estilos de scroll
    contextMenu.style.maxHeight = '';
    contextMenu.style.overflowY = '';
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
        document.getElementById('cards-container').style.display = 'none';
        // Mostrar interface de renomear
        document.getElementById('rename-container').style.display = 'block';
    }    
}
