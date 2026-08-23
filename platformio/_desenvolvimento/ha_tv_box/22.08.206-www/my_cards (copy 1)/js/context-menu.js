/**
 * Classe responsável por gerenciar um menu contextual flutuante.
 * Ajusta automaticamente a posição para cima se não houver espaço abaixo.
 */
export class ContextMenu {
  /**
   * Cria uma instância do menu contextual.
   * 
   * @param {ShadowRoot} shadowRoot - O shadowRoot onde o menu está inserido.
   * @param {string} triggerSelector - Seletor CSS do elemento que abre/fecha o menu.
   * @param {string} menuSelector - Seletor CSS do elemento do menu.
   * @param {Function} onAction - Callback chamado quando um item do menu é clicado.
   *                              Recebe o valor do atributo data-action do item.
   */
  constructor(shadowRoot, triggerSelector, menuSelector, onAction) {
    /**
     * @type {ShadowRoot}
     * @private
     */
    this.shadowRoot = shadowRoot;

    /**
     * @type {Element|null}
     * @private
     */
    this.trigger = shadowRoot.querySelector(triggerSelector);

    /**
     * @type {Element|null}
     * @private
     */
    this.menu = shadowRoot.querySelector(menuSelector);

    /**
     * @type {Function}
     * @private
     */
    this.onAction = onAction;

    /**
     * @type {Object|null}
     * @private
     */
    this._hass = null;

    /**
     * @type {boolean}
     * @private
     */
    this.isOpen = false;

    if (!this.trigger || !this.menu) {
      console.warn('ContextMenu: trigger ou menu não encontrado');
      return;
    }

    /**
     * @type {Function}
     * @private
     */
    this._boundHandleOutsideClick = this._handleOutsideClick.bind(this);

    /**
     * @type {Function}
     * @private
     */
    this._boundTriggerClick = this._handleTriggerClick.bind(this);

    /**
     * @type {Function}
     * @private
     */
    this._boundMenuItemClick = this._handleMenuItemClick.bind(this);

    /**
     * @type {Function}
     * @private
     */
    this._boundScroll = () => this.close();

    this._attachEvents();         
    
  }



  /**
   * Define o objeto hass (chamado pelo card quando disponível).
   */
  setHass(hass) {
    this._hass = hass; 
  } 

 

  /**
   * Alterna a abertura/fechamento do menu.
   * @public
   */
  toggle() {
    this.isOpen = !this.isOpen;
    if (this.isOpen) {
      this._adjustPosition();
    }
    this._updateVisibility();
  }

  /**
   * Abre o menu.
   * @public
   */
  open() {
    this.isOpen = true;
    this._adjustPosition();
    this._updateVisibility();      
  }

  /**
   * Fecha o menu.
   * @public
   */
  close() {
    this.isOpen = false;
    this._updateVisibility();
  }

  /**
   * Ajusta a posição do menu (para cima ou para baixo) conforme o espaço disponível.
   * @private
   */
  _adjustPosition() {    
    const triggerRect = this.trigger.getBoundingClientRect();
    const menuHeight = this.menu.offsetHeight;
    const viewportHeight = window.innerHeight;
    const spaceBelow = viewportHeight - triggerRect.bottom;
    const spaceAbove = triggerRect.top;

    this.menu.classList.remove('context-menu--up', 'context-menu--down');

    if (spaceBelow < menuHeight && spaceAbove > spaceBelow) {
      // Abre para cima se não há espaço abaixo e há mais espaço acima
      this.menu.classList.add('context-menu--up');
    } else {
      // Abre para baixo (padrão)
      this.menu.classList.add('context-menu--down');
    }
  }

  /**
   * Atualiza a visibilidade do menu e gerencia os listeners globais.
   * @private
   */
  _updateVisibility() {
    if (this.isOpen) {
      this.menu.classList.add('active');
      document.addEventListener('click', this._boundHandleOutsideClick);
      window.addEventListener('scroll', this._boundScroll, { passive: true });
      window.addEventListener('resize', this._boundScroll);

    } else {
      this.menu.classList.remove('active');
      document.removeEventListener('click', this._boundHandleOutsideClick);
      window.removeEventListener('scroll', this._boundScroll);
      window.removeEventListener('resize', this._boundScroll);
    }
    
  }


  /*_handleUloadMenuItem(event){  

    const menuTrigger = event.target.closest('.menu-trigger');
    if (!menuTrigger) return;
    const cardElement = menuTrigger.closest('.card');
    if (!cardElement) return;
    const cardHeader = cardElement.querySelector('.card-header');
    const saved_device_mac = cardHeader ? cardHeader.getAttribute('data-device-mac-address') : null;

    const menuItem = this.shadowRoot.querySelector('.menu-item[data-action="upload_sketch"]');        
    
    if (saved_device_mac.length == 17){ 
      menuItem.style.display = 'flex';
    }
    else {
      menuItem.style.display = 'none';
    }
    
  }*/


  /**
   * Updates the visibility of the "upload_sketch" menu item based on the presence of a valid MAC address
   * in the card's header.
   *
   * @param {Event} event - The click event from the menu trigger.
   */
  _updateUploadMenuItemVisibility(event) {
    const menuTrigger = event.target.closest('.menu-trigger');
    if (!menuTrigger) return;

    const cardElement = menuTrigger.closest('.card');
    if (!cardElement) return;

    const cardHeader = cardElement.querySelector('.card-header');
    const macAddress = cardHeader?.getAttribute('data-device-mac-address') || '';

    const menuItem = this.shadowRoot?.querySelector('.menu-item[data-action="upload_sketch"]');
    if (!menuItem) return;

    // MAC addresses typically have 17 characters (e.g., "AA:BB:CC:DD:EE:FF")
    const isValidMac = macAddress.length === 17;
    menuItem.style.display = isValidMac ? 'flex' : 'none';
  } 



  /**
   * Manipula o clique no botão trigger.
   * @private
   * @param {Event} event - O evento de clique.
   */
  _handleTriggerClick(event) {
    event.stopPropagation();
    this.toggle();
    this._updateUploadMenuItemVisibility(event);         
  }



  /**
   * Manipula o clique em um item do menu.
   * @private
   * @param {Event} event - O evento de clique.
   */
  _handleMenuItemClick(event) {
    event.stopPropagation();
    const action = event.currentTarget.dataset.action;
    if (this.onAction) {
      this.onAction(action);
    }
    this.close();
  }



  /**
   * Manipula cliques fora do menu para fechá-lo.
   * @private
   * @param {Event} event - O evento de clique.
   */
  _handleOutsideClick(event) {
    if (!this.menu.contains(event.target) && !this.trigger.contains(event.target)) {
      this.close();
    }
  }

  /**
   * Registra todos os listeners necessários.
   * @private
   */
  _attachEvents() {
    this.trigger.addEventListener('click', this._boundTriggerClick);
    const menuItems = this.menu.querySelectorAll('.menu-item');
    menuItems.forEach(item => {
      item.addEventListener('click', this._boundMenuItemClick);
    });
  }

  /**
   * Remove todos os listeners e limpa a instância.
   * @public
   */
  destroy() {
    this.trigger.removeEventListener('click', this._boundTriggerClick);
    const menuItems = this.menu.querySelectorAll('.menu-item');
    menuItems.forEach(item => {
      item.removeEventListener('click', this._boundMenuItemClick);
    });
    document.removeEventListener('click', this._boundHandleOutsideClick);
    window.removeEventListener('scroll', this._boundScroll);
    window.removeEventListener('resize', this._boundScroll);
  }
}