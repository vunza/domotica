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

/**
 * Card personalizado para Home Assistant Lovelace que exibe uma lâmpada,
 * permite alternar seu estado e mostra informações de temporizador.
 * Também inclui um menu contextual com opções como renomear o dispositivo.
 *
 * @extends HTMLElement
 */
class GenericCard extends HTMLElement {
  /**
   * Retorna a configuração padrão (stub) usada pelo editor visual do Lovelace.
   *
   * @static
   * @returns {{entity: string, name: string, show_time: boolean}} Configuração padrão.
   */
  static getStubConfig() {
    return { entity: '', name: '', show_time: true };
  }

  /**
   * Cria uma instância do card e inicializa o shadow root e propriedades internas.
   */
  constructor() {
    super();
    this.attachShadow({ mode: 'open' });

    /** @private @type {Object} Configuração do card (entity, name, show_time). */
    this._config = {};

    /** @private @type {Object|null} Objeto hass do Home Assistant. */
    this._hass = null;

    /** @private @type {Object|null} Objeto de estado da entidade. */
    this._stateObj = null;

    /** @private @type {boolean} Indica se um nome personalizado foi fornecido na configuração. */
    this._customNameProvided = false;

    /** @private @type {ContextMenu|null} Instância do menu contextual. */
    this._contextMenu = null;

    /** @private @type {string|null} Nome personalizado armazenado internamente. */
    this._customName = null;

    /**
     * @private @type {boolean|null} Indica se a entidade é uma tomada (plug) para ajustar o ícone e comportamento.
       O valor é definido na configuração (config.is_a_plug) e usado para determinar o ícone a ser exibido.
     */
    this._isAplug = null;
  }

  /**
   * Chamado automaticamente quando o elemento é removido do DOM.
   * Limpa os listeners do menu contextual.
   */
  disconnectedCallback() {
    super.disconnectedCallback();
    if (this._contextMenu) {
      this._contextMenu.destroy();
      this._contextMenu = null;
    }
  }

  /**
   * Define a configuração do card. Chamado pelo Lovelace.
   *
   * @param {Object} config - Objeto de configuração.
   * @param {string} config.entity - ID da entidade da lâmpada (obrigatório).
   * @param {string} [config.name] - Nome personalizado a ser exibido no card.
   * @param {boolean} [config.show_time=true] - Se deve exibir informações de temporizador.
   * @throws {Error} Se a entidade não for fornecida.
   */
  setConfig(config) {
    if (!config.entity) throw new Error('Defina a entidade da lâmpada');
    this._config = config;
    this._customNameProvided = !!config.name;
    this._customName = config.name || null;
    this._render();
    this._updateContent();
    this._isAplug = config.is_a_plug ?? false;
    
    // Armazena a lista de entidades, das plug, extras (padrão: array vazio)
    this._extraEntities = config.extra_entities || [];

    // Armazena a lista de atributos a exibir, de dispositivos autodiscovery MQTT, (padrão: array vazio)
    this._displayAttributes = config.display_attributes || []; // ex: ['ups1_current', 'ups1_voltage']
  }

  /**
   * Atualiza o objeto hass e o estado da entidade sempre que o estado global mudar.
   * Chamado pelo Lovelace.
   *
   * @param {Object} hass - Objeto hass do Home Assistant.
   */
  set hass(hass) {
    this._hass = hass;
    
    if (this._config.entity) {
      this._stateObj = hass.states[this._config.entity];
      this._updateContent();
    }  

    // Atualiza o menu com o novo hass
    if (this._contextMenu) {
      this._contextMenu.setHass(hass);
    }

  }

  

  
  /**
   * Renderiza a estrutura HTML do card no shadow DOM.
   *
   * @private
   */
  async _render() {
    const shadow = this.shadowRoot;
    const clockSvg = `
      <svg class="svg_clk" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="10"></circle>
        <polyline points="12 6 12 12 16 14"></polyline>
      </svg>
    `;

    const initialTitle = this._config.name || 'Carregando...';

    const html = `
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

    shadow.innerHTML = html;

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

  }
  

  /**
   * Inicializa o modal de configuração de timers, utilizando a função importada para configurar os eventos e lógica do modal.
   *
   * @private 
   */
  _initTimerModal() {
    // Chama a função importada, passando a instância atual e o shadowRoot
    const timerModal = initTimerModal(this, this.shadowRoot);
    
    // Armazena a função de abrir no contexto da classe
    this._openTimerModal = timerModal.openModal;
    
    // Opcional: armazenar close se precisar
    this._closeTimerModal = timerModal.closeModal;
  }

  /**
   * Inicializa o modal de renomear dispositivo.
   *
   * @private
   */
  _initRenameModal() {
    const renameModal = initRenameModal(this, this.shadowRoot);
    this._openRenameModal = renameModal.openModal;
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

    const domain = this._config.entity.split('.')[0];
    this._hass.callService(domain, 'toggle', {
      entity_id: this._config.entity
    }).catch(() => {
      this._hass.callService('homeassistant', 'toggle', {
        entity_id: this._config.entity
      });
    });
  }


  /**
   * Atualiza o conteúdo dinâmico do card: título, cor do ícone e informações do temporizador.
   *
   * @private
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



    if(this._isAplug && cardIcon){       
      cardIcon.setAttribute('icon', 'mdi:power-plug');      
    }


    // ========== RENDERIZA AS ENTIDADES EXTRAS ==========
    const extraContainer = shadow.querySelector('.extra-entities');
     
    if (extraContainer) {
      let html = '';
      let hasAny = false;       

      // 1. Entidades extras, plugs, (já existente)
      if (this._extraEntities && this._extraEntities.length > 0) {
        html += '<div class="extra-grid">';
        for (const item of this._extraEntities) {
          const entityId = item.entity;
          const stateObj = this._hass.states[entityId];
          let value = 'indisponível';
          let unit = item.unit || '';

          if (stateObj) {
            value = stateObj.state;
            if (!unit && stateObj.attributes.unit_of_measurement) {
              unit = stateObj.attributes.unit_of_measurement;
            }
          }

          const displayName = item.name || entityId.split('.').pop();
          html += `
            <div class="extra-item">
              <span class="extra-name">${displayName}</span>
              <span class="extra-value">${value} ${unit}</span>
            </div>
          `;
        }
        html += '</div>';
        hasAny = true;
      }

      // 2. Atributos da entidade principal, dispositivos autodiscovery MQTT (se configurados)
      if (this._displayAttributes && this._displayAttributes.length > 0) {
        const attrs = this._stateObj?.attributes || {};
        if (!hasAny) html += '<div class="extra-grid">';
        
        for (const attrKey of this._displayAttributes) {
          let value = attrs[attrKey];
          if (value === undefined || value === null) value = '—';
          
          // 👇 MAPEAMENTO PARA PORTUGUÊS
          let displayName = '';
          switch (attrKey) {
            case 'ups1_current':
              displayName = 'Corrente';
              break;
            case 'ups1_voltage':
              displayName = 'Tensão';
              break;
            case 'ups1_temperature':
              displayName = 'Temperatura';
              break;
            case 'ups1_humidity':
              displayName = 'Humidade';
              break;
            default:
              // Fallback: remove prefixo e capitaliza
              displayName = attrKey
                .replace(/^ups1_/, '')
                .replace('_', ' ')
                .replace(/\b\w/g, l => l.toUpperCase());
          }
          
          // Define unidade de medida
          let unit = '';
          if (attrKey.includes('current')) unit = 'A';
          else if (attrKey.includes('voltage')) unit = 'V';
          else if (attrKey.includes('temperature')) unit = '°C';
          else if (attrKey.includes('humidity')) unit = '%';

          html += `
            <div class="extra-item">
              <span class="extra-name">${displayName}</span>
              <span class="extra-value">${value} ${unit}</span>
            </div>
          `;
        }
        if (!hasAny) html += '</div>';
        hasAny = true;
      }

      extraContainer.innerHTML = hasAny ? html : '';
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
   * Exibe uma mensagem temporária.
   * @param {string} mensagem - Texto a ser exibido.   
   */
  _showToast(message, backgroundColor = 'var(--primary-color)') {
      const toast = this.shadowRoot.getElementById('toast');
      toast.textContent = message;
      toast.classList.add('show');
      toast.style.backgroundColor = backgroundColor;
      
      setTimeout(() => {
          toast.classList.remove('show');
      }, 3000);
  }



  /**
   * Retorna o tamanho do card em linhas para o layout em grade do Lovelace.
   *
   * @returns {number} Altura do card (2 linhas).
   */
  getCardSize() {
    let base = 2; // altura do cabeçalho + timer
    if (this._extraEntities) {
      base += Math.ceil(this._extraEntities.length / 2); // cada dois sensores adiciona 1 linha
    }
    return Math.min(base, 6); // limita a 6 linhas
  }
  
}

// Define o elemento customizado
customElements.define('generic-card', GenericCard);