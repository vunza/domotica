import { ContextMenu } from './context-menu.js';
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

const CLOCK_SVG = `
  <svg class="svg_clk" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
    <circle cx="12" cy="12" r="10"></circle>
    <polyline points="12 6 12 12 16 14"></polyline>
  </svg>
`;

const MAPA_DIAS = {
  sun: 'Domingo', mon: 'Segunda-feira', tue: 'Terça-feira',
  wed: 'Quarta-feira', thu: 'Quinta-feira', fri: 'Sexta-feira', sat: 'Sábado'
};

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
    this._els = {};
    this._minTemp = 16;
    this._maxTemp = 30;
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
    if (!config.entity) throw new Error('Defina a entidade de clima (climate)');
    this._config = config;
    this._customName = config.name || null;
    this._tempSensorEntity = config.temp_sensor || 'sensor.current_ac_temperature';
    this._extraEntities = config.extra_entities || [];
    this._isAacCtrlr = config.is_a_ac_controller ?? false; 

    this._render();
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

    shadow.innerHTML = `
      <style>        
        ${contextMenuStyles}
        ${renameModalStyles}
        ${timerModalStyles}
        ${acCardStyles}       
      </style>
      
      ${acCardTemplate(initialTitle, CLOCK_SVG, menuTemplate)}          
      ${renameModalTemplate}      
      ${timerModalTemplate(initialTitle)}
    `;

    this._cacheElements();

    if (this._els.iconSpan) {
      this._els.iconSpan.addEventListener('click', (e) => this._toggleEntity(e));  
    }
    
    this._setupAcEvents();

    const renameModal = initRenameModal(this, shadow);
    this._openRenameModal = renameModal.openModal;

    const timerModal = initTimerModal(this, shadow);
    this._openTimerModal = timerModal.openModal;

    this._initContextMenu();

    try {
      const token = await getToken('/local/json_files/token_api.json');
      getNextTimer2Execute(token, ip_e_porta);

      const timers = await obterDadosTimers(token, ip_e_porta);
      if (timers) {
        Object.keys(timers).forEach(() => {});                    
      }
    } catch (err) {
      console.warn('Erro ao carregar dados dos timers:', err);
    }

    this._updateContent();
  }

  _cacheElements() {
    const shadow = this.shadowRoot;
    this.content = shadow.querySelector('#ac-core-content');
    
    this._els = {
      titleElem: shadow.querySelector('.card-title-content'),
      cardTitle: shadow.querySelector('#card-title'),
      timeElem: shadow.querySelector('.timestamp'),
      iconSpan: shadow.querySelector('#toggle-icon'),
      cardIcon: shadow.querySelector('#card-icon'),
      svgClock: shadow.querySelector('.svg_clk'),
      tempValElem: shadow.querySelector('#temp-val'),
      modeTextElem: shadow.querySelector('#mode-text'),
      pillModeValElem: shadow.querySelector('#pill-mode-val'),
      pillFanValElem: shadow.querySelector('#pill-fan-val'),
      activeCircle: shadow.querySelector('.active'),
      handleCircle: shadow.querySelector('.handle'),
      cardHeader: shadow.querySelector('.card-header'),
      menuTrigger: shadow.querySelector('.menu-trigger'),
      btnMinus: shadow.querySelector('#btn-minus'),
      btnPlus: shadow.querySelector('#btn-plus'),
      toggleArcBtn: shadow.querySelector('#btn-toggle-arc'),
      dialContainer: shadow.querySelector('.dial-container'),
      dialSvg: shadow.querySelector('.dial'),
      controls: shadow.querySelector('.controls'),
      lqiBadge: shadow.querySelector('#lqi-badge'),
      stateBadge: shadow.querySelector('#state-badge'),
      arcSection: shadow.querySelector('.arc-collapsible-section'),
      metricsCompact: shadow.querySelector('#ac-metrics-compact'),
      compactTempVal: shadow.querySelector('#compact-temp-val'),
      compactModeVal: shadow.querySelector('#compact-mode-val'),
      compactFanVal: shadow.querySelector('#compact-fan-val'),
      arcModeVal: shadow.querySelector('#arc-mode-val'),
      arcFanVal: shadow.querySelector('#arc-fan-val'),
    };
  }

  _setupAcEvents() {
    const adjustTemp = (amount) => {
      if (!this._hass || !this._config.entity) return;

      const entityId = this._config.entity;
      const domain = entityId.split('.')[0];
      const stateObj = this._stateObj || this._hass.states[entityId];

      if (!stateObj) return;

      const currentTemp = stateObj.attributes.temperature ?? 25;
      const newTemp = Math.max(this._minTemp, Math.min(this._maxTemp, Number(currentTemp) + amount));

      this._sendTemperature(newTemp);
    };

    this._els.btnMinus?.addEventListener('click', () => adjustTemp(-1));
    this._els.btnPlus?.addEventListener('click', () => adjustTemp(1));

    const { toggleArcBtn, arcSection, metricsCompact } = this._els;
    if (toggleArcBtn && arcSection && metricsCompact) {
      toggleArcBtn.addEventListener('click', (e) => {
        e.stopPropagation();
        const isHidden = arcSection.style.display === 'none';
        
        arcSection.style.display = isHidden ? 'flex' : 'none';
        metricsCompact.style.display = isHidden ? 'none' : 'grid';
        
        toggleArcBtn.classList.toggle('active', isHidden);
      });
    }

    // Interatividade por clique e arrastar no Arco SVG
    const svg = this._els.dialSvg;
    if (!svg) return;

    let isDragging = false;

    const calculateTempFromCoord = (e) => {
      const rect = svg.getBoundingClientRect();
      const center = {
        x: rect.left + rect.width / 2,
        y: rect.top + rect.height / 2
      };

      const clientX = e.touches ? e.touches[0].clientX : e.clientX;
      const clientY = e.touches ? e.touches[0].clientY : e.clientY;

      const radians = Math.atan2(clientY - center.y, clientX - center.x);
      let degrees = radians * (180 / Math.PI);
      degrees = (degrees + 360) % 360;

      let normalizedAngle = degrees - 135;
      if (normalizedAngle < 0) normalizedAngle += 360;

      if (normalizedAngle > 270 && normalizedAngle < 315) normalizedAngle = 270;
      if (normalizedAngle >= 315 && normalizedAngle < 135) normalizedAngle = 0;

      const percentage = Math.min(Math.max(normalizedAngle / 270, 0), 1);
      const newTemp = Math.round(this._minTemp + percentage * (this._maxTemp - this._minTemp));
      return newTemp;
    };

    svg.addEventListener('mousedown', (e) => {
      isDragging = true;
      const temp = calculateTempFromCoord(e);
      this._updateArcVisualDirectly(temp);
    });

    window.addEventListener('mousemove', (e) => {
      if (!isDragging) return;
      const temp = calculateTempFromCoord(e);
      this._updateArcVisualDirectly(temp);
    });

    window.addEventListener('mouseup', (e) => {
      if (isDragging) {
        isDragging = false;
        const temp = calculateTempFromCoord(e);
        this._sendTemperature(temp);
      }
    });
  }

  _sendTemperature(temp) {
    if (!this._hass || !this._config.entity) return;
    const entityId = this._config.entity;
    const domain = entityId.split('.')[0];

    if (domain === 'climate') {
      this._hass.callService('climate', 'set_temperature', { entity_id: entityId, temperature: temp });
    } else if (domain === 'input_number') {
      this._hass.callService('input_number', 'set_value', { entity_id: entityId, value: temp });
    }
  }


 _updateArcVisualDirectly(temp) {
    const safeTemp = Math.max(this._minTemp, Math.min(this._maxTemp, temp));
    
    if (this._els.tempValElem) this._els.tempValElem.textContent = safeTemp;
    if (this._els.compactTempVal) this._els.compactTempVal.textContent = safeTemp;

    const totalDash = 2 * Math.PI * 85;
    const progress = (safeTemp - this._minTemp) / (this._maxTemp - this._minTemp);
    const { activeCircle, handleCircle } = this._els;

    if (activeCircle && handleCircle) {
      const arcLength = totalDash * 0.72;
      activeCircle.style.strokeDasharray = `${arcLength} ${totalDash}`;
      activeCircle.style.strokeDashoffset = arcLength - (arcLength * progress);

      // Interpolação de cor entre o Azul (#29b6f6 -> 41, 182, 246) e o Laranja (#ff9100 -> 255, 145, 0)
      const colorMin = { r: 41, g: 182, b: 246 };   // Frio (16°C)
      const colorMax = { r: 255, g: 145, b: 0 };    // Quente (30°C)

      const r = Math.round(colorMin.r + (colorMax.r - colorMin.r) * progress);
      const g = Math.round(colorMin.g + (colorMax.g - colorMin.g) * progress);
      const b = Math.round(colorMin.b + (colorMax.b - colorMin.b) * progress);
      const dynamicColor = `rgb(${r}, ${g}, ${b})`;

      // Aplica a cor dinâmica no traço do arco e na borda/preenchimento da bolinha
      activeCircle.style.stroke = dynamicColor;
      handleCircle.style.stroke = dynamicColor;

      const rad = ((135 + (progress * 270)) * Math.PI) / 180;
      handleCircle.setAttribute('cx', 100 + 85 * Math.cos(rad));
      handleCircle.setAttribute('cy', 100 + 85 * Math.sin(rad));
    }
  }


  _openMoreInfo() {
    this.dispatchEvent(new CustomEvent('hass-more-info', {
      bubbles: true,
      composed: true,
      detail: { entityId: this._config.entity }
    }));
  }

  _updateContent() {
    if (!this._hass || !this._stateObj || !this.shadowRoot) return;

    const { 
      titleElem, cardTitle, timeElem, iconSpan, cardIcon, svgClock, 
      tempValElem, modeTextElem, pillModeValElem, pillFanValElem, 
      cardHeader, menuTrigger, stateBadge, lqiBadge 
    } = this._els;

    const attrs = this._stateObj.attributes || {};

    if (lqiBadge) {
      const lqi = attrs.linkquality ?? attrs.lqi ?? null;
      if (lqi !== null) {
        lqiBadge.textContent = `LQI: ${lqi}`;
        lqiBadge.style.display = 'inline-block';
      } else {
        lqiBadge.style.display = 'none';
      }
    }

    const targetTemp = attrs.temperature || 25;
    const hvacMode = this._stateObj.state || 'off';
    const fanMode = attrs.fan_mode || 'auto';
    const displayMode = hvacMode === 'cool' ? 'Cool' : hvacMode;

    if (this._customName && cardTitle) cardTitle.textContent = this._customName;
    if (tempValElem) tempValElem.textContent = targetTemp;
    if (modeTextElem) modeTextElem.textContent = displayMode;
    if (pillModeValElem) pillModeValElem.textContent = displayMode;
    if (pillFanValElem) pillFanValElem.textContent = fanMode;
    
    if (this._els.compactModeVal) this._els.compactModeVal.textContent = displayMode;
    if (this._els.compactTempVal) this._els.compactTempVal.textContent = targetTemp;
    if (this._els.compactFanVal) this._els.compactFanVal.textContent = fanMode;

    if (this._els.arcModeVal) this._els.arcModeVal.textContent = displayMode;
    if (this._els.arcFanVal) this._els.arcFanVal.textContent = fanMode;

    // Atualiza o arco com base na temperatura alvo atualizada do Home Assistant
    this._updateArcVisualDirectly(targetTemp);

    if (cardHeader) {
      cardHeader.setAttribute('data-device-mac-address', attrs.mac_address || '');
    }

    if (menuTrigger) {
      menuTrigger.setAttribute('data-id', this._config.entity || '');
    }

    if (titleElem) {
      titleElem.textContent = this._customName || attrs?.friendly_name || this._config.entity.split('.')[1] || 'Ar Condicionado';
    }

    // Processamento de Temporizadores
    if (this._config.show_time && timeElem) {
      const timerSensor = this._hass.states['sensor.info_dos_timers'];
      const timerInfo = timerSensor?.attributes?.timers?.[this._config.entity];

      if (timerInfo?.weekday && timerInfo?.execucao) {
        const diaNome = MAPA_DIAS[timerInfo.weekday] || timerInfo.weekday;
        timeElem.textContent = `${diaNome}, ${timerInfo.execucao}`;
      } else {
        timeElem.textContent = 'Sem temporizador configurado.';
      }

      if (svgClock) {
        if (timerInfo?.action && timerInfo.estado === 'true') {
          const colorMap = { turn_on: 'green', turn_off: 'red', toggle: 'orange' };
          const matchAction = Object.keys(colorMap).find(act => timerInfo.action.includes(act));
          svgClock.setAttribute('fill', matchAction ? colorMap[matchAction] : 'none');
        } else if (timerInfo?.estado === 'false') {
          svgClock.setAttribute('fill', 'gray');
        } else {
          svgClock.setAttribute('fill', 'none');
        }
      }
    } else if (timeElem) {
      timeElem.textContent = '';
      if (svgClock) svgClock.setAttribute('fill', 'none');
    }

    const rawState = (this._stateObj.state || '').toLowerCase();
    const isOn = rawState !== 'off' && rawState !== 'unavailable' && rawState !== 'unknown';

    if (stateBadge) {
      if (rawState === 'unavailable' || rawState === 'unknown') {
        stateBadge.textContent = 'INDISPONÍVEL';
        stateBadge.classList.remove('on');
      } else {
        stateBadge.textContent = isOn ? 'LIGADO' : 'DESLIGADO';
        stateBadge.classList.toggle('on', isOn);
      }
    }

    if (iconSpan) {
      iconSpan.style.color = isOn 
        ? 'var(--paper-item-icon-active-color, #00e676)' 
        : 'var(--paper-item-icon-color, #8e8e93)';
    }

    if (cardIcon) {
      const iconName = isOn ? 'mdi:snowflake' : 'mdi:power';
      cardIcon.setAttribute('icon', iconName);
    }
  }

  _toggleEntity(e) {
    if (e) e.stopPropagation();
    if (!this._hass || !this._config.entity) return;

    const domain = this._config.entity.split('.')[0];
    const rawState = (this._stateObj?.state || '').toLowerCase();
    const isOn = rawState !== 'off' && rawState !== 'unavailable' && rawState !== 'unknown';

    if (domain === 'climate') {
      const service = isOn ? 'turn_off' : 'turn_on';
      this._hass.callService('climate', service, { entity_id: this._config.entity });
    } else {
      this._hass.callService(domain, 'toggle', { entity_id: this._config.entity });
    }
  }    

  getCardSize() {
    return 5;
  }
}

if (!customElements.get('ac-card')) {
  customElements.define('ac-card', AcCard);
}