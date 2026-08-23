export const acCardStyles = `
ha-card.ac-card-root {  
    border: 1px solid var(--primary-color);
    border-radius: 12px;
}

.container {
    display: flex;
    flex-direction: column;
    align-items: center;
    width: 100%;
    font-family: var(--paper-font-body1_-_font-family), -apple-system, sans-serif;           
}

.card-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 0px 5px 3px 5px;  
}

.card-title {
    display: flex;
    align-items: center;
    gap: 8px;
}

.card-icon {
    display: flex;
    align-items: center;
    transition: color 0.2s ease;
    cursor: pointer;
}

.card-title-content {
    font-size: 1.2rem;
    font-weight: 500;
    color: var(--primary-text-color);
}

.title-wrapper {
    display: flex;
    flex-direction: column;
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

/* Botão Discreto de Alternância do Arco no Cabeçalho */
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

/* Seção Retrátil do Arco */
.arc-collapsible-section {
    display: flex;
    flex-direction: column;
    align-items: center;
    width: 100%;
    padding: 4px 0;
}

/* Container do Arco alinhado com as Pontas */
.dial-container {
    position: relative;
    width: 210px;
    height: 210px;
    display: flex;
    justify-content: center;
    align-items: center;
    margin: 4px auto;
}

/* Estilo Base Compartilhado dos Botões nos Extremos */
.btn-dial-extreme {
    position: absolute;
    bottom: 10px;
    background: var(--primary-background-color, #18191a);
    border-radius: 50%;
    width: 38px;
    height: 38px;
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    z-index: 2;
    box-shadow: 0 3px 8px rgba(0, 0, 0, 0.3);
    transition: background 0.2s, transform 0.1s, border-color 0.2s, box-shadow 0.2s;
}

.btn-dial-extreme ha-icon {
    --mdc-icon-size: 20px;
}

/* Botão Esquerdo (- / Frio) - Tons de Azul */
.btn-extreme-left {
    left: 4px;
    border: 2px solid #29b6f6;
    color: #29b6f6;
}

.btn-extreme-left:hover {
    background: #29b6f6;
    color: #ffffff;
    transform: scale(1.1);
    box-shadow: 0 4px 12px rgba(41, 182, 246, 0.4);
}

/* Botão Direito (+ / Calor) - Tons de Laranja/Quente */
.btn-extreme-right {
    right: 4px;
    border: 2px solid #ff9100;
    color: #ff9100;
}

.btn-extreme-right:hover {
    background: #ff9100;
    color: #ffffff;
    transform: scale(1.1);
    box-shadow: 0 4px 12px rgba(255, 145, 0, 0.4);
}

.btn-dial-extreme:active {
    transform: scale(0.95);
}

/* Desenho do Arco SVG */
.dial {
    width: 200px;
    height: 200px;
}

.dial > circle {
    fill: none;
    stroke-width: 8;
    stroke-linecap: round;
}

.dial > circle.bg {
    stroke: var(--disabled-text-color, #f1f2f6);
    stroke-dasharray: 384 534;
    transform: rotate(135deg);
    transform-origin: 100px 100px;
}

.dial > circle.active {
    stroke: var(--primary-color, #2196F3);
    transition: stroke-dashoffset 0.3s ease;
    transform: rotate(135deg);
    transform-origin: 100px 100px;
}

.dial > circle.active {
    transition: stroke-dashoffset 0.3s ease, stroke 0.3s ease;
    transform: rotate(135deg);
    transform-origin: 100px 100px;
}

.dial > circle.handle {
    fill: var(--card-background-color, #ffffff);
    stroke: var(--primary-color, #2196F3);
    stroke-width: 4;
    r: 7;
    transition: cx 0.3s ease, cy 0.3s ease;
}

/* Display Central do Arco (Temperatura + Informações) */
.temp-display {
    position: absolute;
    top: 46%;
    left: 50%;
    transform: translate(-50%, -50%);
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    text-align: center;
    width: 100%;
    pointer-events: none;
}

.temp-num {
    font-size: 44px;
    color: var(--primary-text-color, #2f3542);
    font-weight: 500;
    position: relative;
    display: flex;
    align-items: flex-start;
    line-height: 1;
}

.unit {
    font-size: 18px;
    margin-top: 4px;
    font-weight: 400;
}

/* Informações harmonicamente dentro do arco (Abaixo da temperatura) */
.arc-footer-info {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 8px;
    margin-top: 6px;
    font-size: 0.72rem;
    color: var(--secondary-text-color, #a0a0a0);
}

.arc-info-item {
    display: flex;
    align-items: center;
    gap: 4px;
    text-transform: capitalize;
}

.arc-info-item ha-icon {
    --mdc-icon-size: 13px;
    color: var(--primary-color, #00e676);
}

.arc-divider {
    width: 3px;
    height: 3px;
    border-radius: 50%;
    background: rgba(255, 255, 255, 0.25);
}

/* Painel Compacto (Ordem: Modo | Temperatura | Ventilador) */
.metrics-container {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 8px;
    margin: 8px 12px;
    padding: 6px 4px;
    width: 100%;
    box-sizing: border-box;
}

.metric-block {
    display: flex;
    flex-direction: column;
    align-items: center;
    text-align: center;
}

.metric-value-row {
    display: flex;
    align-items: baseline;
    gap: 2px;
}

.metric-num {
    font-size: 1.25rem;
    font-weight: 700;
    line-height: 1.1;
    color: var(--primary-text-color, #ffffff);
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

.metric-mode .metric-num { color: #ff9100; }
.metric-temp .metric-num { color: var(--primary-color, #00e676); }
.metric-fan .metric-num { color: #29b6f6; }

/* Menu e Rodapé do Card */
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

.card-content {
    display: flex;
    align-items: center;
    gap: 16px;
    padding: 5px;  
    border-top: 1px solid var(--divider-color, #e0e0e0);
}

.div_img_clock {
    display: flex;
    align-items: center;
    color: var(--secondary-text-color);
}

.div_content {
    flex: 1;
    font-size: 0.9rem;
    color: var(--secondary-text-color);
}

.extra-entities {
    margin-top: 1px;
    padding-top: 8px;
    border-top: 1px solid var(--divider-color, #e0e0e0);    
}
`;