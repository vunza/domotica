// Estilos específicos para o Card de Luz Zigbee
export const lightCardStyles = `
  .card {
    background: var(--card-background-color, white);
    color: var(--primary-text-color);
    border-radius: 12px;
    box-shadow: var(--ha-card-box-shadow, 0 2px 2px 0 rgba(0,0,0,0.14), 0 1px 5px 0 rgba(0,0,0,0.12));
    font-family: var(--primary-font-family, Roboto);
    border: 1px solid var(--primary-color);  
  }
  .card-header {
    display: flex;
    justify-content: space-between;
    align-items: center;   
    padding: 8px 8px 4px 8px;
  }
  .card-title {
    display: flex;
    align-items: center;
    gap: 8px;
  }
  .title-wrapper {
    display: flex;
    flex-direction: column;
  }
  .card-icon {
    display: flex;
    align-items: center;
    transition: color 0.2s ease, transform 0.1s ease;
    cursor: pointer;
  }
  .card-icon:active {
    transform: scale(0.92);
  }
  .card-title-content {
    font-size: 1.1rem;
    font-weight: 500;
  }
  .zigbee-badge {
    font-size: 0.7rem;
    color: var(--secondary-text-color, #757575);
  }
  .menu-container {
    position: relative;
    display: inline-block;
  }
  .menu-trigger {
    background: none;
    border: none;
    padding: 4px;
    cursor: pointer;
    color: var(--secondary-text-color);
    border-radius: 4px;
  }
  .menu-trigger:hover {
    background: var(--divider-color, #e0e0e0);
  }

  /* Slider de Brilho */
  .brightness-container {
    display: flex;
    align-items: center;
    gap: 10px;
    padding: 6px 12px;
    background: var(--secondary-background-color, rgba(0,0,0,0.03));
    border-top: 1px dashed var(--divider-color, #e0e0e0);
    transition: opacity 0.3s ease;
  }
  .brightness-container.hidden {
    display: none;
  }
  .brightness-icon {
    --mdc-icon-size: 18px;
    color: var(--secondary-text-color);
  }
  .brightness-slider {
    flex: 1;
    -webkit-appearance: none;
    height: 6px;
    border-radius: 3px;
    background: var(--divider-color, #ccc);
    outline: none;
  }
  .brightness-slider::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 16px;
    height: 16px;
    border-radius: 50%;
    background: var(--primary-color, #03a9f4);
    cursor: pointer;
  }
  .brightness-value {
    font-size: 0.8rem;
    font-weight: bold;
    width: 36px;
    text-align: right;
  }

  .card-content {
    display: flex;
    align-items: center;
    gap: 16px;
    padding: 6px;  
    border-top: 1px solid var(--divider-color, #e0e0e0);  
  }
  .div_img_clock {
    display: flex;
    align-items: center;
    color: var(--secondary-text-color);   
  }
  .div_content {
    flex: 1;
    font-size: 0.85rem;
    color: var(--secondary-text-color);   
  }

  .extra-entities {
    margin-top: 1px;
  }
  .extra-grid {
    display: grid;
    grid-template-columns: 1fr;
    gap: 6px;
    border-top: 1px solid var(--divider-color, #e0e0e0);
    padding: 6px;
  }
  .extra-item {
    display: flex;
    justify-content: space-between;
    align-items: center;
    font-size: 0.82rem;
    padding: 3px 6px;
    background: var(--card-background-color, white);
    border-radius: 6px;
  }
  .extra-name {
    color: var(--secondary-text-color, #5f6368);
  }
  .extra-value {
    font-weight: 600;
  }


  .status-badges {
    display: flex;
    align-items: center;
    gap: 6px;
  }
  .status-state-badge {
    font-size: 0.7rem;
    font-weight: 700;
    padding: 1px 6px;
    border-radius: 4px;
    background: rgba(255, 255, 255, 0.08);
    color: #8e8e93;
    text-transform: uppercase;
  }
  .status-state-badge.on {
    background: rgba(0, 230, 118, 0.15);
    color: #00e676;
  }
  .chart-toggle-btn {
    background: none;
    border: none;
    color: var(--secondary-text-color, #a0a0a0);
    cursor: pointer;
    padding: 6px;
    border-radius: 50%;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: background 0.2s ease, color 0.2s ease;
  }
  .chart-toggle-btn:hover {
    background: rgba(255, 255, 255, 0.08);
    color: var(--primary-text-color, #ffffff);
  }
  .chart-toggle-btn.active {
    color: var(--primary-color, #00e676);
    background: rgba(0, 230, 118, 0.1);
  }
  .chart-toggle-btn ha-icon {
    --mdc-icon-size: 20px;
  }
  
  /* Métricas e Gráficos */
  .metrics-container {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 8px;
    margin: 8px 12px;
    padding: 0 4px;
  }
  .metric-block {
    display: flex;
    flex-direction: column;
  }
  .metric-value-row {
    display: flex;
    align-items: baseline;
    gap: 3px;
  }
  .metric-num {
    font-size: 1.35rem;
    font-weight: 700;
    line-height: 1.1;
  }
  .metric-unit {
    font-size: 0.75rem;
    font-weight: 500;
    color: var(--secondary-text-color, #aaaaaa);
  }
  .metric-label {
    font-size: 0.72rem;
    color: var(--secondary-text-color, #8e8e93);
    margin-top: 2px;
  }
  .metric-power .metric-num { color: #00e676; }
  .metric-voltage .metric-num { color: #ff9100; }
  .metric-current .metric-num { color: #29b6f6; }

  .chart-box {
    position: relative;
    width: 100%;
    background: rgba(0, 0, 0, 0.15);
    border-radius: 8px;
    padding: 8px 0 2px 0;
    margin-top: 8px;
  }
  .chart-svg {
    width: 100%;
    height: 110px;
    overflow: visible;
  }
  .grid-line {
    stroke: var(--divider-color, rgba(255, 255, 255, 0.12));
    stroke-dasharray: 4, 4;
    stroke-width: 1;
  }
  .time-axis {
    display: flex;
    justify-content: space-between;
    padding: 4px 8px 0 8px;
    font-size: 0.68rem;
    color: var(--secondary-text-color, #8e8e93);
  }

`;