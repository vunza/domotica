

import {getToken, ip_e_porta} from './vars_funcs_globais.js';
//import {} from './menu_contextual.js';

// Elementos DOM
const deviceNameInput = document.getElementById('deviceName');
const editBtn = document.getElementById('editBtn');
const saveBtn = document.getElementById('saveBtn');
const cancelBtn = document.getElementById('cancelBtn');
const messageEl = document.getElementById('message');
const charCounter = document.getElementById('charCounter');
const closeBtn = document.getElementById('closeBtn');

let saved_device_id = null;
let saved_device_name = null;

// Estado inicial
let isEditing = false;


// Atualizar contador de caracteres
function updateCharCounter() {
    const length = deviceNameInput.value.length;
    const maxLength = deviceNameInput.maxLength;
    charCounter.textContent = `${length}/${maxLength} caracteres`;
    
    // Mudar cor conforme o limite se aproxima
    charCounter.classList.remove('warning', 'danger');
    if (length > maxLength * 0.8) {
        charCounter.classList.add('warning');
    }
    if (length > maxLength * 0.95) {
        charCounter.classList.add('danger');
    }    
}

// Mostrar mensagem de feedback
function showMessage(text, type = 'success') {
    messageEl.textContent = text;
    messageEl.className = `message ${type}`;
    messageEl.classList.remove('hidden');
    
    // Esconder após 3 segundos
    setTimeout(() => {
        messageEl.classList.add('hidden');
    }, 3000);
}

// Função para fechar o container
function closeContainer() {
   
    saved_device_name = document.getElementById('save_device_name').value;

    // Se estiver editando, perguntar se quer descartar alterações
    if (isEditing && deviceNameInput.value.trim() !== saved_device_name.trim()) {
        if (!confirm('Tem alterações não salvas. Deseja realmente fechar?')) {
            return;
        }
    }
    
    // Em um ambiente real, você poderia:
    // 1. Remover o container do DOM
    // document.querySelector('.rename-container').remove();
    
    // 2. Esconder o container
    // document.querySelector('.rename-container').style.display = 'none';
    
    // 3. Redirecionar para outra página
    // window.location.href = 'index.html';
    
    // 4. Disparar um evento customizado
    // document.dispatchEvent(new CustomEvent('rename-closed'));
    
    // Para este exemplo, vamos mostrar uma mensagem e esconder o container
    //showMessage('Interface fechada', 'success');
    
    // Simular fechamento após 1 segundo
    setTimeout(() => {
        document.querySelector('.rename-container').style.opacity = '0';
        document.querySelector('.rename-container').style.transform = 'scale(0.9)';
        document.querySelector('.rename-container').style.transition = 'all 0.3s ease';

        //document.getElementById('cards-container').style.display = 'grid';
        document.getElementById('cards_main_wrapper').style.display = 'block';
        document.getElementById('rename-container').style.display = 'none';   
        window.location.href = 'index.html';    
       
    }, 1000);    
    
}

// Iniciar edição
function startEditing() {
    isEditing = true;
    deviceNameInput.disabled = false;
    deviceNameInput.focus();
    deviceNameInput.select();
    
    editBtn.classList.add('hidden');
    saveBtn.classList.remove('hidden');
    cancelBtn.classList.remove('hidden');
}


// Salvar alterações
async function saveChanges() {

    const newName = deviceNameInput.value.trim();
    saved_device_id = document.getElementById('save_device_id').value;
    saved_device_name = document.getElementById('save_device_name').value;   
    
    // Validações
    if (!newName) {
        showMessage('Indique o Novo Nome', 'error');
        deviceNameInput.focus();
        return;
    }
    
    if (newName === saved_device_name) {
        showMessage('O Nome não foi alterado', 'error');
        cancelEditing();
        return;
    }

    // Animação no botão para salvar
    saveBtn.disabled = true;
    const originalSaveText = saveBtn.innerHTML;
    saveBtn.innerHTML = '<svg viewBox="0 0 24 24" fill="none" stroke-width="2"><path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"></path></svg> Salvando...';

    // Obter ID do dispositivo
    saved_device_id = document.getElementById('save_device_id').value;    
    
    // Obter Token de acesso ao HA
    const api_token = '/local/json_files/token_api.json';
    const token = await getToken(api_token);

    // Realizar a atualização do nome amigável via API
    actualizaFriendlyName(token, ip_e_porta, saved_device_id, newName, 'unknown', 'unknown').then(() => {
        
        setTimeout(() => {
            document.getElementById('deviceName').value = newName;         
            showMessage(`Nome alterado para "${newName}"`);        
            // Resetar interface
            cancelEditing();
            saveBtn.disabled = false;
            saveBtn.innerHTML = originalSaveText;            
        }, 500);

    }).catch((error) => {
        console.error('Erro ao atualizar friendly name:', error);
    });   
}

// Cancelar edição
function cancelEditing() {
    isEditing = false;
    if (document.getElementById('save_device_name').value.trim() === deviceNameInput.value.trim()) 
        deviceNameInput.value = document.getElementById('save_device_name').value.trim();
    deviceNameInput.disabled = true;
    
    editBtn.classList.remove('hidden');
    saveBtn.classList.add('hidden');
    cancelBtn.classList.add('hidden');
    
    updateCharCounter();
}

// Event Listeners
editBtn.addEventListener('click', startEditing);
saveBtn.addEventListener('click', saveChanges);
cancelBtn.addEventListener('click', cancelEditing);
closeBtn.addEventListener('click', closeContainer);

// Atualizar contador ao digitar
deviceNameInput.addEventListener('input', updateCharCounter);

// Permitir salvar com Enter e cancelar com Escape
deviceNameInput.addEventListener('keydown', (e) => {
    if (e.key === 'Enter' && isEditing) {
        saveChanges();
    } else if (e.key === 'Escape' && isEditing) {
        cancelEditing();
    }
});

// Fechar com a tecla Escape (quando não estiver editando)
document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape' && !isEditing) {
        //closeContainer();
    }
});



async function actualizaFriendlyName(token, ip_porta, entityId, friendlyName, entityState, device_type) {
   
    const url = `${ip_porta}/api/services/shell_command/atualiza_friendly`;

    fetch(url, {
    method: "POST",
    headers: {
        "Authorization": `Bearer ${token}`,
        "Content-Type": "application/json"
    },
    body: JSON.stringify({
        entity_id: entityId,
        friendly_name: friendlyName,
        entity_state: entityState,
        device_type: device_type
    })
    })
    .then(response => {
    if (!response.ok) {
        throw new Error("Erro na requisição");
    }
    return response.json();
    })
    .then(data => {
    console.log("Comando enviado com sucesso:", data);
    })
    .catch(error => {
    console.error("Erro:", error);
    });

}



// Inicializar
document.addEventListener('DOMContentLoaded', () => {       
    updateCharCounter();    
});

//export {  }