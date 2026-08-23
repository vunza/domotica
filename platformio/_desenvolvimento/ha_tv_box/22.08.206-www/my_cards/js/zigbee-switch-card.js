import { ContextMenu } from './context-menu.js';
import { switchCardStyles } from '../css/zigbee-switch-card-styles.js';
import { contextMenuStyles } from '../css/context-menu-styles.js';
import { renameModalStyles } from '../css/rename-modal-styles.js';
import { timerModalStyles } from '../css/timer-modal-styles.js';
import { switchCardTemplate } from '../html/zigbee-switch-card-template.js';
import { menuTemplate } from '../html/menu-template.js';
import { renameModalTemplate } from '../html/rename-modal-template.js';
import { timerModalTemplate } from '../html/timer-modal-template.js';

import { initTimerModal, getNextTimer2Execute, obterDadosTimers } from './timer-modal.js';
import { initRenameModal } from './rename-modal.js';
import { createMenuHandler } from './menu-context-modal.js';
import { getToken, ip_e_porta } from './globals.js';

const WEEKDAY_MAP = {
  sun: 'Domingo', mon: 'Segunda-feira', tue: 'Terça-feira',
  wed: 'Quarta-feira', thu: 'Quinta-feira', fri: 'Sexta-feira', sat: 'Sábado'
};

class ZigbeeSwitchCard extends HTMLElement {
  static getStubConfig() {
    return { entity: 'switch.interruptor_inteligente', name: '', show_time: true };
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
    this._extraEntities = [];
    this._lastHistoryFetch = 0;
    this._deviceMac = null;
    this._isResolving = false;

    this._els = {};
    this._onToggleClick = this._toggleEntity.bind(this);

    // No constructor, adicione:
    this._chartVisible = false;

  }

  connectedCallback() {
    if (this.shadowRoot?.querySelector('.menu-trigger')) {
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
    if (!config.entity || (!config.entity.startsWith('switch.') && !config.entity.startsWith('light.') && !config.entity.startsWith('plug.'))) {
      throw new Error('Defina uma entidade válida (ex: switch.interruptor_sala ou light.luz_sala)');
    }
    this._config = { ...config };
    this._customNameProvided = !!config.name;
    this._customName = config.name || null;
    this._extraEntities = config.extra_entities || [];

    if (this._hass && this._config.entity) {
      this._resolveEntities();
    }

    this._render();
    this._updateContent();
  }

  set hass(hass) {
    const oldHass = this._hass;
    this._hass = hass;

    if (this._config.entity) {
      const currentObj = hass.states[this._config.entity];
      if (!currentObj || currentObj.state === 'unavailable' || currentObj.state === 'unknown') {
        this._resolveEntityFromRegistry();
      } else {
        this._stateObj = currentObj;
      }

      this._updateContent();
      this._fetchHistoryIfNeeded();
    }

    if (this._contextMenu) {
      this._contextMenu.setHass(hass);
    }
  }

  // Consulta o Registro de Entidades do HA para encontrar o novo entity_id do mesmo dispositivo
  async _resolveEntityFromRegistry() {
    if (!this._hass || !this._config.entity || this._isResolving) return;
    this._isResolving = true;

    try {
      const registry = await this._hass.callWS({ type: 'config/entity_registry/list' });
      const oldEntry = registry.find(entry => entry.entity_id === this._config.entity);

      if (oldEntry && oldEntry.device_id) {
        const newEntry = registry.find(entry => 
          entry.device_id === oldEntry.device_id &&
          (entry.entity_id.startsWith('switch.') || entry.entity_id.startsWith('light.') || entry.entity_id.startsWith('plug.')) &&
          !entry.disabled_by
        );

        if (newEntry && newEntry.entity_id !== this._config.entity) {
          const oldPrefix = this._config.entity.split('.')[1];
          const newPrefix = newEntry.entity_id.split('.')[1];

          this._config.entity = newEntry.entity_id;
          this._stateObj = this._hass.states[newEntry.entity_id];

          ['power_sensor', 'voltage_sensor', 'current_sensor'].forEach(key => {
            if (this._config[key]) {
              const updatedSensor = this._config[key].replace(oldPrefix, newPrefix);
              if (this._hass.states[updatedSensor]) {
                this._config[key] = updatedSensor;
              }
            }
          });

          this.dispatchEvent(new CustomEvent('config-changed', {
            detail: { config: this._config },
            bubbles: true,
            composed: true
          }));

          this._updateContent();
        }
      }
    } catch (err) {
      console.warn('Não foi possível obter o registro de entidades via WS:', err);
    } finally {
      this._isResolving = false;
    }
  }

  _resolveEntities() {
    if (!this._hass || !this._config.entity) return;

    let mainState = this._hass.states[this._config.entity];

    if (mainState?.attributes) {
      const mac = mainState.attributes.mac_address || mainState.attributes.ieee_address || mainState.attributes.ieee;
      if (mac) this._deviceMac = mac;
    }

    if (!this._deviceMac && this._config.power_sensor) {
      const pState = this._hass.states[this._config.power_sensor];
      if (pState?.attributes) {
        const mac = pState.attributes.mac_address || pState.attributes.ieee_address || pState.attributes.ieee;
        if (mac) this._deviceMac = mac;
      }
    }

    const isUnavailable = !mainState || mainState.state === 'unavailable' || mainState.state === 'unknown';

    if (isUnavailable) {
      let newEntityId = null;

      if (this._deviceMac) {
        newEntityId = Object.keys(this._hass.states).find(id => {
          if (!id.startsWith('switch.') && !id.startsWith('light.') && !id.startsWith('plug.')) return false;
          const attr = this._hass.states[id]?.attributes;
          return attr && (attr.mac_address === this._deviceMac || attr.ieee_address === this._deviceMac || attr.ieee === this._deviceMac);
        });
      }

      if (!newEntityId && this._hass.entities) {
        const oldEntry = this._hass.entities[this._config.entity];
        if (oldEntry?.device_id) {
          const targetDeviceId = oldEntry.device_id;
          newEntityId = Object.keys(this._hass.entities).find(id => {
            const entry = this._hass.entities[id];
            return entry && entry.device_id === targetDeviceId && (id.startsWith('switch.') || id.startsWith('light.')) && !entry.disabled_by;
          });
        }
      }

      if (newEntityId && newEntityId !== this._config.entity) {
        const oldPrefix = this._config.entity.split('.')[1];
        const newPrefix = newEntityId.split('.')[1];

        this._config.entity = newEntityId;
        mainState = this._hass.states[newEntityId];

        ['power_sensor', 'voltage_sensor', 'current_sensor'].forEach(sensorKey => {
          if (this._config[sensorKey]) {
            const candidate = this._config[sensorKey].replace(oldPrefix, newPrefix);
            if (this._hass.states[candidate]) {
              this._config[sensorKey] = candidate;
            }
          }
        });

        this.dispatchEvent(new CustomEvent('config-changed', {
          detail: { config: this._config },
          bubbles: true,
          composed: true
        }));
      }
    }

    this._stateObj = mainState || null;
  }

  _initContextMenu() {
    if (this._contextMenu) {
      this._contextMenu.destroy();
      this._contextMenu = null;
    }

    const menuTrigger = this.shadowRoot.querySelector('.menu-trigger');
    const contextMenuEl = this.shadowRoot.querySelector('.context-menu');
    if (!menuTrigger || !contextMenuEl) return;

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
      <svg class="svg_clk" width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="10"></circle>
        <polyline points="12 6 12 12 16 14"></polyline>
      </svg>
    `;

    const initialTitle = this._config.name || 'Interruptor Zigbee';

    shadow.innerHTML = `
      <style>
        ${switchCardStyles}
        ${contextMenuStyles}
        ${renameModalStyles}
        ${timerModalStyles}
      </style>
      ${switchCardTemplate(initialTitle, clockSvg, menuTemplate)}
      ${renameModalTemplate}      
      ${timerModalTemplate(this._config.name || this._config.entity.split('.')[1] || 'Interruptor Zigbee')}
    `;

    this._els = {
      title: shadow.querySelector('.card-title-content'),
      time: shadow.querySelector('.timestamp'),
      iconSpan: shadow.querySelector('#toggle-icon'),
      cardIcon: shadow.querySelector('#card-icon'),
      svgClock: shadow.querySelector('.svg_clk'),
      extraContainer: shadow.querySelector('.extra-entities'),
      cardHeader: shadow.querySelector('.card-header'),
      menuTrigger: shadow.querySelector('.menu-trigger'),
      lqiBadge: shadow.querySelector('#lqi-badge'),
      stateBadge: shadow.querySelector('#state-badge'),
      metricsContainer: shadow.querySelector('.metrics-container'),
      valPower: shadow.querySelector('#val-power'),
      valVoltage: shadow.querySelector('#val-voltage'),
      valCurrent: shadow.querySelector('#val-current'),
      gridGroup: shadow.querySelector('#grid-lines'),
      pathsGroup: shadow.querySelector('#chart-paths'),
      timeAxis: shadow.querySelector('#time-axis')
    };

    if (this._els.iconSpan) {
      this._els.iconSpan.removeEventListener('click', this._onToggleClick);
      this._els.iconSpan.addEventListener('click', this._onToggleClick);
    }

    const renameModal = initRenameModal(this, shadow);
    this._openRenameModal = renameModal.openModal;

    const timerModal = initTimerModal(this, shadow);
    this._openTimerModal = timerModal.openModal;
    this._closeTimerModal = timerModal.closeModal;

    this._initContextMenu();

    try {
      const token = await getToken('/local/json_files/token_api.json');
      getNextTimer2Execute(token, ip_e_porta);
      await obterDadosTimers(token, ip_e_porta);
    } catch (err) {
      console.error('Erro ao inicializar dados dos timers do interruptor:', err);
    }

    this._updateContent();


    // No método _render(), adicione o listener do botão de toggle do gráfico:
    const chartToggleBtn = shadow.querySelector('.chart-toggle-btn');
    if (chartToggleBtn) {
      chartToggleBtn.addEventListener('click', (e) => {
        e.stopPropagation();
        this._chartVisible = !this._chartVisible;
        const chartBox = shadow.querySelector('.chart-box');
        if (chartBox) {
          chartBox.style.display = this._chartVisible ? 'block' : 'none';
        }
        chartToggleBtn.classList.toggle('active', this._chartVisible);
      });
    }


  }

  _toggleEntity(e) {
    if (e) e.stopPropagation();
    if (!this._hass || !this._config.entity) return;

    const domain = this._config.entity.split('.')[0] || 'switch';
    this._hass.callService(domain, 'toggle', {
      entity_id: this._config.entity
    });
  }

  _updateContent() {
    if (!this._hass) return;

    const {
      title, time, iconSpan, cardIcon, svgClock, extraContainer,
      cardHeader, menuTrigger, lqiBadge, stateBadge, metricsContainer,
      valPower, valVoltage, valCurrent
    } = this._els;

    if (!this._stateObj) {
      if (stateBadge) {
        stateBadge.textContent = 'INDISPONÍVEL';
        stateBadge.classList.remove('on');
      }
      if (title) {
        title.textContent = this._customName || this._config.entity || 'Entidade Ausente';
      }
      if (iconSpan) {
        iconSpan.style.color = 'var(--paper-item-icon-color, #8e8e93)';
      }
      if (cardIcon) {
        cardIcon.setAttribute('icon', 'mdi:toggle-switch-off');
      }
      return;
    }

    const attrs = this._stateObj.attributes || {};

    if (cardHeader) {
      cardHeader.setAttribute('data-device-mac-address', attrs.mac_address || attrs.ieee_address || '');
    }
    if (menuTrigger) {
      menuTrigger.setAttribute('data-id', this._config.entity || '');
    }

    if (lqiBadge) {
      const lqi = attrs.linkquality ?? attrs.lqi ?? null;
      lqiBadge.textContent = lqi !== null ? `LQI: ${lqi}` : 'Zigbee';

      if (lqi !== null) {
        lqiBadge.textContent = `LQI: ${lqi}`;
        lqiBadge.style.display = 'inline-block';
      } else {
        lqiBadge.style.display = 'none';
      }
    }

    const rawState = (this._stateObj.state || '').toLowerCase();
    const isOn = rawState === 'on';

    if (stateBadge) {
      if (rawState === 'unavailable' || rawState === 'unknown') {
        stateBadge.textContent = 'INDISPONÍVEL';
        stateBadge.classList.remove('on');
      } else {
        stateBadge.textContent = isOn ? 'LIGADO' : 'DESLIGADO';
        stateBadge.classList.toggle('on', isOn);
      }
    }

    const hasTelemetry = !!(
      this._config.power_sensor ||
      this._config.voltage_sensor ||
      this._config.current_sensor ||
      attrs.power !== undefined ||
      attrs.active_power !== undefined
    );

    if (metricsContainer) {
      metricsContainer.style.display = hasTelemetry ? 'grid' : 'none';
    }

    if (hasTelemetry) {
      const p = this._config.power_sensor ? this._hass.states[this._config.power_sensor]?.state : (attrs.power ?? attrs.active_power);
      const v = this._config.voltage_sensor ? this._hass.states[this._config.voltage_sensor]?.state : attrs.voltage;
      const c = this._config.current_sensor ? this._hass.states[this._config.current_sensor]?.state : attrs.current;

      if (valPower) valPower.textContent = p !== undefined && p !== null && !isNaN(p) ? `${parseFloat(p).toFixed(1)} W` : '-- W';
      if (valVoltage) valVoltage.textContent = v !== undefined && v !== null && !isNaN(v) ? `${parseFloat(v).toFixed(1)} V` : '-- V';
      if (valCurrent) valCurrent.textContent = c !== undefined && c !== null && !isNaN(c) ? `${parseFloat(c).toFixed(2)} A` : '-- A';
    }

    if (title) {
      title.textContent = this._customNameProvided && this._customName
        ? this._customName
        : attrs.friendly_name || this._config.entity.split('.')[1];
    }

    if (iconSpan) {
      iconSpan.style.color = isOn
        ? 'var(--paper-item-icon-active-color, #00e676)'
        : 'var(--paper-item-icon-color, #8e8e93)';
    }

    if (cardIcon) {
      cardIcon.setAttribute('icon', isOn ? 'mdi:toggle-switch' : 'mdi:toggle-switch-off');
    }

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

      extraContainer.innerHTML = hasContent ? html : '';
    }

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

  async _fetchHistoryIfNeeded() {
    if (!this._hass || !this._config.entity) return;

    const now = Date.now();
    if (now - this._lastHistoryFetch < 60000 && this._lastHistoryFetch !== 0) return;

    const sensors = [
      this._config.power_sensor,
      this._config.voltage_sensor,
      this._config.current_sensor,
      !this._config.power_sensor ? this._config.entity : null
    ].filter(Boolean);

    this._lastHistoryFetch = now;
    const startTime = new Date(now - 24 * 3600 * 1000).toISOString();

    try {
      const history = await this._hass.callApi(
        'GET',
        `history/period/${startTime}?filter_entity_id=${sensors.join(',')}`
      );
      this._renderGraph(history, sensors);
    } catch (err) {
      console.error('Erro ao obter histórico do interruptor:', err);
      this._lastHistoryFetch = 0;
    }
  }

  _renderGraph(historyData, sensors) {
    if (!historyData || !Array.isArray(historyData) || historyData.length === 0) return;

    const width = 500;
    const height = 100;
    const padding = 10;

    let gridHtml = '';
    [20, 50, 80].forEach(y => {
      gridHtml += `<line x1="0" y1="${y}" x2="${width}" y2="${y}" class="grid-line" />`;
    });
    if (this._els.gridGroup) this._els.gridGroup.innerHTML = gridHtml;

    const hasTelemetry = !!(this._config.power_sensor || this._config.voltage_sensor || this._config.current_sensor);

    if (!hasTelemetry) {
      let switchHistory = historyData.find(h =>
        Array.isArray(h) && h.length > 0 && (h[0].entity_id === this._config.entity || h.some(i => i.entity_id === this._config.entity))
      ) || historyData[0];

      if (!switchHistory || switchHistory.length < 2) return;

      const points = switchHistory
        .map(entry => ({
          x: new Date(entry.last_updated || entry.last_changed).getTime(),
          y: entry.state === 'on' ? 1 : 0
        }))
        .filter(p => !isNaN(p.x));

      if (points.length < 2) return;

      const minX = points[0].x;
      const maxX = points[points.length - 1].x;
      const rangeX = (maxX - minX) || 1;

      const normalized = points.map(p => ({
        x: ((p.x - minX) / rangeX) * width,
        y: p.y === 1 ? padding + 10 : height - padding - 10
      }));

      let d = `M ${normalized[0].x.toFixed(1)} ${normalized[0].y.toFixed(1)}`;
      for (let i = 1; i < normalized.length; i++) {
        const prev = normalized[i - 1];
        const curr = normalized[i];
        d += ` L ${curr.x.toFixed(1)} ${prev.y.toFixed(1)} L ${curr.x.toFixed(1)} ${curr.y.toFixed(1)}`;
      }

      const areaPath = `${d} L ${normalized[normalized.length - 1].x.toFixed(1)} ${height} L ${normalized[0].x.toFixed(1)} ${height} Z`;

      let pathsHtml = `<path d="${areaPath}" fill="url(#grad-power-sw)" opacity="0.3" />`;
      pathsHtml += `<path d="${d}" fill="none" stroke="#00e676" stroke-width="2.5" />`;

      if (this._els.pathsGroup) this._els.pathsGroup.innerHTML = pathsHtml;

      let timeLabels = [];
      const step = Math.max(1, Math.floor(points.length / 4));
      for (let i = 0; i < points.length; i += step) {
        const date = new Date(points[i].x);
        timeLabels.push(date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }));
      }
      if (timeLabels.length > 0 && this._els.timeAxis) {
        this._els.timeAxis.innerHTML = timeLabels.map(t => `<span>${t}</span>`).join('');
      }
      return;
    }

    const seriesConfig = [
      { entity: this._config.power_sensor, color: '#00e676', grad: 'url(#grad-power-sw)' },
      { entity: this._config.voltage_sensor, color: '#ff9100', grad: 'url(#grad-voltage-sw)' },
      { entity: this._config.current_sensor, color: '#29b6f6', grad: 'url(#grad-current-sw)' }
    ].filter(s => !!s.entity);

    let pathsHtml = '';
    let timeLabels = [];

    seriesConfig.forEach(series => {
      let entityHistory = historyData.find(h =>
        Array.isArray(h) && h.length > 0 && (h[0].entity_id === series.entity || h.some(item => item.entity_id === series.entity))
      );

      if (!entityHistory) {
        const idx = sensors.indexOf(series.entity);
        if (idx !== -1 && historyData[idx]) entityHistory = historyData[idx];
      }

      if (!entityHistory || entityHistory.length === 0) return;

      const points = entityHistory
        .map(entry => ({
          x: new Date(entry.last_updated || entry.last_changed).getTime(),
          y: parseFloat(entry.state)
        }))
        .filter(p => !isNaN(p.x) && !isNaN(p.y));

      if (points.length < 2) return;

      const minX = points[0].x;
      const maxX = points[points.length - 1].x;
      const minY = Math.min(...points.map(p => p.y));
      const maxY = Math.max(...points.map(p => p.y));

      const rangeX = (maxX - minX) || 1;
      const rangeY = (maxY - minY) || 1;

      const normalized = points.map(p => ({
        x: ((p.x - minX) / rangeX) * width,
        y: height - padding - ((p.y - minY) / rangeY) * (height - 2 * padding)
      }));

      let d = `M ${normalized[0].x.toFixed(1)} ${normalized[0].y.toFixed(1)}`;
      for (let i = 1; i < normalized.length; i++) {
        const prev = normalized[i - 1];
        const curr = normalized[i];
        const cx = ((prev.x + curr.x) / 2).toFixed(1);
        d += ` C ${cx} ${prev.y.toFixed(1)}, ${cx} ${curr.y.toFixed(1)}, ${curr.x.toFixed(1)} ${curr.y.toFixed(1)}`;
      }

      const areaPath = `${d} L ${normalized[normalized.length - 1].x.toFixed(1)} ${height} L ${normalized[0].x.toFixed(1)} ${height} Z`;

      pathsHtml += `<path d="${areaPath}" fill="${series.grad}" />`;
      pathsHtml += `<path d="${d}" fill="none" stroke="${series.color}" stroke-width="2.5" stroke-linecap="round" />`;

      if (timeLabels.length === 0) {
        const step = Math.max(1, Math.floor(points.length / 4));
        for (let i = 0; i < points.length; i += step) {
          const date = new Date(points[i].x);
          timeLabels.push(date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }));
        }
      }
    });

    if (this._els.pathsGroup) this._els.pathsGroup.innerHTML = pathsHtml;

    if (timeLabels.length > 0 && this._els.timeAxis) {
      this._els.timeAxis.innerHTML = timeLabels.map(t => `<span>${t}</span>`).join('');
    }
  }

  getCardSize() {
    return 3;
  }
}

if (!customElements.get('zigbee-switch-card')) {
  customElements.define('zigbee-switch-card', ZigbeeSwitchCard);
}