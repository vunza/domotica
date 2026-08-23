export const remoteControlCardStyles = `
  :host {
    display: block;
    margin: 4px;
  }
  ha-card {
    background: var(--ha-card-background, var(--card-background-color, white));
    border-radius: var(--ha-card-border-radius, 16px);
    box-shadow: var(--ha-card-box-shadow, none);
    padding: 16px;
    font-family: var(--paper-font-body1_-_font-family, sans-serif);
    transition: all 0.3s ease;
    border: 1px solid var(--primary-color);  
  }
  .header {
    text-align: center;
    margin-bottom: 16px;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 4px;
  }
  
  /* Dynamic Combo Box Styling */
  .room-dropdown {
    width: 80%;
    max-width: 220px;
    padding: 8px 12px;
    font-size: 16px;
    font-weight: bold;
    border-radius: 8px;
    border: 1px solid var(--divider-color, #e0e0e0);
    background-color: var(--card-background-color, #ffffff);
    color: var(--primary-text-color);
    cursor: pointer;
    outline: none;
    text-align: center;
    text-align-last: center;
  }
  .room-dropdown:focus {
    border-color: var(--accent-color, #ff9800);
  }

  .subtitle {
    font-size: 13px;
    color: var(--secondary-text-color);
    text-transform: capitalize;
  }
  
  .mode-selector {
    display: grid;
    grid-template-columns: repeat(5, 1fr);
    gap: 6px;
    margin-bottom: 20px;
  }
  
  @media (max-width: 380px) {
    .mode-selector {
      grid-template-columns: repeat(5, minmax(50px, 1fr));
      overflow-x: auto;
      padding-bottom: 4px;
    }
  }

  .mode-btn {
    background: var(--secondary-background-color, #f4f5f7);
    color: var(--primary-text-color);
    border: none;
    padding: 10px 4px;
    border-radius: 10px;
    cursor: pointer;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 4px;
    font-size: 11px;
    transition: all 0.2s ease;
  }
  .mode-btn.active {
    background: var(--accent-color, #ff9800);
    color: white;
  }
  .mode-btn ha-icon {
    --mdc-icon-size: 20px;
  }

  .controls-grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 16px;
    justify-items: center;
    align-items: center;
    max-width: 280px;
    margin: 0 auto;
  }

  .btn {
    width: 60px;
    height: 60px;
    border-radius: 50%;
    border: none;
    cursor: pointer;
    display: flex;
    justify-content: center;
    align-items: center;
    color: white;
    box-shadow: 0 3px 6px rgba(0,0,0,0.12);
    transition: transform 0.1s ease, opacity 0.2s;
  }
  .btn:active { transform: scale(0.90); }
  .btn ha-icon { --mdc-icon-size: 26px; }
  
  .spacer {
    width: 60px;
    height: 60px;
    visibility: hidden;
  }
  
  .btn-power   { background-color: var(--error-color, #db4437); }
  .btn-action  { background-color: var(--primary-color, #03a9f4); }
  .btn-neutral { background-color: #616161; }
  .btn-accent  { background-color: var(--success-color, #4caf50); }
  .btn-orange  { background-color: #ff9800; }
`;