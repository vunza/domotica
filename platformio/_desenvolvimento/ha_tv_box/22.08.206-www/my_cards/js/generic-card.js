import { ContextMenu } from './context-menu.js';
import { cardStyles } from '../css/generic-card-styles.js';
import { contextMenuStyles } from '../css/context-menu-styles.js';
import { renameModalStyles } from '../css/rename-modal-styles.js';
import { timerModalStyles } from '../css/timer-modal-styles.js';
import { cardTemplate } from '../html/generic-card-template.js';
import { menuTemplate } from '../html/menu-template.js';
import { renameModalTemplate } from '../html/rename-modal-template.js';
import { timerModalTemplate } from '../html/timer-modal-template.js';

import { initTimerModal, getNextTimer2Execute, obterDadosTimers } from './timer-modal.js';
import { initRenameModal } from './rename-modal.js';
import { createMenuHandler } from './menu-context-modal.js';
import { getToken, ip_e_porta } from './globals.js';

/** Mapeamento estático dos dias da semana para o temporizador */
const WEEKDAY_MAP = {
  sun: 'Domingo', mon: 'Segunda-feira', tue: 'Terça-feira',
  wed: 'Quarta-feira', thu: 'Quinta-feira', fri: 'Sexta-feira', sat: 'Sábado'
};

/** Mapeamento de atributos e unidades conhecidos para renderização rápida */
const ATTR_META = {
  ups1_current: { name: 'Corrente', unit: 'A' },
  ups1_voltage: { name: 'Tensão', unit: 'V' },
  ups1_temperature: { name: 'Temperatura', unit: '°C' },
  ups1_humidity: { name: 'Humidade', unit: '%' }
};

/**
 * Card personalizado para Home Assistant Lovelace.
 * @extends HTMLElement
 */
class GenericCard extends HTMLElement {
  static getStubConfig() {
    return { entity: '', name: '', show_time: true };
  }

  constructor() {
    super();
    this.attachShadow({ mode: 'open' });

    this._config = {};
    this._hass = null;
    this._stateObj = null;
    this._customNameProvided = false;
    this._contextMenu = null;
    this._customName = null;
    this._isAplug = false;
    this._extraEntities = [];
    this._displayAttributes = [];
    this._isResolving = false;

    // Cache de referências aos elementos do DOM
    this._els = {};
    this._onToggleClick = this._toggleEntity.bind(this);
  }

  connectedCallback() {
    if (!this._contextMenu && this.shadowRoot?.querySelector('.menu-trigger')) {
      this._initContextMenu();
    }
  }

  disconnectedCallback() {
    if (this._contextMenu) {
      this._contextMenu.destroy();
      this._contextMenu = null;
    }
  }

  setConfig(config) {
    if (!config.entity) throw new Error('Defina a entidade da lâmpada');
    this._config = { ...config };
    this._customNameProvided = !!config.name;
    this._customName = config.name || null;
    this._isAplug = config.is_a_plug ?? false;
    this._extraEntities = config.extra_entities || [];
    this._displayAttributes = config.display_attributes || [];

    if (this._hass && this._config.entity) {
      const freshState = this._hass.states[this._config.entity];
      if (freshState && freshState.state !== 'unavailable' && freshState.state !== 'unknown') {
        this._stateObj = freshState;
      }
    }

    this._render();
    this._updateContent();
  }

  set hass(hass) {
    this._hass = hass;

    if (this._config.entity) {
      const newState = hass.states[this._config.entity];

      if (newState && newState.state !== 'unavailable' && newState.state !== 'unknown') {
        // Estado válido recebido
        this._stateObj = newState;
      } else if (!newState) {
        // Entidade não encontrada em hass.states - tenta resolver se mudou o entity_id
        this._resolveEntityFromRegistry();
      }
      // Se o novo estado for 'unavailable' ou 'unknown', ignora temporariamente
      // para manter o último _stateObj válido em memória durante a renomeação.

      this._updateContent();
    }

    if (this._contextMenu) {
      this._contextMenu.setHass(hass);
    }
  }

  // Resolve a entidade caso o entity_id tenha mudado no registry do HA
  async _resolveEntityFromRegistry() {
    if (!this._hass || !this._config.entity || this._isResolving) return;
    this._isResolving = true;

    try {
      const registry = await this._hass.callWS({ type: 'config/entity_registry/list' });
      const oldEntry = registry.find(entry => entry.entity_id === this._config.entity);

      if (oldEntry && oldEntry.device_id) {
        const newEntry = registry.find(entry => 
          entry.device_id === oldEntry.device_id && !entry.disabled_by
        );

        if (newEntry && newEntry.entity_id !== this._config.entity) {
          this._config.entity = newEntry.entity_id;
          if (this._hass.states[newEntry.entity_id]) {
            this._stateObj = this._hass.states[newEntry.entity_id];
          }

          this.dispatchEvent(new CustomEvent('config-changed', {
            detail: { config: this._config },
            bubbles: true,
            composed: true
          }));

          this._updateContent();
        }
      }
    } catch (err) {
      console.warn('Erro ao resolver entidade no registro:', err);
    } finally {
      this._isResolving = false;
    }
  }

  _initContextMenu() {
    if (this._contextMenu) return;

    const handleMenuAction = createMenuHandler(this);
    this._contextMenu = new ContextMenu(
      this.shadowRoot,
      '.menu-trigger',
      '.context-menu',
      handleMenuAction
    );

    if (this._hass) {
      this._contextMenu.setHass(this._hass);
    }
  }

  async _render() {
    const shadow = this.shadowRoot;
    const clockSvg = `
      <svg class="svg_clk" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="10"></circle>
        <polyline points="12 6 12 12 16 14"></polyline>
      </svg>
    `;

    const initialTitle = this._customName || this._config.name || (this._stateObj?.attributes?.friendly_name) || 'Carregando...';

    shadow.innerHTML = `
      <style>
        ${cardStyles}
        ${contextMenuStyles}
        ${renameModalStyles}
        ${timerModalStyles}
      </style>
      ${cardTemplate(initialTitle, clockSvg, menuTemplate)}
      ${renameModalTemplate}      
      ${timerModalTemplate(this._config.name || this._config.entity.split('.')[1] || 'Dispositivo')}
    `;

    // Cache de referências aos elementos do DOM
    this._els = {
      title: shadow.querySelector('.card-title-content'),
      time: shadow.querySelector('.timestamp'),
      iconSpan: shadow.querySelector('#toggle-icon'),
      cardIcon: shadow.querySelector('#card-icon'),
      svgClock: shadow.querySelector('.svg_clk'),
      extraContainer: shadow.querySelector('.extra-entities'),
      cardHeader: shadow.querySelector('.card-header'),
      menuTrigger: shadow.querySelector('.menu-trigger')
    };

    // Evento de clique de toggle
    if (this._els.iconSpan) {
      this._els.iconSpan.removeEventListener('click', this._onToggleClick);
      this._els.iconSpan.addEventListener('click', this._onToggleClick);
    }

    // Inicialização de modais e menu
    const renameModal = initRenameModal(this, shadow);
    this._openRenameModal = renameModal.openModal;

    const timerModal = initTimerModal(this, shadow);
    this._openTimerModal = timerModal.openModal;
    this._closeTimerModal = timerModal.closeModal;

    this._initContextMenu();

    // Renderiza o conteúdo visual imediatamente de forma síncrona
    this._updateContent();

    // Requisições de rede em segundo plano
    try {
      const token = await getToken('/local/json_files/token_api.json');
      getNextTimer2Execute(token, ip_e_porta);
      await obterDadosTimers(token, ip_e_porta);
      this._updateContent();
    } catch (err) {
      console.error('Erro ao carregar dados dos timers:', err);
    }
  }

  _toggleEntity(e) {
    if (e) e.stopPropagation();
    if (!this._hass || !this._config.entity) return;

    const domain = this._config.entity.split('.')[0] || 'homeassistant';
    this._hass.callService(domain, 'toggle', {
      entity_id: this._config.entity
    }).catch(() => {
      this._hass.callService('homeassistant', 'toggle', {
        entity_id: this._config.entity
      });
    });
  }

  _updateContent() {
    if (!this._hass) return;

    // Tenta obter o estado mais recente caso não esteja em memória
    if (!this._stateObj && this._config.entity) {
      this._stateObj = this._hass.states[this._config.entity];
    }

    const { title, time, iconSpan, cardIcon, svgClock, extraContainer, cardHeader, menuTrigger } = this._els;

    // Atualiza o título imediatamente
    if (title) {
      const displayName = (this._customNameProvided && this._customName)
        ? this._customName
        : (this._stateObj?.attributes?.friendly_name || this._config.name || this._config.entity.split('.')[1] || 'Dispositivo');
      title.textContent = displayName;
    }

    if (!this._stateObj) return;

    if (this._isAplug && cardIcon) {
      cardIcon.setAttribute('icon', 'mdi:power-plug');
    }

    // 1. Atualizar atributos do Header e Menu Trigger
    if (cardHeader) {
      cardHeader.setAttribute('data-device-mac-address', this._stateObj.attributes?.mac_address || '');
    }
    if (menuTrigger) {
      menuTrigger.setAttribute('data-id', this._config.entity || '');
    }

    // 2. Determinação flexível de estado (tolerante a maiúsculas e variações de payload)
    const rawState = String(this._stateObj.state).toLowerCase();
    const isOn = ['on', 'true', 'home', 'open', 'unlocked', '1'].includes(rawState);

    // 3. Aplicação de cor direta no container e no elemento <ha-icon>
    const activeColor = 'var(--paper-item-icon-active-color, #fdd835)';
    const inactiveColor = 'var(--paper-item-icon-color, #44739e)';
    const targetColor = isOn ? activeColor : inactiveColor;

    if (iconSpan) iconSpan.style.color = targetColor;
    if (cardIcon) cardIcon.style.color = targetColor;

    // 4. Renderizar Entidades Extras e Atributos Customizados
    if (extraContainer) {
      let html = '';
      let hasContent = false;

      if (this._extraEntities.length > 0) {
        html += '<div class="extra-grid">';
        for (const item of this._extraEntities) {
          const stateObj = this._hass.states[item.entity];
          const value = stateObj ? stateObj.state : 'indisponível';
          const unit = item.unit || stateObj?.attributes?.unit_of_measurement || '';
          const displayName = item.name || item.entity.split('.').pop();

          html += `
            <div class="extra-item">
              <span class="extra-name">${displayName}</span>
              <span class="extra-value">${value} ${unit}</span>
            </div>`;
        }
        html += '</div>';
        hasContent = true;
      }

      if (this._displayAttributes.length > 0) {
        const attrs = this._stateObj.attributes || {};
        if (!hasContent) html += '<div class="extra-grid">';

        for (const attrKey of this._displayAttributes) {
          const val = attrs[attrKey] ?? '—';
          const meta = ATTR_META[attrKey] || {
            name: attrKey.replace(/^ups1_/, '').replace(/_/g, ' ').replace(/\b\w/g, l => l.toUpperCase()),
            unit: attrKey.includes('current') ? 'A' : attrKey.includes('voltage') ? 'V' : attrKey.includes('temperature') ? '°C' : attrKey.includes('humidity') ? '%' : ''
          };

          html += `
            <div class="extra-item">
              <span class="extra-name">${meta.name}</span>
              <span class="extra-value">${val} ${meta.unit}</span>
            </div>`;
        }
        if (!hasContent) html += '</div>';
        hasContent = true;
      }

      extraContainer.innerHTML = hasContent ? html : '';
    }

    // 5. Informações do Temporizador
    if (this._config.show_time && time) {
      const timerSensor = this._hass.states['sensor.info_dos_timers'];
      const timerInfo = timerSensor?.attributes?.timers?.[this._config.entity];

      if (timerInfo?.weekday && timerInfo?.execucao) {
        const diaNome = WEEKDAY_MAP[timerInfo.weekday] || timerInfo.weekday;
        time.textContent = `${diaNome}, ${timerInfo.execucao}`;
      } else {
        time.textContent = 'Sem temporizador configurado.';
      }

      if (svgClock) {
        if (timerInfo?.action) {
          const isTrue = timerInfo.estado === 'true';
          if (timerInfo.action.includes('turn_on') && isTrue) svgClock.setAttribute('fill', 'green');
          else if (timerInfo.action.includes('turn_off') && isTrue) svgClock.setAttribute('fill', 'red');
          else if (timerInfo.action.includes('toggle') && isTrue) svgClock.setAttribute('fill', 'orange');
          else if (timerInfo.estado === 'false') svgClock.setAttribute('fill', 'gray');
        } else {
          svgClock.setAttribute('fill', 'none');
        }
      }
    } else if (time) {
      time.textContent = '';
      if (svgClock) svgClock.setAttribute('fill', 'none');
    }
  }

  _showToast(message, backgroundColor = 'var(--primary-color)') {
    const toast = this.shadowRoot.getElementById('toast');
    if (!toast) return;

    toast.textContent = message;
    toast.style.backgroundColor = backgroundColor;
    toast.classList.add('show');

    setTimeout(() => toast.classList.remove('show'), 3000);
  }

  getCardSize() {
    return Math.min(2 + Math.ceil((this._extraEntities.length + this._displayAttributes.length) / 2), 6);
  }
}

if (!customElements.get('generic-card')) {
  customElements.define('generic-card', GenericCard);
}