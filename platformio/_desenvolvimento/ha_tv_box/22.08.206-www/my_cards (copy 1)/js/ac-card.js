import { ContextMenu } from './context-menu.js';
//import { cardStyles } from '../css/generic-card-styles.js';
import { contextMenuStyles } from '../css/context-menu-styles.js';
import { acCardStyles } from '../css/ac-card-styles.js';
import { renameModalStyles } from '../css/rename-modal-styles.js';
import { timerModalStyles } from '../css/timer-modal-styles.js';
import { menuTemplate } from '../html/menu-template.js';
import { acCardTemplate } from '../html/ac-card-template.js';
import { renameModalTemplate } from '../html/rename-modal-template.js';
import { timerModalTemplate } from '../html/timer-modal-template.js';

import { initTimerModal, getNextTimer2Execute, obterDadosTimers } from './timer-modal.js';
import { initRenameModal } from './rename-modal.js';
import { createMenuHandler } from './menu-context-modal.js';
import { getToken, ip_e_porta } from './globals.js';

/**
 * Card personalizado estruturado para controle de Ar Condicionado (AC)
 * baseado no seu template modular com menu de contexto e modais.
 */
class AcCard extends HTMLElement {
  static getStubConfig() {
    return { entity: 'climate.ar_condicionado', temp_sensor: 'sensor.current_ac_temperature', name: '' };
  }

  constructor() {
    super();
    this.attachShadow({ mode: 'open' });
    this._config = {};
    this._hass = null;
    this._stateObj = null;
    this._contextMenu = null;
    this._customName = null;
    this.content = null;
    this._isAacCtrlr = null;
  }

  /**
   * Executado sempre que o elemento é inserido no DOM (ex: ao navegar entre views)
   */
  connectedCallback() {
    if (!this._contextMenu && this.shadowRoot && this.shadowRoot.querySelector('.menu-trigger')) {
      this._initContextMenu();
    }
  }

  /**
   * Executado quando o card é removido do DOM (ex: ao trocar de aba)
   */
  disconnectedCallback() {
    if (this._contextMenu) {
      this._contextMenu.destroy();
      this._contextMenu = null;
    }
  }

  setConfig(config) {
    if (!config.entity) throw new Error('Defina a entidade de clima (climate)');
    this._config = config;
    this._customName = config.name || null;
    this._tempSensorEntity = config.temp_sensor || 'sensor.current_ac_temperature';
    this._extraEntities = config.extra_entities || [];

    this._isAacCtrlr = config.is_a_ac_controller ?? false; 

    this._render();
    this._updateContent();
  }

  set hass(hass) {
    this._hass = hass;
    if (this._config.entity) {
      this._stateObj = hass.states[this._config.entity];
      this._updateContent();
    }
    if (this._contextMenu) {
      this._contextMenu.setHass(hass);
    }
  }

  /**
   * Inicializa o Menu de Contexto de forma segura
   */
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
    const initialTitle = this._config.name || (this._config.entity ? this._config.entity.split('.')[1].replace('_', ' ') : 'Ar Condicionado');
    const clockSvg = `
      <svg class="svg_clk" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="10"></circle>
        <polyline points="12 6 12 12 16 14"></polyline>
      </svg>
    `;
   
    shadow.innerHTML = `
      <style>        
        ${contextMenuStyles}
        ${renameModalStyles}
        ${timerModalStyles}
        ${acCardStyles}       
      </style>
      
      ${acCardTemplate(initialTitle, clockSvg, menuTemplate)}          
      ${renameModalTemplate}      
      ${timerModalTemplate(initialTitle)}
      ${timerModalTemplate(this._config.name || this._config.entity.split('.')[1] || 'Dispositivo')}
    `;

    this.content = shadow.querySelector('#ac-core-content');

    // Configura o listener de clique para alternar o estado da entidade
    const iconSpan = shadow.querySelector('#toggle-icon');
    if (iconSpan) {
      iconSpan.addEventListener('click', (e) => this._toggleEntity(e));  
    }
    
    // Configura os eventos dos botões de controle do AC
    this._setupAcEvents();

    // Inicializa os modais
    const renameModal = initRenameModal(this, shadow);
    this._openRenameModal = renameModal.openModal;

    const timerModal = initTimerModal(this, shadow);
    this._openTimerModal = timerModal.openModal;

    // Inicializa o menu contextual
    this._initContextMenu();

    // Obter Token de acesso ao HA
    const api_token = '/local/json_files/token_api.json';
    const token = await getToken(api_token);

    // Próximos timers
    getNextTimer2Execute(token, ip_e_porta);

    this._updateContent();

    // Obter dados dos Timers atualizados
    obterDadosTimers(token, ip_e_porta).then(timers => {
      if (timers) {
        Object.keys(timers).forEach(entityId => {  
          // Lógica dos timers
        });                    
      }
    });

    this._updateContent();
  }

  /**
   * Configura os eventos de clique para os botões de controle do AC
   */
  _setupAcEvents() {
    const shadow = this.shadowRoot;

    // Função auxiliar para ajuste seguro de temperatura
    const adjustTemp = (amount) => {
      if (!this._hass || !this._config.entity) return;

      const entityId = this._config.entity;
      const domain = entityId.split('.')[0];
      const stateObj = this._stateObj || this._hass.states[entityId];

      if (!stateObj) return;

      const step = stateObj.attributes.target_temp_step || 1;

      let currentTemp = stateObj.attributes.temperature;
      if (currentTemp === undefined || currentTemp === null) {
        currentTemp = stateObj.attributes.current_temperature ?? 25;
      }

      const newTemp = Number(currentTemp) + (amount * step);

      if (domain === 'climate') {
        this._hass.callService('climate', 'set_temperature', {
          entity_id: entityId,
          temperature: newTemp
        });
      } else if (domain === 'input_number') {
        this._hass.callService('input_number', 'set_value', {
          entity_id: entityId,
          value: newTemp
        });
      }
    };

    const btnMinus = shadow.querySelector('#btn-minus');
    if (btnMinus) {
      btnMinus.addEventListener('click', () => adjustTemp(-1));
    }

    const btnPlus = shadow.querySelector('#btn-plus');
    if (btnPlus) {
      btnPlus.addEventListener('click', () => adjustTemp(1));
    }

    // Controle de alternância do Arco e Ícones SVG
    const toggleArcBtn = shadow.querySelector('#btn-toggle-arc');
    const dialContainer = shadow.querySelector('.dial-container');
    const controls = shadow.querySelector('.controls');

    if (toggleArcBtn && dialContainer && controls) {
      // Ícone normal (Olho Aberto) -> Verde
      const svgEyeOpen = `
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--success-color, #4caf50)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path>
          <circle cx="12" cy="12" r="3"></circle>
        </svg>
      `;

      // Ícone com risco (Olho Cortado) -> Vermelho
      const svgEyeOff = `
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--error-color, #f44336)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19m-6.72-1.07a3 3 0 1 1-4.24-4.24"></path>
          <line x1="1" y1="1" x2="23" y2="23"></line>
        </svg>
      `;

      // Define estado inicial do ícone do botão
      toggleArcBtn.innerHTML = svgEyeOff;
      toggleArcBtn.title = 'Ocultar Arco';

      toggleArcBtn.addEventListener('click', () => {
        if (dialContainer.style.display === 'none') {
          dialContainer.style.display = 'block';
          toggleArcBtn.innerHTML = svgEyeOff;
          toggleArcBtn.title = 'Ocultar Arco';
          controls.style.margin = '-20px 0 15px 0';
        } else {
          dialContainer.style.display = 'none';
          toggleArcBtn.innerHTML = svgEyeOpen;
          toggleArcBtn.title = 'Exibir Arco';
          controls.style.margin = '1px 0 15px 0';
        }
      });
    }
  }

  /**
   * Abre o modal de informações detalhadas do Home Assistant para a entidade configurada.
   */
  _openMoreInfo() {
    const event = new Event('hass-more-info', { bubbles: true, composed: true });
    event.detail = { entityId: this._config.entity };
    this.dispatchEvent(event);
  }

  /**
   * Vincula dinamicamente a geometria do arco e os textos às leituras do Home Assistant
   */
  _updateContent() {
    if (!this._hass || !this._stateObj) return;

    const shadow = this.shadowRoot;
    if (!shadow) return;

    const titleElem = shadow.querySelector('.card-title-content');
    const timeElem = shadow.querySelector('.timestamp');
    const iconSpan = shadow.querySelector('#toggle-icon');
    const cardIcon = shadow.querySelector('#card-icon');
    const clockDiv = shadow.querySelector('.div_img_clock');
    const svgClock = clockDiv ? clockDiv.querySelector('.svg_clk') : null;

    if (this._isAacCtrlr && cardIcon) {
      cardIcon.setAttribute('icon', 'mdi:air-conditioner');      
    }

    const tempSensor = this._hass.states[this._tempSensorEntity];
    const currentTemp = tempSensor ? parseFloat(tempSensor.state) : (this._stateObj.attributes.temperature || 25);
    const hvacMode = this._stateObj.state || 'off';
    const fanMode = this._stateObj.attributes.fan_mode || 'auto';

    if (this._customName && shadow.querySelector('#card-title')) {
      shadow.querySelector('#card-title').textContent = this._customName;
    }

    const tempValElem = shadow.querySelector('#temp-val');
    if (tempValElem) tempValElem.textContent = isNaN(currentTemp) ? '--' : currentTemp;

    const modeTextElem = shadow.querySelector('#mode-text');
    if (modeTextElem) modeTextElem.textContent = hvacMode === 'cool' ? 'Cool' : hvacMode;

    const pillModeValElem = shadow.querySelector('#pill-mode-val');
    if (pillModeValElem) pillModeValElem.textContent = hvacMode === 'cool' ? 'Cool' : hvacMode;

    const pillFanValElem = shadow.querySelector('#pill-fan-val');
    if (pillFanValElem) pillFanValElem.textContent = fanMode;

    // Lógica matemática do arco SVG (Escala 16°C a 30°C)
    const min = 16;
    const max = 30;
    const totalDash = 2 * Math.PI * 85;
    const safeTemp = isNaN(currentTemp) ? 25 : Math.max(min, Math.min(max, currentTemp));
    const progress = (safeTemp - min) / (max - min);
    const activeCircle = shadow.querySelector('.active');
    const handleCircle = shadow.querySelector('.handle');

    if (activeCircle && handleCircle) {
      const arcLength = totalDash * 0.72;
      activeCircle.style.strokeDasharray = `${arcLength} ${totalDash}`;
      activeCircle.style.strokeDashoffset = arcLength - (arcLength * progress);

      const angle = 135 + (progress * 270);
      const rad = (angle * Math.PI) / 180;
      const cx = 100 + 85 * Math.cos(rad);
      const cy = 100 + 85 * Math.sin(rad);
      handleCircle.setAttribute('cx', cx);
      handleCircle.setAttribute('cy', cy);
    }

    // Atributo MAC no header
    const cardHeader = shadow.querySelector('.card-header');
    if (cardHeader) {
      const macAddress = this._stateObj?.attributes?.mac_address; 
      cardHeader.setAttribute('data-device-mac-address', macAddress || null);
    }

    // Atributo data-id no botão do menu
    const menuTrigger = shadow.querySelector('.menu-trigger');
    if (menuTrigger) {
      menuTrigger.setAttribute('data-id', this._config.entity || '');
    }

    // Título do Card
    if (titleElem) {
      if (this._customNameProvided && this._customName) {
        titleElem.textContent = this._customName;
      } else {
        const friendlyName = this._stateObj?.attributes.friendly_name;
        titleElem.textContent = friendlyName || this._config.entity.split('.')[1] || 'Ar Condicionado';
      }
    }

    // Cor do ícone principal
    if (iconSpan) {
      if (this._stateObj.state === 'on' || this._stateObj.state === 'cool' || this._stateObj.state === 'heat') {
        iconSpan.style.color = 'var(--paper-item-icon-active-color, #fdd835)';
      } else {
        iconSpan.style.color = 'var(--paper-item-icon-color, #44739e)';
      }
    }

    // Informações de temporizadores
    if (this._config.show_time && timeElem) {
      const timerSensor = this._hass.states['sensor.info_dos_timers'];
      let timerInfo = null;     

      if (timerSensor && timerSensor.attributes && timerSensor.attributes.timers) {
        const timers = timerSensor.attributes.timers;
        timerInfo = timers[this._config.entity];
      }

      if (timerInfo && timerInfo.weekday && timerInfo.execucao) {
        const mapaDias = {
          sun: 'Domingo',
          mon: 'Segunda-feira',
          tue: 'Terça-feira',
          wed: 'Quarta-feira',
          thu: 'Quinta-feira',
          fri: 'Sexta-feira',
          sat: 'Sábado'
        };
        const diaNome = mapaDias[timerInfo.weekday] || timerInfo.weekday;
        timeElem.textContent = `${diaNome}, ${timerInfo.execucao}`;
      } else {
        timeElem.textContent = 'Sem temporizador configurado.';
      }

      if (svgClock) {
        if (timerInfo && timerInfo.action) {
          if (timerInfo.action.includes('turn_on') && timerInfo.estado === 'true') {
            svgClock.setAttribute('fill', 'green');
          } else if (timerInfo.action.includes('turn_off') && timerInfo.estado === 'true') {
            svgClock.setAttribute('fill', 'red');
          } else if (timerInfo.action.includes('toggle') && timerInfo.estado === 'true') {
            svgClock.setAttribute('fill', 'orange');
          } else if (timerInfo.estado === 'false') {
            svgClock.setAttribute('fill', 'gray');
          }
        } else {
          svgClock.setAttribute('fill', 'none');
        }
      }
    } else if (timeElem) {
      timeElem.textContent = '';
      if (svgClock) svgClock.setAttribute('fill', 'none');
    }
  }

  /**
   * Alterna o estado da entidade (liga/desliga)
   */
  _toggleEntity(e) {
    if (e) e.stopPropagation();
    if (!this._hass || !this._stateObj) return;

    /*
    const domain = this._config.entity.split('.')[0];
    this._hass.callService(domain, 'toggle', {
      entity_id: this._config.entity
    }).catch(() => {
      this._hass.callService('homeassistant', 'toggle', {
        entity_id: this._config.entity
      });
    });
    */
  }

  getCardSize() {
    return 5;
  }
}

//customElements.define('ac-card', AcCard);
if (!customElements.get('ac-card')) {
  customElements.define('ac-card', AcCard);
}