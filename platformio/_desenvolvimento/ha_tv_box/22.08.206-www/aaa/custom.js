// ==========================================
// 1. SECCIÓN DE ESTILOS (CSS)
// ==========================================
const cardStyles = `
  .card-content {
    color: green;
    font-family: sans-serif;
    padding: 16px;
  }
  .btn-saludar {
    background-color: var(--primary-color, #03a9f4);
    color: var(--text-primary-color, #ffffff);
    border: none;
    padding: 10px 20px;
    font-size: 14px;
    font-weight: bold;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
  }
  .btn-saludar:hover {
    opacity: 0.9;
  }
  .btn-saludar:active {
    transform: scale(0.98);
  }
`;

// ==========================================
// 2. SECCIÓN DE PLANTILLA (HTML)
// ==========================================
const cardTemplate = `
  <div class="card-content">
    <h1>saludos</h1>
    <p>esto es un saludo</p>
    <button id="saludar" class="btn-saludar">Saludar</button>
  </div>
`;

// ==========================================
// 3. LÓGICA DE LA TARJETA (JAVASCRIPT)
// ==========================================
class Saludos extends HTMLElement {
  setConfig(config) {
    this._config = config;
  }

  connectedCallback() {
    // Unimos el CSS y el HTML en el DOM de la tarjeta
    this.innerHTML = `
      <style>${cardStyles}</style>
      ${cardTemplate}
    `;

    this._bindEvents();
  }

  // Mapeo de eventos aislados de la renderización
  _bindEvents() {
    const boton = this.querySelector('#saludar');
    if (boton) {
      boton.addEventListener('click', () => this._manejarClick());
    }
  }

  _manejarClick() {
    alert("¡Hola desde Home Assistant!");
  }

  getCardSize() {
    return 1;
  }
}

customElements.define("saludos-card", Saludos);



