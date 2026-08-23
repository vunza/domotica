export const acCardTemplate = (initialTitle, clockSvg, menuHtml) => `
<ha-card class="ac-card-root">

  <!-- Cabeçalho -->
  <div class="card-header" data-device-mac-address="">
    <div class="card-title">
      <span class="card-icon" id="toggle-icon">          
        <ha-icon id="card-icon" icon="mdi:air-conditioner"></ha-icon> 
      </span>
      <div class="title-wrapper">
        <span class="card-title-content">${initialTitle}</span>
        <div class="status-badges">
          <span class="zigbee-badge" id="lqi-badge" style="display: none;">Zigbee</span>
          <span class="status-state-badge" id="state-badge">DESLIGADO</span>
        </div>
      </div>
    </div>
    <div class="header-actions" style="display: flex; align-items: center; gap: 4px;">
      <button class="chart-toggle-btn" id="btn-toggle-arc" aria-label="Alternar Arco" title="Mostrar/Ocultar Arco">
        <ha-icon icon="mdi:chart-line"></ha-icon>
      </button>
      <div class="menu-container">
        <button class="menu-trigger" aria-label="Menu" data-id="">
          <svg width="24" height="24" fill="currentColor" viewBox="0 0 24 24">
            <path d="M12 8c1.1 0 2-.9 2-2s-.9-2-2-2-2 .9-2 2 .9 2 2 2zm0 2c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm0 6c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2z"/>
          </svg>
        </button>
        <div class="context-menu">
          ${menuHtml}
        </div>
      </div>
    </div>
  </div>
    
  <!-- Entidades Extras -->
  <div class="extra-entities"></div>

  <!-- Conteúdo Principal -->
  <div class="container" id="ac-core-content">
    
    <!-- Seção do Arco com Botões nos Extremos -->
    <div class="arc-collapsible-section" style="display: none;">
      <div class="dial-container">
        <!-- Botão Menus nos Extremos (Esquerda) -->
        <button class="btn-dial-extreme btn-extreme-left" id="btn-minus" title="Diminuir Temperatura">
            <ha-icon icon="mdi:thermometer-minus"></ha-icon>
        </button>

        <svg class="dial" id="dial-arc-svg" viewBox="0 0 200 200">
          <circle class="bg" cx="100" cy="100" r="85"></circle>
          <circle class="active" cx="100" cy="100" r="85"></circle>
          <circle class="handle" cx="100" cy="100" r="85"></circle>
        </svg>

        <div class="temp-display">
          <div class="temp-num"><span id="temp-val">25</span><span class="unit">°C</span></div>
          <div class="arc-footer-info">
            <div class="arc-info-item">
              <ha-icon icon="mdi:snowflake"></ha-icon>
              <span id="arc-mode-val">Cool</span>
            </div>
            <div class="arc-divider"></div>
            <div class="arc-info-item">
              <ha-icon icon="mdi:fan"></ha-icon>
              <span id="arc-fan-val">Auto</span>
            </div>
          </div>
        </div>

        <!-- Botão Mais nos Extremos (Direita) -->
        <button class="btn-dial-extreme btn-extreme-right" id="btn-plus" title="Aumentar Temperatura">
            <ha-icon icon="mdi:thermometer-plus"></ha-icon>
        </button>
      </div>
    </div>

    <!-- Painel Compacto (Modo | Temperatura | Ventilador) -->
    <div class="metrics-container" id="ac-metrics-compact" style="display: grid;">
      <div class="metric-block metric-mode">
        <div class="metric-value-row">
          <span class="metric-num" id="compact-mode-val" style="font-size: 1.1rem; text-transform: capitalize;">Cool</span>
        </div>
        <span class="metric-label">Modo</span>
      </div>

      <div class="metric-block metric-temp">
        <div class="metric-value-row">
          <span class="metric-num" id="compact-temp-val">25</span>
          <span class="metric-unit">°C</span>
        </div>
        <span class="metric-label">Temperatura</span>
      </div>

      <div class="metric-block metric-fan">
        <div class="metric-value-row">
          <span class="metric-num" id="compact-fan-val" style="font-size: 1.1rem; text-transform: capitalize;">Auto</span>
        </div>
        <span class="metric-label">Ventilador</span>
      </div>
    </div>

  </div>

  <div class="card-content">
    <div class="div_img_clock">${clockSvg}</div>
    <div class="div_content">
      <span class="timestamp"></span>
    </div>
  </div>

</ha-card>
`;