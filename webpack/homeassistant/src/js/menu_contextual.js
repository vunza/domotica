
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
        currentMenuCardId = cardId;
        
        // Posicionar menu próximo ao botão clicado
        const rect = menuTrigger.getBoundingClientRect();
        contextMenu.style.top = `${rect.bottom + window.scrollY + 5}px`;
        contextMenu.style.left = `${rect.left + window.scrollX - 180}px`;
        
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
        case 'move':
            openViewsSelector(cardId);
            break;
        case 'duplicate':
            duplicateCard(cardId);
            break;
        case 'copy':
            copyCard(cardId);
            break;
        case 'cut':
            cutCard(cardId);
            break;
        case 'delete':
            deleteCard(cardId);
            break;
    }
    
    closeAllMenus();
});



// Funções das ações
function openViewsSelector(cardId) {
    document.getElementById('views-selector').classList.add('active');
    
    // Atualizar seleção
    const viewOptions = document.querySelectorAll('.view-option');
    viewOptions.forEach(option => option.classList.remove('selected'));
    
    // Configurar botão de confirmação
    document.getElementById('confirm-move').onclick = function() {
        const selectedView = document.querySelector('.view-option.selected');
        if (selectedView) {
            moveCardToView(cardId, selectedView.dataset.view);
        }
        closeAllMenus();
    };
    
    // Configurar cancelamento
    document.getElementById('cancel-move').onclick = closeAllMenus;
}

function moveCardToView(cardId, viewName) {
    showActionIndicator(`Card movido para: ${viewName}`);
    console.log(`Card ${cardId} movido para view: ${viewName}`);
}

function duplicateCard(cardId) {
    const cardIndex = state.cards.findIndex(card => card.id === cardId);
    if (cardIndex === -1) return;
    
    const originalCard = state.cards[cardIndex];
    const newCard = {
        ...originalCard,
        id: Date.now(), // Novo ID único
        title: `${originalCard.title} (Cópia)`
    };
    
    state.cards.splice(cardIndex + 1, 0, newCard);
    initDashboard();
    showActionIndicator('Card duplicado com sucesso!');
}

function copyCard(cardId) {
    const card = state.cards.find(card => card.id === cardId);
    if (card) {
        state.clipboard = {...card};
        state.clipboardAction = 'copy';
        updateClipboardStatus();
        showActionIndicator('Card copiado para a clipboard!');
    }
}

function cutCard(cardId) {
    const card = state.cards.find(card => card.id === cardId);
    if (card) {
        state.clipboard = {...card};
        state.clipboardAction = 'cut';
        updateClipboardStatus();
        showActionIndicator('Card recortado para a clipboard!');
    }
}

function deleteCard(cardId) {
    if (confirm('Tem certeza que deseja excluir este card?')) {
        state.cards = state.cards.filter(card => card.id !== cardId);
        initDashboard();
        showActionIndicator('Card excluído com sucesso!');
    }
}

// Atualizar status da clipboard
function updateClipboardStatus() {
    /*const clipboardContent = document.getElementById('clipboard-content');
    if (state.clipboard) {
        clipboardContent.textContent = `${state.clipboard.title} (${state.clipboardAction === 'copy' ? 'Cópia' : 'Recorte'})`;
        clipboardContent.style.color = state.clipboardAction === 'cut' ? '#e53935' : '#41bdf5';
    } else {
        clipboardContent.textContent = 'Vazio';
        clipboardContent.style.color = '#666';
    }*/
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
    document.getElementById('views-selector').classList.remove('active');
}