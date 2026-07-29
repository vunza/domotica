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
    
  <!-- NOVA ÁREA PARA SENSORES -->
  <div class="extra-entities"></div>

  <!-- O seu código HTML enviado integrado com ID interno para manipulação -->
  <div class="container" id="ac-core-content">
    <!-- Área do Arco e Temperatura -->
    <div class="dial-container">
      <svg class="dial" viewBox="0 0 200 200">
        <circle class="bg" cx="100" cy="100" r="85"></circle>
        <circle class="active" cx="100" cy="100" r="85"></circle>
        <circle class="handle" cx="100" cy="100" r="85"></circle>
      </svg>
      <div class="temp-display">
        <span class="mode-label" id="mode-text">Cool</span>
        <div class="temp-num"><span id="temp-val">25</span><span class="unit">°C</span></div>
      </div>
    </div> <!-- CORRIGIDO: Tag </div> adicionada para fechar a dial-container -->

    <!-- Botões de Controle de Temperatura (Enviam comando para o Climate) -->
    <div class="controls">
      <button class="btn-step" id="btn-minus">
        <ha-icon icon="mdi:minus"></ha-icon>
      </button>
      <button class="btn-step" id="btn-plus">
        <ha-icon icon="mdi:plus"></ha-icon>
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
  </div> <!-- Fecha corretamente a div#ac-core-content -->

  <div class="card-content">
    <div class="div_img_clock">${clockSvg}</div>
    <div class="div_content">
      <span class="timestamp"></span>
    </div>
  </div>

</ha-card>
`;