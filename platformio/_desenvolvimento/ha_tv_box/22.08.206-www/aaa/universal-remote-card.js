// ==========================================
// 1. STYLES (CSS)
// ==========================================
const cardStyles = `
  :host {
    display: block;
    margin: 4px;
  }
  ha-card {
    background: var(--ha-card-background, var(--card-background-color, white));
    border-radius: var(--ha-card-border-radius, 16px);
    box-shadow: var(--ha-card-box-shadow, none);
    padding: 16px;
    font-family: var(--paper-font-body1_-_font-family, sans-serif);
    transition: all 0.3s ease;
  }
  .header {
    text-align: center;
    margin-bottom: 16px;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 4px;
  }
  
  /* Dynamic Combo Box Styling */
  .room-dropdown {
    width: 80%;
    max-width: 220px;
    padding: 8px 12px;
    font-size: 16px;
    font-weight: bold;
    border-radius: 8px;
    border: 1px solid var(--divider-color, #e0e0e0);
    background-color: var(--card-background-color, #ffffff);
    color: var(--primary-text-color);
    cursor: pointer;
    outline: none;
    text-align: center;
    text-align-last: center;
  }
  .room-dropdown:focus {
    border-color: var(--accent-color, #ff9800);
  }

  .subtitle {
    font-size: 13px;
    color: var(--secondary-text-color);
    text-transform: capitalize;
  }
  
  .mode-selector {
    display: grid;
    grid-template-columns: repeat(5, 1fr);
    gap: 6px;
    margin-bottom: 20px;
  }
  
  @media (max-width: 380px) {
    .mode-selector {
      grid-template-columns: repeat(5, minmax(50px, 1fr));
      overflow-x: auto;
      padding-bottom: 4px;
    }
  }

  .mode-btn {
    background: var(--secondary-background-color, #f4f5f7);
    color: var(--primary-text-color);
    border: none;
    padding: 10px 4px;
    border-radius: 10px;
    cursor: pointer;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 4px;
    font-size: 11px;
    transition: all 0.2s ease;
  }
  .mode-btn.active {
    background: var(--accent-color, #ff9800);
    color: white;
  }
  .mode-btn ha-icon {
    --mdc-icon-size: 20px;
  }

  .controls-grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 16px;
    justify-items: center;
    align-items: center;
    max-width: 280px;
    margin: 0 auto;
  }

  .btn {
    width: 60px;
    height: 60px;
    border-radius: 50%;
    border: none;
    cursor: pointer;
    display: flex;
    justify-content: center;
    align-items: center;
    color: white;
    box-shadow: 0 3px 6px rgba(0,0,0,0.12);
    transition: transform 0.1s ease, opacity 0.2s;
  }
  .btn:active { transform: scale(0.90); }
  .btn ha-icon { --mdc-icon-size: 26px; }
  
  .spacer {
    width: 60px;
    height: 60px;
    visibility: hidden;
  }
  
  .btn-power   { background-color: var(--error-color, #db4437); }
  .btn-action  { background-color: var(--primary-color, #03a9f4); }
  .btn-neutral { background-color: #616161; }
  .btn-accent  { background-color: var(--success-color, #4caf50); }
  .btn-orange  { background-color: #ff9800; }
`;

// ==========================================
// 2. TEMPLATE (HTML)
// ==========================================
const cardTemplate = `
  <ha-card>
    <div class="header">
      <!-- Symmetrical Combo Box -->
      <select id="room-select" class="room-dropdown"></select>
      <div class="subtitle" id="card-subtitle">Loading...</div>
    </div>
    
    <div class="mode-selector">
      <button class="mode-btn active" data-mode="tv">
        <ha-icon icon="mdi:television"></ha-icon>
        <span>TV</span>
      </button>
      <button class="mode-btn" data-mode="ac">
        <ha-icon icon="mdi:air-conditioner"></ha-icon>
        <span>AC</span>
      </button>
      <button class="mode-btn" data-mode="fan">
        <ha-icon icon="mdi:fan"></ha-icon>
        <span>Fan</span>
      </button>
      <button class="mode-btn" data-mode="radio">
        <ha-icon icon="mdi:radio"></ha-icon>
        <span>Radio</span>
      </button>
      <button class="mode-btn" data-mode="media">
        <ha-icon icon="mdi:play-network"></ha-icon>
        <span>Media</span>
      </button>
    </div>

    <div id="controls-container" class="controls-grid"></div>
  </ha-card>
`;

const layouts = {
  tv: `
    <button class="btn btn-neutral" data-command="volume_down"><ha-icon icon="mdi:volume-minus"></ha-icon></button>
    <button class="btn btn-power" data-command="power"><ha-icon icon="mdi:power"></ha-icon></button>
    <button class="btn btn-neutral" data-command="volume_up"><ha-icon icon="mdi:volume-plus"></ha-icon></button>
    <button class="btn btn-action" data-command="previous"><ha-icon icon="mdi:skip-previous"></ha-icon></button>
    <button class="btn btn-action" data-command="play_pause"><ha-icon icon="mdi:play-pause"></ha-icon></button>
    <button class="btn btn-action" data-command="next"><ha-icon icon="mdi:skip-next"></ha-icon></button>
  `,
  ac: `
    <button class="btn btn-neutral" data-command="temp_down"><ha-icon icon="mdi:thermometer-minus"></ha-icon></button>
    <button class="btn btn-power" data-command="power"><ha-icon icon="mdi:power"></ha-icon></button>
    <button class="btn btn-neutral" data-command="temp_up"><ha-icon icon="mdi:thermometer-plus"></ha-icon></button>
    <div class="spacer"></div>
    <button class="btn btn-accent" data-command="fan_mode"><ha-icon icon="mdi:fan"></ha-icon></button>
    <div class="spacer"></div>
  `,
  fan: `
    <button class="btn btn-accent" data-command="speed_down"><ha-icon icon="mdi:minus"></ha-icon></button>
    <button class="btn btn-power" data-command="power"><ha-icon icon="mdi:power"></ha-icon></button>
    <button class="btn btn-accent" data-command="speed_up"><ha-icon icon="mdi:plus"></ha-icon></button>
    <div class="spacer"></div>
    <button class="btn btn-neutral" data-command="oscillate"><ha-icon icon="mdi:cached"></ha-icon></button>
    <div class="spacer"></div>
  `,
  radio: `
    <button class="btn btn-neutral" data-command="tune_down"><ha-icon icon="mdi:tune-vertical"></ha-icon></button>
    <button class="btn btn-power" data-command="power"><ha-icon icon="mdi:power"></ha-icon></button>
    <button class="btn btn-neutral" data-command="tune_up"><ha-icon icon="mdi:tune"></ha-icon></button>
    <button class="btn btn-orange" data-command="preset_1"><ha-icon icon="mdi:numeric-1-box"></ha-icon></button>
    <button class="btn btn-orange" data-command="preset_2"><ha-icon icon="mdi:numeric-2-box"></ha-icon></button>
    <button class="btn btn-orange" data-command="preset_3"><ha-icon icon="mdi:numeric-3-box"></ha-icon></button>
  `,
  media: `
    <button class="btn btn-neutral" data-command="mute"><ha-icon icon="mdi:volume-mute"></ha-icon></button>
    <button class="btn btn-power" data-command="power"><ha-icon icon="mdi:power"></ha-icon></button>
    <button class="btn btn-neutral" data-command="source_select"><ha-icon icon="mdi:input"></ha-icon></button>
    <button class="btn btn-action" data-command="rewind"><ha-icon icon="mdi:rewind"></ha-icon></button>
    <button class="btn btn-action" data-command="play_pause"><ha-icon icon="mdi:play-pause"></ha-icon></button>
    <button class="btn btn-action" data-command="fast_forward"><ha-icon icon="mdi:fast-forward"></ha-icon></button>
  `
};

const defaultLabels = {
  tv: "Televisão",
  ac: "Ar Condicionado",
  fan: "Ventilador",
  radio: "Rádio / Estação",
  media: "Media Player"
};

// ==========================================
// 3. LOGIC (JAVASCRIPT)
// ==========================================
class UniversalRemoteCard extends HTMLElement {
  setConfig(config) {
    this._config = config;
    // Load room array list from YAML configuration, with a default fallback list
    this._rooms = config.rooms || ["Sala de Estar", "Quarto", "Cozinha"];
    this._currentMode = 'tv';
  }

  set hass(hass) {
    this._hass = hass;
  }

  connectedCallback() {
    this.innerHTML = `      
      <style>${cardStyles}</style>
      ${cardTemplate}
    `;
    this._buildRoomSelector();
    this._updateSubtitle();
    this._renderControls();
    this._bindEvents();
  }

  _buildRoomSelector() {
    const selectEl = this.querySelector('#room-select');
    if (selectEl) {
      selectEl.innerHTML = this._rooms
        .map(room => `<option value="${room.toLowerCase().replace(/\s+/g, '_')}">${room}</option>`)
        .join('');
    }
  }

  _updateSubtitle() {
    const subtitleEl = this.querySelector('#card-subtitle');
    if (subtitleEl) subtitleEl.innerText = defaultLabels[this._currentMode];
  }

  _renderControls() {
    const container = this.querySelector('#controls-container');
    if (container) {
      container.innerHTML = layouts[this._currentMode];
      this._bindActionButtons();
    }
  }

  _bindEvents() {
    const modeButtons = this.querySelectorAll('.mode-btn');
    modeButtons.forEach(btn => {
      btn.addEventListener('click', (e) => {
        modeButtons.forEach(b => b.classList.remove('active'));
        const target = e.currentTarget;
        target.classList.add('active');
        this._currentMode = target.getAttribute('data-mode');
        
        this._updateSubtitle();
        this._renderControls();
      });
    });
  }

  _bindActionButtons() {
    const actionButtons = this.querySelectorAll('.btn');
    actionButtons.forEach(btn => {
      btn.addEventListener('click', (e) => {
        const command = e.currentTarget.getAttribute('data-command');
        this._fireRemoteEvent(command);
      });
    });
  }

  _fireRemoteEvent(command) {
    if (!this._hass) return;

    // Dynamically retrieve the current combo-box chosen value
    const selectEl = this.querySelector('#room-select');
    const activeRoom = selectEl ? selectEl.value : "default_room";

    // Fire the event containing targeted room, targeted device mode, and the code action
    this._hass.callService("homeassistant", "fire_event", {
      event_type: "remote_button_pressed",
      event_data: {
        room: activeRoom,      // e.g., "sala_de_estar", "quarto_principal"
        device: this._currentMode, // 'tv', 'ac', 'fan', 'radio', 'media'
        action: command            // 'power', 'volume_up', etc.
      }
    });
  }

  getCardSize() {
    return 4;
  }
}

customElements.define("universal-remote-card", UniversalRemoteCard);
