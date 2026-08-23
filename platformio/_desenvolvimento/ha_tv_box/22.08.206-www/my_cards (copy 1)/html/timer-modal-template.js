// html/timer-modal-template.js
export const timerModalTemplate = (deviceName = 'Dispositivo') => `
  <div class="config-timers-container" id="config-timers-container" style="display: none;">
    <!-- Guardar dados do dispositivo cujo Timer será configurado -->
    <input type="text" id="timer_save_device_id" name="timer_save_device_id" style="display: none;">
    <div class="config-timers-header">
      <h2 id="timer_device_name">${deviceName}</h2>
    </div>

    <div class="timer-selector-container">
      <div class="timer-selector-grid">
        <label class="timer-selector-label" id="lbl1">
          <input class="timer-selector-input" type="radio" value="1" name="select_timer" checked>
          <div id="div_timer_1" class="timer-selector-number">1</div>
        </label>
        <label class="timer-selector-label" id="lbl2">
          <input class="timer-selector-input" type="radio" value="2" name="select_timer">
          <div id="div_timer_2" class="timer-selector-number">2</div>
        </label>
        <label class="timer-selector-label" id="lbl3">
          <input class="timer-selector-input" type="radio" value="3" name="select_timer">
          <div id="div_timer_3" class="timer-selector-number">3</div>
        </label>
        <label class="timer-selector-label" id="lbl4">
          <input class="timer-selector-input" type="radio" value="4" name="select_timer">
          <div id="div_timer_4" class="timer-selector-number">4</div>
        </label>
      </div>
    </div>

    <div class="timer-config-section">
      <form class="config-form">
        <div class="config-row">
          <div class="checkbox-container">
            <span><label id="lbl_activar_temporizador" class="config-label" for="">Activar Temporizador: -</label></span>
            <input type="checkbox" id="chk_activar" name="activar" hidden>
            <div class="custom-checkbox" role="checkbox" aria-checked="false" tabindex="0"></div>
          </div>
        </div>

        <div class="config-row">
          <label class="config-label" for="select_accao">Acção:</label>
          <div class="select-container">
            <select class="custom-select" id="select_accao" name="select_action" required>
              <option value="" disabled selected>Indique uma acção</option>
              <option class="opcao_accao" value="turn_on">Ligar</option>
              <option class="opcao_accao" value="turn_off">Desligar</option>
              <option class="opcao_accao" value="toggle">Inverter</option>
            </select>
            <div class="select-icon">
              <svg width="25" height="25" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <circle cx="12" cy="12" r="10"></circle>
                <line x1="12" y1="8" x2="12" y2="12"></line>
                <line x1="12" y1="16" x2="12.01" y2="16"></line>
              </svg>
            </div>
          </div>
        </div>

        <div class="config-row">
          <label class="config-label" for="time_hora">Hora:</label>
          <div class="time-input-container">
            <input class="time-input" id="time_hora" type="time" name="appt-time" placeholder="00:00" value="">
            <div class="time-icon">
              <svg width="25" height="25" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <circle cx="12" cy="12" r="10"></circle>
                <polyline points="12 6 12 12 16 14"></polyline>
              </svg>
            </div>
          </div>
        </div>
      </form>

      <div class="weekdays-container">
        <label class="weekdays-label" id="weekdays-label">Dias da Semana:</label>
        <div class="weekdays-grid">
          <label class="weekday-label">
            <input class="weekday-checkbox" type="checkbox" name="sel_week_day" value="0">
            <div class="weekday-letter">D</div>
            <span class="weekday-name">Dom</span>
          </label>
          <label class="weekday-label">
            <input class="weekday-checkbox" type="checkbox" name="sel_week_day" value="1">
            <div class="weekday-letter">S</div>
            <span class="weekday-name">Seg</span>
          </label>
          <label class="weekday-label">
            <input class="weekday-checkbox" type="checkbox" name="sel_week_day" value="2">
            <div class="weekday-letter">T</div>
            <span class="weekday-name">Ter</span>
          </label>
          <label class="weekday-label">
            <input class="weekday-checkbox" type="checkbox" name="sel_week_day" value="3">
            <div class="weekday-letter">Q</div>
            <span class="weekday-name">Qua</span>
          </label>
          <label class="weekday-label">
            <input class="weekday-checkbox" type="checkbox" name="sel_week_day" value="4">
            <div class="weekday-letter">Q</div>
            <span class="weekday-name">Qui</span>
          </label>
          <label class="weekday-label">
            <input class="weekday-checkbox" type="checkbox" name="sel_week_day" value="5">
            <div class="weekday-letter">S</div>
            <span class="weekday-name">Sex</span>
          </label>
          <label class="weekday-label">
            <input class="weekday-checkbox" type="checkbox" name="sel_week_day" value="6">
            <div class="weekday-letter">S</div>
            <span class="weekday-name">Sáb</span>
          </label>
        </div>
      </div>

      <div class="current-time-date">
        <div class="current-time">
          <span id="dia_semana_actual">Segunda-feira</span> <span id="hora_actual">00:00:00</span>
        </div>
      </div>
    </div>

    <div class="timers-toolbar">     
      <button class="tool-btn btn-save" id="timer-save-btn">         
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
          <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path>
          <polyline points="17 21 17 13 7 13 7 21"></polyline>
          <polyline points="7 3 7 8 15 8"></polyline>
        </svg>
        Guardar
      </button>

      <button class="tool-btn btn-close" id="timer-close-btn">
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
          <circle cx="12" cy="12" r="10"></circle>
          <line x1="15" y1="9" x2="9" y2="15"></line>
          <line x1="9" y1="9" x2="15" y2="15"></line>
        </svg>
        Sair
      </button>
    </div>
  </div>

  <div class="toast" id="toast"></div>
  
`;