export const acCardTemplate = (initialTitle, clockSvg, menuHtml) => `
<ha-card style="border-radius: 12px;">

  <!-- Cabeçalho com Menu de Contexto integrado ao seu ecossistema -->
  <div class="card-header" data-device-mac-address="">
    <div class="card-title">
      <span class="card-icon" id="toggle-icon">          
        <ha-icon id="card-icon" icon="mdi:lightbulb"></ha-icon> 
      </span>
      <span class="card-title-content">${initialTitle}</span>
    </div>
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
    
  <!-- ÁREA PARA SENSORES EXTRA -->
  <div class="extra-entities"></div>

  <!-- Conteúdo Principal do AC -->
  <div class="container" id="ac-core-content">
    <!-- Área do Arco e Temperatura -->
    <div class="dial-container">
      <svg class="dial" id="dial-arc-svg" viewBox="0 0 200 200">
        <circle class="bg" cx="100" cy="100" r="85"></circle>
        <circle class="active" cx="100" cy="100" r="85"></circle>
        <circle class="handle" cx="100" cy="100" r="85"></circle>
      </svg>
      <div class="temp-display">
        <span class="mode-label" id="mode-text">Cool</span>
        <div class="temp-num"><span id="temp-val">25</span><span class="unit">°C</span></div>
      </div>
    </div> 

    <!-- Botões de Controle (Power, Termómetro + Símbolos +/- Separados e Ocultar/Exibir Arco) -->
    <div class="controls">
      <!-- Botão Power -->
      <button class="btn-step btn-power" id="btn-power" aria-label="Ligar/Desligar AC" title="Ligar/Desligar">
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M18.36 6.64a9 9 0 1 1-12.73 0"></path>
          <line x1="12" y1="2" x2="12" y2="12"></line>
        </svg>
      </button>

      <!-- Botão Diminuir Temp (Termómetro + Sinal de Menos separado) -->
      <button class="btn-step" id="btn-minus" aria-label="Diminuir Temperatura" title="Diminuir Temperatura">
        <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z"/>
        </svg>
        <svg width="11" height="11" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
          <line x1="5" y1="12" x2="19" y2="12"></line>
        </svg>
      </button>

      <!-- Botão Aumentar Temp (Termómetro + Sinal de Mais separado) -->
      <button class="btn-step" id="btn-plus" aria-label="Aumentar Temperatura" title="Aumentar Temperatura">
        <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z"/>
        </svg>
        <svg width="11" height="11" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
          <line x1="12" y1="5" x2="12" y2="19"></line>
          <line x1="5" y1="12" x2="19" y2="12"></line>
        </svg>
      </button>

      <!-- Botão Ocultar / Visualizar Arco -->
      <button class="btn-step btn-arc-toggle" id="btn-toggle-arc" aria-label="Ocultar/Exibir Arco" title="Ocultar/Exibir Arco">
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path>
          <circle cx="12" cy="12" r="3"></circle>
        </svg>
      </button>
    </div>

    <!-- Pílulas Inferiores de Status -->
    <div class="pill-grid" style="display:none;">
      <div class="pill-button" id="pill-mode">
        <ha-icon icon="mdi:snowflake"></ha-icon>
        <div class="pill-text">
          <span class="pill-title">Mode</span>
          <span class="pill-value" id="pill-mode-val">Cool</span>
        </div>
      </div>
      <div class="pill-button" id="pill-fan">
        <ha-icon icon="mdi:fan"></ha-icon>
        <div class="pill-text">
          <span class="pill-title">Fan mode</span>
          <span class="pill-value" id="pill-fan-val">Auto</span>
        </div>
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