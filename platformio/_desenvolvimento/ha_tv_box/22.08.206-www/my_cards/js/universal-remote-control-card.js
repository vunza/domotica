import { remoteControlCardStyles } from '../css/universal-remote-control-card-styles.js';
import { remoteControlcardTemplate } from '../html/universal-remote-control-card-template.js';

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
    <button id="btn-ac-power" class="btn btn-power" data-command="power"><ha-icon icon="mdi:power"></ha-icon></button>
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

class UniversalRemoteCard extends HTMLElement {

  setConfig(config) {
    this._config = config;

    this._rooms = config.rooms || [
      "Sala de Estar",
      "Quarto",
      "Cozinha"
    ];

    this._currentMode = 'tv';
  }

  set hass(hass) {
    this._hass = hass;
  }

  connectedCallback() {

    this.innerHTML = `
      <style>
        ${remoteControlCardStyles}
      </style>

      ${remoteControlcardTemplate}
    `;

    this._buildRoomSelector();
    this._updateSubtitle();
    this._renderControls();
    this._bindEvents();
  }

  // ==========================================
  // ROOM SELECTOR
  // ==========================================

  _buildRoomSelector() {

    const selectEl = this.querySelector('#room-select');

    if (!selectEl) return;

    selectEl.innerHTML = this._rooms
      .map(room => `
        <option value="${room.toLowerCase().replace(/\s+/g, '_')}">
          ${room}
        </option>
      `)
      .join('');
  }

  // ==========================================
  // SUBTITLE
  // ==========================================

  _updateSubtitle() {

    const subtitleEl = this.querySelector('#card-subtitle');

    if (subtitleEl) {
      subtitleEl.innerText = defaultLabels[this._currentMode];
    }
  }

  // ==========================================
  // RENDER CONTROLS
  // ==========================================

  _renderControls() {

    const container = this.querySelector('#controls-container');

    if (!container) return;

    container.innerHTML = layouts[this._currentMode];

    this._bindActionButtons();
  }

  // ==========================================
  // MODE BUTTON EVENTS
  // ==========================================

  _bindEvents() {    

    const modeButtons = this.querySelectorAll('.mode-btn');

    modeButtons.forEach(btn => {

      btn.addEventListener('click', (e) => {

        modeButtons.forEach(b => {
          b.classList.remove('active');
        });

        const target = e.currentTarget;

        target.classList.add('active');

        this._currentMode = target.getAttribute('data-mode');

        this._updateSubtitle();

        this._renderControls();

        console.log(
          `[REMOTE] modo alterado para: ${this._currentMode}`
        );
      });
    });

  }

  // ==========================================
  // ACTION BUTTON EVENTS
  // ==========================================

  _bindActionButtons() {

    const boton = this.querySelector('#btn-ac-power');
    if (boton) {
      boton.addEventListener('click', () => {
        
        this._hass
          .callService("script", "desligar_ac", {})
          .then(() => {
            console.log("Script desligar_ac executado com sucesso!");
          })
          .catch((error) => {
            console.error("Erro ao executar o script:", error);
          });

      });
      
    }

    const actionButtons = this.querySelectorAll('.btn');

    /*actionButtons.forEach(btn => {

      btn.addEventListener('click', async (e) => {

        const target = e.currentTarget;

        const command = target.getAttribute('data-command');

        console.log(
          `[REMOTE] botão clicado -> ${command}`
        );

        // efeito visual ao clicar
        target.classList.add('pressed');

        setTimeout(() => {
          target.classList.remove('pressed');
        }, 150);

        await this._fireRemoteEvent(command);
      });
    });*/
  }

  // ==========================================
  // FIRE EVENT / SERVICE
  // ==========================================

  async _fireRemoteEvent(command) {

    if (!this._hass) {
      console.error('[REMOTE] Home Assistant indisponível');
      return;
    }

    const selectEl = this.querySelector('#room-select');

    const activeRoom = selectEl
      ? selectEl.value
      : "default_room";

    const payload = {
      room: activeRoom,
      device: this._currentMode,
      action: command
    };

    console.log('[REMOTE] payload:', payload);

    try {

      // Evento HA
      await this._hass.callService(
        "homeassistant",
        "fire_event",
        {
          event_type: "remote_button_pressed",
          event_data: payload
        }
      );

      // Opcional:
      // enviar também para script dedicado

      /*
      await this._hass.callService(
        "script",
        "turn_on",
        {
          entity_id: "script.remote_dispatcher",
          variables: payload
        }
      );
      */

      console.log(
        `[REMOTE] comando enviado com sucesso: ${command}`
      );

    } catch (err) {

      console.error(
        `[REMOTE] erro ao enviar comando ${command}`,
        err
      );
    }
  }

  // ==========================================
  // CARD SIZE
  // ==========================================

  getCardSize() {
    return 4;
  }
}

customElements.define(
  "universal-remote-control-card",
  UniversalRemoteCard
);