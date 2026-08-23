export const acCardStyles = `

ha-card {  
   border: 1px solid var(--primary-color);  
}

.container {
    display: flex;
    flex-direction: column;
    align-items: center;
    width: 100%;
    font-family: var(--paper-font-body1_-_font-family), -apple-system, sans-serif;           
}

.header-card {
    display: flex;
    justify-content: space-between;
    width: 100%;
    align-items: center;
    margin-bottom: 10px;
}

.title {
    font-weight: 500;
    font-size: 16px;
    color: var(--primary-text-color);
    text-transform: capitalize;
}

.dial-container {
    position: relative;
    width: 210px;
    height: 210px;
    margin-top: -4px;  
}

dial-container > circle {
    padding-bottom: -10px;     
}

.dial {
    width: 100%;
    height: 100%;
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

.dial > circle.handle {
    fill: var(--card-background-color, #ffffff);
    stroke: var(--primary-color, #2196F3);
    stroke-width: 4;
    r: 7;
    transition: cx 0.3s ease, cy 0.3s ease;
}

.temp-display {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
}

.mode-label {
    font-size: 14px;
    color: var(--secondary-text-color, #747d8c);
    font-weight: 500;
    margin-bottom: -2px;
    text-transform: capitalize;
}

.temp-num {
    font-size: 56px;
    color: var(--primary-text-color, #2f3542);
    font-weight: 400;
    position: relative;
    display: flex;
    align-items: flex-start;
}

.unit {
    font-size: 22px;
    margin-top: 10px;
    font-weight: 400;
}

/*.controls {
    display: flex;
    gap: 12px;
    justify-content: center;
    margin: -10px 0 24px 0;
}*/


.controls {
    display: flex;
    gap: 12px;
    justify-content: center;
    margin: -20px 0 15px 0; /* Substitui o '-10px 0 24px 0' por margem simétrica */
}


.btn-step {
    width: 46px;
    height: 46px;
    border-radius: 50%;
    border: 1px solid var(--divider-color, #ced6e0);
    background: var(--card-background-color, #ffffff);
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    color: var(--primary-text-color, #2f3542);
    transition: background 0.2s, transform 0.1s ease, border-color 0.2s;
}

.btn-step:active {
    transform: scale(0.94);
}

/* --- CORES ESPECÍFICAS DOS BOTÕES --- */

/* Botão Power (Vermelho) */
.btn-power {
    color: var(--error-color, #ff4757);
    border-color: rgba(255, 71, 87, 0.4);
    background: rgba(255, 71, 87, 0.08);
}

.btn-power:hover {
    background: rgba(255, 71, 87, 0.18);
    border-color: var(--error-color, #ff4757);
}

/* Botão Diminuir Temp (Frio / Azul) */
#btn-minus {
    color: var(--info-color, #1e90ff);
    border-color: rgba(30, 144, 255, 0.4);
    background: rgba(30, 144, 255, 0.08);
}

#btn-minus:hover {
    background: rgba(30, 144, 255, 0.18);
    border-color: var(--info-color, #1e90ff);
}

/* Botão Aumentar Temp (Quente / Laranja) */
#btn-plus {
    color: var(--warning-color, #ff6b81);
    border-color: rgba(255, 107, 129, 0.4);
    background: rgba(255, 107, 129, 0.08);
}

#btn-plus:hover {
    background: rgba(255, 107, 129, 0.18);
    border-color: var(--warning-color, #ff6b81);
}

/* Botão Ocultar / Visualizar Arco (Neutro / Utilitário) */
.btn-arc-toggle {
    color: var(--secondary-text-color, #747d8c);
    border-color: var(--divider-color, #ced6e0);
    background: var(--card-background-color, #ffffff);
}

.btn-arc-toggle:hover {
    color: var(--primary-text-color, #2f3542);
    background: var(--secondary-background-color, #f1f2f6);
}

.pill-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 16px;
    width: 100%;
}

.pill-button {
    background: var(--secondary-background-color, #f1f2f6);
    border-radius: 18px;
    padding: 12px 16px;
    display: flex;
    align-items: center;
    gap: 12px;
    cursor: pointer;
    width: 100%;
    box-sizing: border-box;
}

.pill-text {
    display: flex;
    flex-direction: column;
}

.pill-title {
    font-size: 12px;
    color: var(--secondary-text-color, #747d8c);
}

.pill-value {
    font-size: 14px;
    color: var(--primary-text-color, #2f3542);
    font-weight: 600;
    text-transform: capitalize;
}

ha-icon {
    color: var(--primary-text-color, #2f3542);
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

.card-content {
    display: flex;
    align-items: center;
    gap: 16px;
    padding: 5px;  
    border-top: 1px solid var(--divider-color, #e0e0e0);
}

/* ENTIDADES EXTRAS */
.extra-entities {
    margin-top: 1px;
    padding-top: 8px;
    border-top: 1px solid var(--divider-color, #e0e0e0);    
}

.extra-grid {
    display: grid;
    grid-template-columns: 1fr;
    gap: 8px;
}

.extra-item {
    display: flex;
    justify-content: space-between;
    align-items: baseline;
    font-size: 0.85rem;
    padding: 4px 8px;
    background: var(--card-background-color, white);
    border-radius: 8px;
}

.extra-name {
    color: var(--secondary-text-color, #5f6368);
    font-weight: 500;
}

.extra-value {
    font-weight: bold;
    color: var(--primary-text-color, #202124);
}

`;