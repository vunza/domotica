// Template do card principal (inclui placeholders para título, SVG do relógio e menu)
export const cardTemplate = (initialTitle, clockSvg, menuHtml) => `
  <div class="card">
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

    <div class="card-content">
      <div class="div_img_clock">${clockSvg}</div>
      <div class="div_content">
        <span class="timestamp"></span>
      </div>
    </div>
    <!-- NOVA ÁREA PARA SENSORES -->
    <!-- <div class="extra-entities"></div> -->
  </div>
`;