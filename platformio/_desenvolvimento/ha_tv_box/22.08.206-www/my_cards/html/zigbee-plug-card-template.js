export const plugCardTemplate = (initialTitle, clockSvg, menuHtml) => `
  <div class="card">
    <div class="card-header" data-device-mac-address="">
      <div class="card-title">
        <span class="card-icon" id="toggle-icon">          
          <ha-icon id="card-icon" icon="mdi:power-plug-off"></ha-icon> 
        </span>
        <div class="title-wrapper">
          <span class="card-title-content">${initialTitle}</span>
          <div class="status-badges">
            <span class="zigbee-badge" id="lqi-badge">LQI: --</span>
            <span class="status-state-badge" id="state-badge">DESLIGADO</span>
          </div>
        </div>
      </div>
      <div class="header-actions" style="display: flex; align-items: center; gap: 4px;">
        <button class="chart-toggle-btn" aria-label="Alternar Gráfico" title="Mostrar/Ocultar Gráfico">
          <ha-icon icon="mdi:chart-line"></ha-icon>
        </button>
        <div class="menu-container">
          <button class="menu-trigger" aria-label="Menu" data-id="">
            <svg width="24" height="24" fill="currentColor" viewBox="0 0 24 24">
              <path d="M12 8c1.1 0 2-.9 2-2s-.9-2-2-2-2 .9-2 2 .9 2 2 2zm0 2c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm0 6c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2z"/>
            </svg>
          </button>
          <div class="context-menu">${menuHtml}</div>
        </div>
      </div>
    </div>

    <!-- Painel de Telemetria da Tomada -->
    <div class="metrics-container">
      <div class="metric-block metric-power">
        <div class="metric-value-row">
          <span class="metric-num" id="val-power">--</span>
          <span class="metric-unit">W</span>
        </div>
        <span class="metric-label">Potência</span>
      </div>
      <div class="metric-block metric-voltage">
        <div class="metric-value-row">
          <span class="metric-num" id="val-voltage">--</span>
          <span class="metric-unit">V</span>
        </div>
        <span class="metric-label">Tensão</span>
      </div>
      <div class="metric-block metric-current">
        <div class="metric-value-row">
          <span class="metric-num" id="val-current">--</span>
          <span class="metric-unit">A</span>
        </div>
        <span class="metric-label">Corrente</span>
      </div>
    </div>

    <!-- Gráfico SVG Oculto por Defeito -->
    <div class="chart-box" style="display: none;">
      <svg class="chart-svg" id="telemetry-chart" viewBox="0 0 500 130" preserveAspectRatio="none">
        <defs>
          <linearGradient id="grad-power-sw" x1="0" y1="0" x2="0" y2="1">
            <stop offset="0%" stop-color="#00e676" stop-opacity="0.35"/>
            <stop offset="100%" stop-color="#00e676" stop-opacity="0.0"/>
          </linearGradient>
          <linearGradient id="grad-voltage-sw" x1="0" y1="0" x2="0" y2="1">
            <stop offset="0%" stop-color="#ff9100" stop-opacity="0.25"/>
            <stop offset="100%" stop-color="#ff9100" stop-opacity="0.0"/>
          </linearGradient>
          <linearGradient id="grad-current-sw" x1="0" y1="0" x2="0" y2="1">
            <stop offset="0%" stop-color="#29b6f6" stop-opacity="0.25"/>
            <stop offset="100%" stop-color="#29b6f6" stop-opacity="0.0"/>
          </linearGradient>
        </defs>
        <g id="grid-lines"></g>
        <g id="chart-paths"></g>
      </svg>
      <div class="time-axis" id="time-axis"></div>
    </div>

    <div class="extra-entities"></div>

    <div class="card-content">
      <div class="div_img_clock">${clockSvg}</div>
      <div class="div_content">
        <span class="timestamp"></span>
      </div>
    </div>
  </div>
`;