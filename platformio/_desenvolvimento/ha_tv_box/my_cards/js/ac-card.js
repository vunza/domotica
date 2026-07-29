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
    iconSpan.addEventListener('click', (e) => this._toggleEntity(e));    

    // Inicializa os modais (em arquivos separados)
    const renameModal = initRenameModal(this, shadow);
    this._openRenameModal = renameModal.openModal;

    const timerModal = initTimerModal(this, shadow);
    this._openTimerModal = timerModal.openModal;

    // Cria o manipulador do menu contextual usando a função importada
    const handleMenuAction = createMenuHandler(this);

    this._contextMenu = new ContextMenu(
      shadow,
      '.menu-trigger',
      '.context-menu',
      handleMenuAction  // <-- passa a função criada
    );

     // Obter Token de acesso ao HA
    const api_token = '/local/json_files/token_api.json';
    const token = await getToken(api_token);

    // Após guardar as configurações, obter os próximos timers a serem executados para actualizar a interface ou mostrar uma mensagem.
    getNextTimer2Execute(token, ip_e_porta);

    // Atualiza o conteúdo do card (título, cor do ícone, informações do temporizador) com base no estado atual da entidade e na configuração.
    this._updateContent();

    // Obter dados dos Timers atualizados e trabalhar com eles (ex: actualizar a interface, mostrar uma mensagem, etc.)
    obterDadosTimers(token, ip_e_porta).then(timers => {
        if (timers) {
            Object.keys(timers).forEach(entityId => {  
              //console.log(`Timer para ${entityId}:`, timers[entityId].weekday + ", " + timers[entityId].execucao + " - " + timers[entityId].action);                    
              //document.getElementById(`${entityId}.content`).innerHTML = timers[entityId].weekday + ", " + timers[entityId].execucao + " - " + timers[entityId].action;
            });                    
        }

    }); // FIM obterDadosTimers(...)

    this._updateContent();


  } // render



  _setupAcEvents() {
    const shadow = this.shadowRoot;
    
    shadow.querySelector('#btn-minus').addEventListener('click', () => {
      if (!this._hass || !this._config.entity) return;
      const targetTemp = this._stateObj ? this._stateObj.attributes.temperature : 25;
      this._hass.callService('climate', 'set_temperature', {
        entity_id: this._config.entity,
        temperature: targetTemp - 1
      });
    });

    shadow.querySelector('#btn-plus').addEventListener('click', () => {
      if (!this._hass || !this._config.entity) return;
      const targetTemp = this._stateObj ? this._stateObj.attributes.temperature : 25;
      this._hass.callService('climate', 'set_temperature', {
        entity_id: this._config.entity,
        temperature: targetTemp + 1
      });
    });

    shadow.querySelector('#pill-mode').addEventListener('click', () => this._openMoreInfo());
    shadow.querySelector('#pill-fan').addEventListener('click', () => this._openMoreInfo());
  }

  _openMoreInfo() {
    const event = new Event('hass-more-info', { bubbles: true, composed: true });
    event.detail = { entityId: this._config.entity };
    this.dispatchEvent(event);
  }

  /**
   * Vincula dinamicamente a geometria do arco e os textos às leituras MQTT
   */
  _updateContent() {

    if (!this._hass || !this._stateObj) return;

    const shadow = this.shadowRoot;
    const titleElem = shadow.querySelector('.card-title-content');
    const timeElem = shadow.querySelector('.timestamp');
    const iconSpan = shadow.querySelector('#toggle-icon');
    const cardIcon = shadow.querySelector('#card-icon');       // seleciona o ha-icon
    const clockDiv = shadow.querySelector('.div_img_clock');
    const svgClock = clockDiv ? clockDiv.querySelector('.svg_clk') : null;

    
    if (!shadow || !this._hass || !this._stateObj) return;
    const tempSensor = this._hass.states[this._tempSensorEntity];
    const currentTemp = tempSensor ? parseFloat(tempSensor.state) : (this._stateObj.attributes.temperature || 25);
    const hvacMode = this._stateObj.state || 'off';
    const fanMode = this._stateObj.attributes.fan_mode || 'auto';
    if (this._customName) shadow.querySelector('#card-title').textContent = this._customName;
    // Atualiza os seletores de texto enviados no seu bloco HTML
    shadow.querySelector('#temp-val').textContent = isNaN(currentTemp) ? '--' : currentTemp;
    shadow.querySelector('#mode-text').textContent = hvacMode === 'cool' ? 'Cool' : hvacMode;
    shadow.querySelector('#pill-mode-val').textContent = hvacMode === 'cool' ? 'Cool' : hvacMode;
    shadow.querySelector('#pill-fan-val').textContent = fanMode;
    // Lógica matemática de preenchimento do arco SVG (Escala 16°C a 30°C)
    const min = 16;
    const max = 30;
    const totalDash = 2 * Math.PI * 85;
    const safeTemp = isNaN(currentTemp) ? 25 : Math.max(min, Math.min(max, currentTemp));
    const progress = (safeTemp - min) / (max - min);
    const activeCircle = shadow.querySelector('.active');
    const handleCircle = shadow.querySelector('.handle');
    if (activeCircle && handleCircle) {
        const arcLength = totalDash * 0.72; // Mantém a abertura inferior do arco padrão
        activeCircle.style.strokeDasharray = `${arcLength} ${totalDash}`;
        activeCircle.style.strokeDashoffset = arcLength - (arcLength * progress);
        // Mapeamento angular da bolinha guia (.handle) baseado na rotação de 135° do CSS
        const angle = 135 + (progress * 270);
        const rad = (angle * Math.PI) / 180;
        const cx = 100 + 85 * Math.cos(rad);
        const cy = 100 + 85 * Math.sin(rad);
        handleCircle.setAttribute('cx', cx);
        handleCircle.setAttribute('cy', cy);
    }


     // Atualiza o atributo data-device-mac-address no header do card
    const cardHeader = shadow.querySelector('.card-header');
    if (cardHeader) {
      const macAddress = this._stateObj?.attributes?.mac_address; 
      if (macAddress) {
        cardHeader.setAttribute('data-device-mac-address', macAddress);
      } else {
        // Opcional: limpar ou definir um valor padrão
        cardHeader.setAttribute('data-device-mac-address', null);
      }
    }

    // Atualizar data-id no botão do menu (exemplo: usando o entity_id como ID)
    const menuTrigger = shadow.querySelector('.menu-trigger');
    if (menuTrigger) {
      const entityId = this._config.entity; // ou this._config.id se existir
      menuTrigger.setAttribute('data-id', entityId || '');
    }


    // Atualiza título priorizando nome personalizado
    if (this._customNameProvided && this._customName) {
      titleElem.textContent = this._customName;
    } else {
      const friendlyName = this._stateObj?.attributes.friendly_name;
      titleElem.textContent = friendlyName || this._config.entity.split('.')[1] || 'Lâmpada';
    }

    // Cor do ícone principal conforme estado
    if (iconSpan) {
      if (this._stateObj.state === 'on') {
        iconSpan.style.color = 'var(--paper-item-icon-active-color, #fdd835)';
      } else {
        iconSpan.style.color = 'var(--paper-item-icon-color, #44739e)';
      }
    }

    // Informações do temporizador
    if (this._config.show_time) {
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
    } else {
      timeElem.textContent = '';
      if (svgClock) {
        svgClock.setAttribute('fill', 'none');
      }
    }
}

/**
   * Alterna o estado da entidade (liga/desliga) através do serviço toggle.
   *
   * @private
   * @param {Event} [e] - Evento de clique (opcional), usado para interromper propagação.
   */
_toggleEntity(e) {
  if (e) e.stopPropagation();
  if (!this._hass || !this._stateObj) return;

  /*const domain = this._config.entity.split('.')[0];
  this._hass.callService(domain, 'toggle', {
    entity_id: this._config.entity
  }).catch(() => {
    this._hass.callService('homeassistant', 'toggle', {
      entity_id: this._config.entity
    });
  });*/
}

getCardSize() {
    return 5;
}

}

customElements.define('ac-card', AcCard);
