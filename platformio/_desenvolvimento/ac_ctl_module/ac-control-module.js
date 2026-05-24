class ModuloControloAC extends HTMLElement {
  constructor() {
    super();
    // Cria o Shadow DOM para isolar o componente
    this.attachShadow({ mode: "open" });

    // Configurações padrão
    this.minTemp = 16;
    this.maxTemp = 30;
    this.currentTemp = 25;
    this.maxStroke = 400;

    this.modes = ["Cool", "Heat", "Dry", "Fan"];
    this.modeIndex = 0;

    this.fanModes = ["Auto", "Low", "Medium", "High"];
    this.fanIndex = 0;
  }

  // Define quais atributos HTML monitorar
  static get observedAttributes() {
    return ["temperatura"];
  }

  // Executado quando um atributo monitorado muda
  attributeChangedCallback(name, oldValue, newValue) {
    if (name === "temperatura" && oldValue !== newValue) {
      const val = parseInt(newValue, 10);
      if (!isNaN(val) && val >= this.minTemp && val <= this.maxTemp) {
        this.currentTemp = val;
        this.updateThermostat();
      }
    }
  }

  // Executado quando o componente é inserido na página
  connectedCallback() {
    // Se houver atributo de temperatura inicial no HTML, usa ele
    if (this.hasAttribute("temperatura")) {
      this.currentTemp = parseInt(this.getAttribute("temperatura"), 10) || 25;
    }

    this.shadowRoot.innerHTML = `
            <style>
                :host {
                    display: inline-block;
                    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
                    --bg-color: #ffffff;
                    --text-color: #1a1a1a;
                    --gray-light: #f0f0f0;
                    --gray-text: #666666;
                    --accent-blue: #2196f3;
                    user-select: none;
                }

                .thermostat-container {
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    width: 350px;
                    padding: 20px;
                    background-color: var(--bg-color);
                }

                .dial-area {
                    position: relative;
                    width: 280px;
                    height: 280px;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                }

                .svg-ring {
                    position: absolute;
                    transform: rotate(-90deg);
                }

                .info-center {
                    text-align: center;
                    z-index: 2;
                }

                .mode-text {
                    font-size: 16px;
                    font-weight: 500;
                    color: var(--text-color);
                    margin-bottom: 5px;
                }

                .temp-display {
                    font-size: 64px;
                    font-weight: 400;
                    position: relative;
                    display: inline-block;
                    line-height: 1;
                }

                .unit {
                    font-size: 20px;
                    position: absolute;
                    top: 8px;
                    right: -24px;
                }

                .controls-row {
                    display: flex;
                    gap: 25px;
                    margin-top: 10px;
                    margin-bottom: 35px;
                }

                .btn-adjust {
                    width: 50px;
                    height: 50px;
                    border-radius: 50%;
                    border: 1px solid #ccc;
                    background-color: transparent;
                    font-size: 28px;
                    font-weight: 300;
                    cursor: pointer;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    transition: background-color 0.2s, transform 0.1s;
                }

                .btn-adjust:active {
                    background-color: var(--gray-light);
                    transform: scale(0.95);
                }

                .footer-modes {
                    display: flex;
                    gap: 15px;
                    width: 100%;
                }

                .mode-card {
                    flex: 1;
                    background-color: var(--gray-light);
                    border-radius: 12px;
                    padding: 12px 15px;
                    display: flex;
                    align-items: center;
                    gap: 12px;
                    cursor: pointer;
                }

                .icon-box {
                    font-size: 20px;
                }

                .card-label {
                    font-size: 11px;
                    color: var(--gray-text);
                    margin: 0;
                    text-transform: uppercase;
                    letter-spacing: 0.5px;
                }

                .card-value {
                    font-size: 14px;
                    font-weight: 500;
                    color: var(--text-color);
                    margin: 2px 0 0 0;
                }
            </style>

            <div class="thermostat-container">
                <div class="dial-area">
                    <svg class="svg-ring" width="280" height="280" viewBox="0 0 200 200">
                        <circle cx="100" cy="100" r="85" fill="none" stroke="#f0f0f0" stroke-width="10" 
                                stroke-dasharray="400 534" stroke-linecap="round" transform="rotate(-30 100 100)"/>
                        <circle id="progress-bar" cx="100" cy="100" r="85" fill="none" stroke="#2196f3" stroke-width="10" 
                                stroke-dasharray="0 534" stroke-linecap="round" transform="rotate(-30 100 100)" 
                                style="transition: stroke-dasharray 0.3s ease;"/>
                    </svg>

                    <div class="info-center">
                        <div class="mode-text" id="current-mode-text">Cool</div>
                        <div class="temp-display">
                            <span id="temp-val">${this.currentTemp}</span><span class="unit">°C</span>
                        </div>
                    </div>
                </div>

                <div class="controls-row">
                    <button class="btn-adjust" id="btn-minus">−</button>
                    <button class="btn-adjust" id="btn-plus">+</button>
                </div>

                <div class="footer-modes">
                    <div class="mode-card" id="card-mode">
                        <div class="icon-box">❄️</div>
                        <div>
                            <p class="card-label">Mode</p>
                            <p class="card-value" id="mode-val">Cool</p>
                        </div>
                    </div>
                    
                    <div class="mode-card" id="card-fan">
                        <div class="icon-box">🌀</div>
                        <div>
                            <p class="card-label">Fan mode</p>
                            <p class="card-value" id="fan-val">Auto</p>
                        </div>
                    </div>
                </div>
            </div>
        `;

    this.initElements();
    this.addEventListeners();
    this.updateThermostat();
  }

  initElements() {
    this.tempValEl = this.shadowRoot.getElementById("temp-val");
    this.progressBar = this.shadowRoot.getElementById("progress-bar");
    this.btnMinus = this.shadowRoot.getElementById("btn-minus");
    this.btnPlus = this.shadowRoot.getElementById("btn-plus");
    this.cardMode = this.shadowRoot.getElementById("card-mode");
    this.cardFan = this.shadowRoot.getElementById("card-fan");
    this.modeValEl = this.shadowRoot.getElementById("mode-val");
    this.fanValEl = this.shadowRoot.getElementById("fan-val");
    this.currentModeText = this.shadowRoot.getElementById("current-mode-text");
  }

  addEventListeners() {
    this.btnMinus.addEventListener("click", () => {
      if (this.currentTemp > this.minTemp) {
        this.currentTemp--;
        this.updateThermostat();
        this.dispatchChangeEvent();
      }
    });

    this.btnPlus.addEventListener("click", () => {
      if (this.currentTemp < this.maxTemp) {
        this.currentTemp++;
        this.updateThermostat();
        this.dispatchChangeEvent();
      }
    });

    this.cardMode.addEventListener("click", () => {
      this.modeIndex = (this.modeIndex + 1) % this.modes.length;
      const selectedMode = this.modes[this.modeIndex];
      this.modeValEl.textContent = selectedMode;
      this.currentModeText.textContent = selectedMode;

      const iconEl = this.cardMode.querySelector(".icon-box");
      if (selectedMode === "Cool") iconEl.textContent = "❄️";
      else if (selectedMode === "Heat") iconEl.textContent = "☀️";
      else if (selectedMode === "Dry") iconEl.textContent = "💧";
      else if (selectedMode === "Fan") iconEl.textContent = "💨";

      this.dispatchChangeEvent();
    });

    this.cardFan.addEventListener("click", () => {
      this.fanIndex = ((fanIndex) => (fanIndex + 1) % this.fanModes.length)(
        this.fanIndex,
      );
      this.fanValEl.textContent = this.fanModes[this.fanIndex];
      this.dispatchChangeEvent();
    });
  }

  updateThermostat() {
    if (!this.tempValEl) return;
    this.tempValEl.textContent = this.currentTemp;
    const percentage =
      (this.currentTemp - this.minTemp) / (this.maxTemp - this.minTemp);
    const strokeValue = percentage * this.maxStroke;
    this.progressBar.style.strokeDasharray = `${strokeValue} 534`;
  }
  // Dispara um evento personalizado para que a aplicação externa saiba quando houve mudança
  dispatchChangeEvent() {
    this.dispatchEvent(
      new CustomEvent("change", {
        detail: {
          temperature: this.currentTemp,
          mode: this.modes[this.modeIndex],
          fanMode: this.fanModes[this.fanIndex],
        },
        bubbles: true,
        composed: true,
      }),
    );
  }
}
// Registra a tag HTML customizada
customElements.define("ac-control-module", ModuloControloAC);
