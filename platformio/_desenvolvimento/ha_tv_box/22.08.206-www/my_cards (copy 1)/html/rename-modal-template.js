export const renameModalTemplate = `
  <div class="rename-container" id="rename-container" style="display: none;">
    <!-- Guardar dados do dispositivo a ser renomeado -->
    <input type="text" id="save_device_name" name="save_device_name" style="display: none;">
    <input type="text" id="save_device_id" name="save_device_id" style="display: none;">

    <!-- Botão de fechar -->
    <button class="close-btn" id="closeBtn" title="Fechar">×</button>
    
    <div class="rename-header">
      <div class="rename-icon">
        <svg viewBox="0 0 24 24" fill="none" stroke-width="2">
          <path d="M20 14.66V20a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h5.34"></path>
          <polygon points="18 2 22 6 12 16 8 16 8 12 18 2"></polygon>
        </svg>
      </div>
      <h1>Renomear Dispositivo</h1>
      <p>Atualize o Nome do Dispositivo</p>
    </div>
    
    <!-- Mensagem de feedback -->
    <div id="message" class="message hidden"></div>
    
    <!-- Campo para editar nome -->
    <div class="name-input-wrapper">
      <label class="name-label" for="deviceName">Nome do Dispositivo</label>
      <input 
        type="text" 
        id="deviceName" 
        class="name-input" 
        value="" 
        maxlength="30" 
        disabled
        placeholder="Digite o Novo Nome"
        autocomplete="off"
      >
      <div id="charCounter" class="char-counter">0/30 caracteres</div>
    </div>
    
    <!-- Botões de ação -->
    <div class="actions-row">
      <button id="editBtn" class="action-btn btn-edit">
        <svg viewBox="0 0 24 24" fill="none" stroke-width="2">
          <path d="M20 14.66V20a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h5.34"></path>
          <polygon points="18 2 22 6 12 16 8 16 8 12 18 2"></polygon>
        </svg>
        Editar Nome
      </button>
      
      <button id="saveBtn" class="action-btn btn-save hidden">
        <svg viewBox="0 0 24 24" fill="none" stroke-width="2">
          <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path>
          <polyline points="17 21 17 13 7 13 7 21"></polyline>
          <polyline points="7 3 7 8 15 8"></polyline>
        </svg>
        Salvar
      </button>
      
      <button id="cancelBtn" class="action-btn btn-cancel hidden">
        <svg viewBox="0 0 24 24" fill="none" stroke-width="2">
          <line x1="18" y1="6" x2="6" y2="18"></line>
          <line x1="6" y1="6" x2="18" y2="18"></line>
        </svg>
        Cancelar
      </button>
    </div>
  </div>
`;