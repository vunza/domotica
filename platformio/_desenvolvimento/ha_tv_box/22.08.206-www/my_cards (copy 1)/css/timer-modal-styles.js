export const timerModalStyles = `
  .config-timers-container {
    /*position: fixed;
    top: 65px;
    width: 90%;
    max-width: 380px;
    background: #ffffff;
    color: #333;
    border-radius: 12px;
    box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);    
    border: 1px solid var(--primary-color);
    overflow: hidden; 
    left: auto;  
    marin: auto;     
    z-index: 10000;*/   


    position: fixed;
    top: 65px;
    left: 0;
    right: 0;
    margin-left: auto;
    margin-right: auto;
    width: 90%;
    max-width: 380px;
    background: #ffffff;
    color: #333;
    border-radius: 12px;
    box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);
    border: 1px solid var(--primary-color);
    overflow: hidden;
    z-index: 10000;
  }

  .config-timers-header {
    padding: 8px 10px;
    text-align: center;
    border-bottom: 1px solid #ddd;
    background: #f5f5f5;
  }

  .config-timers-header h2 {
    margin: 0;
    font-size: 1.2rem;
    font-weight: 600;
    color: #333;
  }

  .timer-selector-container {
    padding: 0 20px 4px 20px;
    border-bottom: 1px solid #ddd;
    background: #fafafa;
  }

  .timer-selector-grid {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 8px;
    padding: 4px 0;
  }

  .timer-selector-label {
    display: flex;
    flex-direction: column;
    align-items: center;
    cursor: pointer;
  }

  .timer-selector-input {
    display: none;
  }

  .timer-selector-number {
    width: 40px;
    height: 40px;
    border-radius: 50%;
    border: 2px solid #ccc;
    display: flex;
    align-items: center;
    justify-content: center;
    font-weight: bold;
    font-size: 1.1rem;
    transition: all 0.2s ease;
    background: #f0f0f0;
    color: #333;
  }

  .timer-selector-input:checked + .timer-selector-number {
    background: #41bdf5;
    border-color: #41bdf5;
    color: white;
    transform: scale(1.1);
    box-shadow: 0 0 10px rgba(65, 189, 245, 0.5);
  }

  .timer-config-section {
    padding: 12px 20px;
  }

  .config-form {
    display: flex;
    flex-direction: column;
    gap: 12px;
  }

  .config-row {
    display: flex;
    flex-direction: column;
    gap: 4px;
  }

  .config-label {
    font-weight: 600;
    font-size: 0.9rem;
    color: #555;
    margin-bottom: 2px;
  }

  .checkbox-container {
    display: flex;
    align-items: center;
    gap: 10px;
  }

  .custom-checkbox {
    width: 24px;
    height: 24px;
    border: 2px solid #ccc;
    border-radius: 6px;
    background: #f9f9f9;
    cursor: pointer;
    position: relative;
    transition: all 0.2s ease;
    flex-shrink: 0;
  }

  .custom-checkbox::after {
    content: '';
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%) scale(0);
    width: 14px;
    height: 14px;
    background: #39be5a;
    border-radius: 3px;
    transition: all 0.2s ease;
  }

  #chk_activar:checked + .custom-checkbox {
    border-color: #39be5a;
  }

  #chk_activar:checked + .custom-checkbox::after {
    transform: translate(-50%, -50%) scale(1);
  }

  .select-container {
    position: relative;
    width: 100%;
  }

  .custom-select {
    width: 100%;
    padding: 10px 12px;
    padding-left: 40px;
    border: 2px solid #ccc;
    border-radius: 8px;
    background: #f9f9f9;
    color: #333;
    font-size: 0.95rem;
    transition: all 0.2s ease;
    appearance: none;
    cursor: pointer;
    min-height: 42px;
    text-align: center;
  }

  .custom-select option[value=""][disabled] {
    color: #999;
    font-style: italic;
  }

  .custom-select:invalid {
    color: #999;
  }

  .select-icon {
    position: absolute;
    padding-top: 5px;
    left: 14px;
    top: 50%;
    transform: translateY(-50%);
    color: #666;
    pointer-events: none;
  }

  .select-icon svg {
    width: 18px;
    height: 18px;
  }

  .custom-select option {
    text-align: left;
    color: #333;
  }

  .opcao_accao {
    color: #333;
  }

  .time-input-container {
    position: relative;
    width: 100%;
  }

  .time-input {
    width: auto;
    padding: 5px 12px;
    padding-left: 40px;
    border: 2px solid #ccc;
    border-radius: 8px;
    background: #f9f9f9;
    color: #333;
    font-size: 0.95rem;
    transition: all 0.2s ease;
    min-height: 25px;
    text-align: center;
    cursor: pointer;
    -webkit-appearance: none;
    -moz-appearance: textfield;
    appearance: none;
  }

  .time-input::-webkit-calendar-picker-indicator {
    display: none;
  }

  .time-input::-webkit-inner-spin-button,
  .time-input::-webkit-outer-spin-button {
    -webkit-appearance: none;
    margin: 0;
  }

  .time-input::placeholder {
    color: #999;
    opacity: 1;
  }

  .time-icon {
    position: absolute;
    left: 14px;
    padding-top: 5px;
    top: 50%;
    transform: translateY(-50%);
    color: #666;
    pointer-events: none;
  }

  .time-icon svg {
    width: 18px;
    height: 18px;
  }

  .custom-select:focus,
  .time-input:focus {
    outline: none;
    border-color: #41bdf5;
    background-color: #e6f7ff;
  }

  .weekdays-container {
    margin-top: 12px;
  }

  .weekdays-label {
    display: block;
    margin-bottom: 8px;
    font-weight: 600;
    font-size: 0.9rem;
    color: #555;
    cursor: pointer;
  }

  .weekdays-grid {
    display: grid;
    grid-template-columns: repeat(7, 1fr);
    gap: 4px;
  }

  .weekday-label {
    display: flex;
    flex-direction: column;
    align-items: center;
    cursor: pointer;
  }

  .weekday-checkbox {
    display: none;
  }

  .weekday-letter {
    width: 32px;
    height: 32px;
    border-radius: 50%;
    border: 2px solid #ccc;
    display: flex;
    align-items: center;
    justify-content: center;
    font-weight: bold;
    font-size: 0.9rem;
    margin-bottom: 2px;
    transition: all 0.2s ease;
    background: #f0f0f0;
    color: #333;
  }

  .weekday-checkbox:checked + .weekday-letter {
    background: #41bdf5;
    border-color: #41bdf5;
    color: white;
    transform: scale(1.1);
  }

  .weekday-name {
    font-size: 0.65rem;
    color: #666;
    text-transform: uppercase;
    text-align: center;
    white-space: nowrap;
  }

  .current-time-date {
    text-align: center;
    margin-top: 16px;
    padding: 8px;
    background: #f5f5f5;
    border-radius: 8px;
    border: 1px solid #ddd;
  }

  .current-time {
    font-size: 1rem;
    margin-bottom: 1px;
    color: #333;
  }

  #hora_actual {
    color: #ff9800;
    font-weight: 700;
    font-size: 1.1rem;
  }

  #dia_semana_actual {
    color: #41bdf5;
    font-weight: 600;
  }

  .timers-toolbar {
    padding: 8px 20px;
    background: #f5f5f5;
    border-top: 1px solid #ddd;
    display: flex;
    justify-content: center;
    gap: 16px;
  }

  .tool-btn {
    min-width: 100px;
    padding: 10px 16px;
    border: none;
    border-radius: 8px;
    font-size: 0.95rem;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s ease;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 8px;
  }

  .tool-btn svg {
    width: 18px;
    height: 18px;
  }

  .btn-save {
    background: #39be5a;
    color: white;
  }

  .btn-save:hover {
    background: #2e8b57;
    transform: translateY(-2px);
  }

  .btn-close {
    background: #f44336;
    color: white;
  }

  .btn-close:hover {
    background: #d32f2f;
    transform: translateY(-2px);
  }

  @media (max-width: 480px) {
    body {
      padding: 15px;
    }
    
    .config-timers-container {
      max-width: 90%;
    }
    
    .timer-selector-number {
      width: 36px;
      height: 36px;
      font-size: 1rem;
    }
    
    .weekday-letter {
      width: 28px;
      height: 28px;
      font-size: 0.85rem;
    }
    
    .tool-btn {
      min-width: 90px;
      padding: 8px 12px;
      font-size: 0.9rem;
    }
  }

  @media (max-width: 320px) {
    .tool-btn {
      min-width: 80px;
      padding: 8px 10px;
      font-size: 0.85rem;
    }
    
    .weekday-letter {
      width: 24px;
      height: 24px;
    }
    
    .custom-select,
    .time-input {
      padding-left: 36px;
      font-size: 0.9rem;
    }
    
    .select-icon,
    .time-icon {
      left: 10px;
    }
  }



  /************* Toast Notification *****************/
  .toast {
      position: fixed;        /* fixo na viewport, não rola com a página */
      top: 50%;               /* posiciona o canto superior esquerdo no centro */
      left: 50%;
      transform: translate(-50%, -50%); /* desloca o elemento para que seu centro fique no centro da tela */
      background: var(--primary-color);
      color: white;
      padding: 15px 25px;
      border-radius: 10px;
      box-shadow: 0 5px 15px rgba(0, 0, 0, 0.2);
      z-index: 10001;
      opacity: 0;
      transition: opacity 0.3s ease;    /* apenas opacidade para um fade simples */
      /* se quiser um efeito de entrada com escala, use:
      transform: translate(-50%, -50%) scale(0.9);
      transition: opacity 0.3s, transform 0.3s;
      */
  }

  .toast.show {
      opacity: 1;
      /* se usar escala, adicione também:
      transform: translate(-50%, -50%) scale(1);
      */
  }


`;