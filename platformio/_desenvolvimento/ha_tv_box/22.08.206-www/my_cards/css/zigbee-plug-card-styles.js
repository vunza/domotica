export const plugCardStyles = `
  .card {
    background: var(--card-background-color, #1c1c1e);
    color: var(--primary-text-color, #ffffff);
    border-radius: 16px;
    box-shadow: var(--ha-card-box-shadow, 0 4px 12px rgba(0,0,0,0.3));
    font-family: var(--primary-font-family, Roboto, sans-serif);
    padding: 12px;
    box-sizing: border-box;
    position: relative;
    overflow: visible;
    border: 1px solid var(--primary-color);
  }
  .card-header {
    display: flex;
    justify-content: space-between;
    align-items: center;   
    margin-bottom: 12px;
  }
  .card-title {
    display: flex;
    align-items: center;
    gap: 10px;
  }
  .title-wrapper {
    display: flex;
    flex-direction: column;
  }
  .card-title-content {
    font-size: 1.15rem;
    font-weight: 600;
  }
  .status-badges {
    display: flex;
    align-items: center;
    gap: 6px;
  }
  .zigbee-badge {
    font-size: 0.72rem;
    color: var(--secondary-text-color, #a0a0a0);
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
  .card-icon {
    cursor: pointer;
    transition: transform 0.1s ease;
  }
  .card-icon:active { transform: scale(0.9); }

  .menu-container {
    position: relative;
    display: inline-block;
  }
  .menu-trigger {
    background: none;
    border: none;
    color: var(--secondary-text-color, #a0a0a0);
    cursor: pointer;
    padding: 4px;
    border-radius: 50%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .menu-trigger:hover {
    background: rgba(255, 255, 255, 0.08);
  }

  .metrics-container {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 8px;
    margin-bottom: 14px;
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
    font-size: 1.55rem;
    font-weight: 700;
    line-height: 1.1;
  }
  .metric-unit {
    font-size: 0.85rem;
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
  }
  .chart-svg {
    width: 100%;
    height: 110px;
    overflow: visible;
  }
  .chart-svg path, .chart-svg line {
    pointer-events: none;
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

  .card-content {
    display: flex;
    align-items: center;
    gap: 10px;
    margin-top: 8px;
    padding-top: 8px;
    border-top: 1px solid var(--divider-color, rgba(255, 255, 255, 0.1));
    font-size: 0.78rem;
    color: var(--secondary-text-color, #8e8e93);
  }

  /* Botão Discreto de Alternância do Gráfico */
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

`;