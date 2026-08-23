export const remoteControlcardTemplate = `
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