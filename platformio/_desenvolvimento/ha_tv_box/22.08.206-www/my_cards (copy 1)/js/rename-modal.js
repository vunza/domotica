// rename-modal.js

/**
 * Inicializa o modal de renomear dispositivo.
 * @param {Object} cardInstance - Instância do LampCard (this).
 * @param {ShadowRoot} shadow - ShadowRoot do card.
 * @returns {{openModal: Function}} Objeto com a função para abrir o modal.
 */
export function initRenameModal(cardInstance, shadow) {
  const modal = shadow.getElementById('rename-container');
  const closeBtn = shadow.getElementById('closeBtn');
  const editBtn = shadow.getElementById('editBtn');
  const saveBtn = shadow.getElementById('saveBtn');
  const cancelBtn = shadow.getElementById('cancelBtn');
  const deviceNameInput = shadow.getElementById('deviceName');
  const charCounter = shadow.getElementById('charCounter');
  const saveDeviceName = shadow.getElementById('save_device_name');
  const saveDeviceId = shadow.getElementById('save_device_id');
  const messageDiv = shadow.getElementById('message');

  if (!modal || !closeBtn || !editBtn || !saveBtn || !cancelBtn || !deviceNameInput) {
    console.error('Elementos do modal de renomear não encontrados!');
    return {
      openModal: () => console.warn('Modal de renomear não disponível')
    };
  }

  /**
   * Exibe uma mensagem temporária no modal.
   */
  const showMessage = (text, type = 'success') => {
    messageDiv.textContent = text;
    messageDiv.className = `message ${type}`;
    messageDiv.classList.remove('hidden');
    setTimeout(() => messageDiv.classList.add('hidden'), 3000);
  };

  /**
   * Atualiza o contador de caracteres do campo de nome.
   */
  const updateCharCounter = () => {
    const len = deviceNameInput.value.length;
    charCounter.textContent = `${len}/30 caracteres`;
    charCounter.classList.remove('warning', 'danger');
    if (len > 25) charCounter.classList.add('warning');
    if (len >= 30) charCounter.classList.add('danger');
  };

  /**
   * Fecha o modal e limpa a mensagem.
   */
  const closeModal = () => {
    modal.style.display = 'none';
    messageDiv.classList.add('hidden');
  };

  // Event listeners (configurados uma vez)
  closeBtn.addEventListener('click', closeModal);

  editBtn.addEventListener('click', () => {
    deviceNameInput.disabled = false;
    deviceNameInput.focus();
    editBtn.classList.add('hidden');
    saveBtn.classList.remove('hidden');
    cancelBtn.classList.remove('hidden');
  });

  cancelBtn.addEventListener('click', () => {
    deviceNameInput.value = saveDeviceName.value;
    deviceNameInput.disabled = true;
    editBtn.classList.remove('hidden');
    saveBtn.classList.add('hidden');
    cancelBtn.classList.add('hidden');
    updateCharCounter();
  });

  deviceNameInput.addEventListener('input', updateCharCounter);

  saveBtn.addEventListener('click', () => {
    const newName = deviceNameInput.value.trim();
    if (!newName) {
      showMessage('O nome não pode estar vazio', 'error');
      return;
    }

    // Animação no botão
    saveBtn.disabled = true;
    const originalSaveText = saveBtn.innerHTML;
    saveBtn.innerHTML = '<svg viewBox="0 0 24 24" fill="none" stroke-width="2"><path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"></path></svg> Salvando...';

    const entityId = saveDeviceId.value;

    cardInstance._hass.callService('shell_command', 'atualiza_friendly', {
      entity_id: entityId,
      friendly_name: newName,
      entity_state: 'unknown',
      device_type: 'unknown'
    })
    .then(() => {
      // Atualiza o card localmente
      cardInstance._customNameProvided = true;
      cardInstance._customName = newName; // importante: atualizar também _customName
      const titleElem = shadow.querySelector('.card-title-content');
      if (titleElem) titleElem.textContent = newName;
      saveDeviceName.value = newName;
      deviceNameInput.disabled = true;
      editBtn.classList.remove('hidden');
      saveBtn.classList.add('hidden');
      cancelBtn.classList.add('hidden');
      showMessage('Nome atualizado com sucesso!');
      saveBtn.disabled = false;
      saveBtn.innerHTML = originalSaveText;
      setTimeout(closeModal, 5000);
    })
    .catch((error) => {
      console.error('Erro ao chamar serviço:', error);
      showMessage('Erro ao atualizar nome. Tente novamente.', 'error');
      saveBtn.disabled = false;
      saveBtn.innerHTML = originalSaveText;
    });
  });

  // Fecha o modal ao clicar fora
  modal.addEventListener('click', (e) => {
    if (e.target === modal) closeModal();
  });

  /**
   * Abre o modal de renomear, preenchendo com o nome atual.
   */
  const openRenameModal = () => {
    const currentName = cardInstance._customNameProvided && cardInstance._customName
      ? cardInstance._customName
      : (cardInstance._stateObj ? cardInstance._stateObj.attributes.friendly_name : '') ||
        cardInstance._config.entity.split('.')[1] ||
        'Dispositivo';

    deviceNameInput.value = currentName;
    deviceNameInput.disabled = true;
    saveDeviceName.value = currentName;
    saveDeviceId.value = cardInstance._config.entity;
    updateCharCounter();

    editBtn.classList.remove('hidden');
    saveBtn.classList.add('hidden');
    cancelBtn.classList.add('hidden');

    modal.style.display = 'block';
  };

  return {
    openModal: openRenameModal
  };
}