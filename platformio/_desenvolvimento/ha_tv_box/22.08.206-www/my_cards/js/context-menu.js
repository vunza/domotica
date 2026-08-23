/**
 * Classe responsável por gerenciar um menu contextual flutuante.
 * Ajusta automaticamente a posição para cima se não houver espaço abaixo.
 */
export class ContextMenu {
  /**
   * @param {ShadowRoot} shadowRoot - O shadowRoot onde o menu está inserido.
   * @param {string} triggerSelector - Seletor CSS do elemento que abre/fecha o menu.
   * @param {string} menuSelector - Seletor CSS do elemento do menu.
   * @param {Function} onAction - Callback chamado quando um item do menu é clicado.
   */
  constructor(shadowRoot, triggerSelector, menuSelector, onAction) {
    this.shadowRoot = shadowRoot;
    this.trigger = shadowRoot.querySelector(triggerSelector);
    this.menu = shadowRoot.querySelector(menuSelector);
    this.onAction = onAction;
    this._hass = null;
    this.isOpen = false;

    if (!this.trigger || !this.menu) {
      console.warn('ContextMenu: trigger ou menu não encontrado');
      return;
    }

    // Handlers de evento encapsulados com bind
    this._boundOutsideClick = this._handleOutsideClick.bind(this);
    this._boundTriggerClick = this._handleTriggerClick.bind(this);
    this._boundMenuClick = this._handleMenuClick.bind(this);
    this._boundClose = () => this.close();

    this._attachEvents();
  }

  /**
   * Define o objeto hass.
   */
  setHass(hass) {
    this._hass = hass;
  }

  /**
   * Alterna a abertura/fechamento do menu.
   */
  toggle() {
    this.isOpen ? this.close() : this.open();
  }

  /**
   * Abre o menu.
   */
  open() {
    this.isOpen = true;
    this._adjustPosition();
    this._updateVisibility();
  }

  /**
   * Fecha o menu.
   */
  close() {
    if (!this.isOpen) return;
    this.isOpen = false;
    this._updateVisibility();
  }

  /**
   * Ajusta a posição do menu conforme o espaço na janela.
   * @private
   */
  _adjustPosition() {
    const triggerRect = this.trigger.getBoundingClientRect();
    const menuHeight = this.menu.offsetHeight;
    const spaceBelow = window.innerHeight - triggerRect.bottom;
    const spaceAbove = triggerRect.top;

    const isUp = spaceBelow < menuHeight && spaceAbove > spaceBelow;
    this.menu.classList.toggle('context-menu--up', isUp);
    this.menu.classList.toggle('context-menu--down', !isUp);
  }

  /**
   * Atualiza a visibilidade do menu e gerencia os listeners globais.
   * @private
   */
  _updateVisibility() {
    this.menu.classList.toggle('active', this.isOpen);

    const action = this.isOpen ? 'addEventListener' : 'removeEventListener';
    document[action]('click', this._boundOutsideClick);
    window[action]('scroll', this._boundClose, { passive: true });
    window[action]('resize', this._boundClose);
  }

  /**
   * Atualiza a visibilidade do item "upload_sketch" baseado no MAC.
   * @private
   */
  _updateUploadMenuItemVisibility() {
    const cardHeader = this.shadowRoot.querySelector('.card-header');
    const macAddress = cardHeader?.getAttribute('data-device-mac-address') || '';
    const menuItem = this.menu.querySelector('.menu-item[data-action="upload_sketch"]');

    if (menuItem) {
      menuItem.style.display = macAddress.length === 17 ? 'flex' : 'none';
    }
  }

  /**
   * Manipula o clique no botão trigger.
   * @private
   */
  _handleTriggerClick(event) {
    event.stopPropagation();
    this.toggle();
    if (this.isOpen) {
      this._updateUploadMenuItemVisibility();
    }
  }

  /**
   * Manipula o clique no menu utilizando delegação de eventos.
   * @private
   */
  _handleMenuClick(event) {
    const item = event.target.closest('.menu-item');
    if (!item) return;

    event.stopPropagation();
    const action = item.dataset.action;

    if (this.onAction) {
      this.onAction(action);
    }
    this.close();
  }

  /**
   * Manipula cliques fora do menu.
   * @private
   */
  _handleOutsideClick(event) {
    if (!this.menu.contains(event.target) && !this.trigger.contains(event.target)) {
      this.close();
    }
  }

  /**
   * Registra os listeners iniciais.
   * @private
   */
  _attachEvents() {
    this.trigger.addEventListener('click', this._boundTriggerClick);
    this.menu.addEventListener('click', this._boundMenuClick);
  }

  /**
   * Limpa todos os listeners e referências da instância.
   */
  destroy() {
    this.trigger?.removeEventListener('click', this._boundTriggerClick);
    this.menu?.removeEventListener('click', this._boundMenuClick);
    
    document.removeEventListener('click', this._boundOutsideClick);
    window.removeEventListener('scroll', this._boundClose, { passive: true });
    window.removeEventListener('resize', this._boundClose);
  }
}