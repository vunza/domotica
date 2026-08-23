// timer-modal.js
import { getToken, ip_e_porta } from './globals.js';
let getCardInstance = null;

/**
 * Inicializa o modal de configuração de timers, configurando eventos e estado.
 *
 * @param {Object} cardInstance - Instância do card principal (this).
 * @param {ShadowRoot} shadow - Shadow root do card onde o modal está inserido.
 * @returns {{openModal: function, closeModal: function}} Objeto com funções para controlar o modal.
 */
export function initTimerModal(cardInstance, shadow) {
  getCardInstance = cardInstance;

  const modal = shadow.getElementById('config-timers-container');
  const closeBtn = shadow.getElementById('timer-close-btn');
  const saveBtn = shadow.getElementById('timer-save-btn');
  const deviceNameSpan = shadow.getElementById('timer_device_name');
  const deviceIdInput = shadow.getElementById('timer_save_device_id');
  const horaActualElement = shadow.getElementById('hora_actual');
  const diaSemanaElement = shadow.getElementById('dia_semana_actual');
  const timerSelectors = shadow.querySelectorAll('.timer-selector-input');
  const customCheckbox = shadow.querySelector('.custom-checkbox');
  const chkActivar = shadow.getElementById('chk_activar');

  if (!modal || !closeBtn || !saveBtn) {
    console.error('Elementos do modal de timers não encontrados!');
    return {
      openModal: () => console.warn('Modal não disponível'),
      closeModal: () => {}
    };
  }

  /** @constant {string[]} diasDaSemana - Nomes dos dias da semana em português. */
  const diasDaSemana = [
    'Domingo', 'Segunda-feira', 'Terça-feira', 'Quarta-feira',
    'Quinta-feira', 'Sexta-feira', 'Sábado'
  ];

  /** @type {number|null} intervalId - Identificador do setInterval para atualização da hora. */
  let intervalId = null;

  /**
   * Atualiza o conteúdo dos elementos de hora e dia da semana com a data/hora atual.
   * @private
   */
  const atualizarDataHora = () => {
    const agora = new Date();
    const horas = String(agora.getHours()).padStart(2, '0');
    const minutos = String(agora.getMinutes()).padStart(2, '0');
    const segundos = String(agora.getSeconds()).padStart(2, '0');
    if (horaActualElement) horaActualElement.textContent = `${horas}:${minutos}:${segundos}`;
    
    const diaSemana = agora.getDay();
    if (diaSemanaElement) diaSemanaElement.textContent = diasDaSemana[diaSemana];
  };

  /**
   * Abre o modal de configuração de timers, preenche os dados iniciais e inicia a atualização da hora.
   */
  const openTimerModal = () => {
    // Atualiza o nome do dispositivo
    const deviceName = (cardInstance._customNameProvided && cardInstance._customName)
      ? cardInstance._customName
      : (cardInstance._stateObj ? cardInstance._stateObj.attributes.friendly_name : '') ||
        cardInstance._config.entity.split('.')[1] ||
        'Dispositivo';
    if (deviceNameSpan) deviceNameSpan.textContent = deviceName;
    if (deviceIdInput) deviceIdInput.value = cardInstance._config.entity;

    // Inicia a atualização da hora
    atualizarDataHora();
    
    if (intervalId) clearInterval(intervalId);
    intervalId = setInterval(atualizarDataHora, 1000);
    modal.style.display = 'block';

    // Limpar selecção dos temporizadores
    if (timerSelectors.length > 0) {      
        for (let i = 0; i < timerSelectors.length; i++) { timerSelectors[i].checked = false; }
    }

    // Limpar os campos do formulário ao abrir o modal
    resetarElementos(shadow);

    if (customCheckbox) customCheckbox.setAttribute('aria-checked', 'false');
  };

  /**
   * Fecha o modal de timers, interrompe a atualização da hora e limpa os campos.
   */
  const closeTimerModal = () => {
    // Limpar selecção dos temporizadores
    if (timerSelectors.length > 0) {      
        for (let i = 0; i < timerSelectors.length; i++) { timerSelectors[i].checked = false; }
    }

    // Limpar os campos do formulário ao fechar o modal
    resetarElementos(shadow);
    if (customCheckbox) customCheckbox.setAttribute('aria-checked', 'false');

    modal.style.display = 'none';
    if (intervalId) {
      clearInterval(intervalId);
      intervalId = null;
    }    
  };

  // Limpar os campos do formulário inicialmente
  resetarElementos(shadow);

  if (customCheckbox) customCheckbox.setAttribute('aria-checked', 'false');

  // Configurar eventos do Checkbox customizado
  if (customCheckbox && chkActivar) {
    customCheckbox.addEventListener('click', () => {
        chkActivar.checked = !chkActivar.checked;
        customCheckbox.setAttribute('aria-checked', chkActivar.checked);
    });

    chkActivar.addEventListener('change', () => {
        customCheckbox.setAttribute('aria-checked', chkActivar.checked);
    });
  }

  // Atribui EventListener aos radio buttons "select_timer"
  const radios = shadow.querySelectorAll('input[name="select_timer"]');
  for (const radio of radios) {
      radio.onclick = (e) => {        
        const label_innerHTML = `Activar Temporizador: <span style="color: #41bdf5; font-size: 1.2em; font-weight: bold;">${e.target.value}</span>`;
        const lblActivar = shadow.getElementById('lbl_activar_temporizador');
        if (lblActivar) lblActivar.innerHTML = label_innerHTML;         
      };
  }

  // Event listeners
  closeBtn.addEventListener('click', closeTimerModal);
  
  // Fechar ao clicar fora
  modal.addEventListener('click', (e) => {
    if (e.target === modal) closeTimerModal();    
  });

// Botão Guardar com efeito de salvamento, ícone de disquete e reativação sem fechar o modal
// SVGs dos ícones (Disquete e Spinner de carregamento)
const svgDisquete = `<svg style="width:18px;height:18px;fill:currentColor;vertical-align:middle;margin-right:6px;" viewBox="0 0 24 24"><path d="M15,9H5V5H15M12,19A3,3 0 0,1 9,16A3,3 0 0,1 12,13A3,3 0 0,1 15,16A3,3 0 0,1 12,19M17,3H5C3.89,3 3,3.89 3,5V19A2,2 0 0,0 5,21H19A2,2 0 0,0 21,19V7L17,3Z"/></svg>`;
const svgSpinner = `<svg class="spinner" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"><line x1="12" y1="2" x2="12" y2="6"></line><line x1="12" y1="18" x2="12" y2="22"></line><line x1="4.93" y1="4.93" x2="7.76" y2="7.76"></line><line x1="16.24" y1="16.24" x2="19.07" y2="19.07"></line><line x1="2" y1="12" x2="6" y2="12"></line><line x1="18" y1="12" x2="22" y2="12"></line><line x1="4.93" y1="19.07" x2="7.76" y2="16.24"></line><line x1="16.24" y1="7.76" x2="19.07" y2="4.93"></line></svg>`;

// Botão Guardar: Efeito visual de carregamento com spinner radial e reativação dinâmica
saveBtn.addEventListener('click', async () => {         
  if (saveBtn.disabled) return;

  const textoOriginal = `${svgDisquete} Guardar`;

  // Ativa o estado visual "Salvando..." com spinner e classe CSS
  saveBtn.disabled = true;
  saveBtn.classList.add('loading');
  saveBtn.innerHTML = `${svgSpinner} Salvando...`;

  try {
    const sucesso = await guardarConfigsTimers(shadow, cardInstance);
    
    // Remove o efeito de carregamento antes de exibir o resultado
    saveBtn.classList.remove('loading');

    if (sucesso) {
      saveBtn.innerHTML = `${svgDisquete} Guardado!`;
      
      // Mantém a confirmação por 1 segundo e reativa o botão
      setTimeout(() => {
        saveBtn.innerHTML = textoOriginal;
        saveBtn.disabled = false;
      }, 1000);
    } else {
      saveBtn.innerHTML = textoOriginal;
      saveBtn.disabled = false;
    }
  } catch (error) {
    console.error('Erro ao guardar temporizador:', error);
    saveBtn.classList.remove('loading');
    saveBtn.innerHTML = textoOriginal;
    saveBtn.disabled = false;
  }
});

  // Seleciona todos os elementos com a classe 'timer-selector-number'
  const elementos = shadow.querySelectorAll('.timer-selector-number');
  for (const elemento of elementos) {
    elemento.addEventListener('click', async function() {
      const timer_index = this.innerHTML;
      const id_entidade = shadow.getElementById('timer_save_device_id')?.value;

      if (!id_entidade) return;

      // Obter Token de acesso ao HA
      const api_token = '/local/json_files/token_api.json';
      const token = await getToken(api_token); 
      getAutomationData(token, ip_e_porta, timer_index, id_entidade, shadow);
    });
  }

  return {
    openModal: openTimerModal,
    closeModal: closeTimerModal
  };
}

/**
 * Guarda as configurações do timer no Home Assistant através de chamadas de API.
 * @async
 * @param {ShadowRoot} shadow - Shadow root do card.
 * @param {Object} cardInstance - Instância do GenericCard.
 * @returns {Promise<boolean>} Retorna true se salvou com sucesso, false caso contrário.
 */
async function guardarConfigsTimers(shadow, cardInstance) {    
    const id_entidade = shadow.getElementById('timer_save_device_id')?.value;
    const chkActivar = shadow.getElementById('chk_activar');
    const selectAccao = shadow.getElementById('select_accao');
    const timeInput = shadow.getElementById('time_hora');
    
    let timer_index = null;
    const activado = chkActivar ? chkActivar.checked : false;
    const accao = selectAccao ? selectAccao.value : '';
    const hora = timeInput ? timeInput.value : '';
 
    // Obtém todos os radio buttons
    const radios = shadow.querySelectorAll('.timer-selector-input');
    let selecionado = false;    

    radios.forEach(radio => {
        if (radio.checked) {
            selecionado = true;
            timer_index = radio.value;            
        }
    });
        
    // Validação: temporizador deve ser selecionado
    if (!selecionado) {          
        cardInstance._showToast?.('Por favor, selecione um Temporizador!', '#ff6242');      
        return false;
    }
        
    // Validação: ação deve ser selecionada
    if (!accao) {
        cardInstance._showToast?.('Por favor, selecione uma ação!', '#ff6242');  
        selectAccao?.focus();
        return false;
    }
    
    // Validação: hora deve ser preenchida
    if (!hora) {       
        cardInstance._showToast?.('Por favor, defina uma hora!', '#ff6242');
        timeInput?.focus();
        return false;
    }   

    try {
        // Obter Token de acesso ao HA
        const api_token = '/local/json_files/token_api.json';
        const token = await getToken(api_token);

        // Actualiza input_datetime no HA
        const hora_str = `${hora}:00`;
        await indicarHoraTimer(token, 'input_datetime.horario_automacao', hora_str);

        // Actualiza o input_text com a ação selecionada        
        const dominio = id_entidade.split('.')[0];       
        await indicarAccaoTimer(token, 'input_text.accao_do_timer', dominio, accao);               

        // Atualiza o input_text com o id do dispositivo
        await indicarIdDevice(token, "input_text.identificador_do_dispositivo", `${id_entidade}_${timer_index}`);   
        
        // Altera o estado do input_boolean para activar/desactivar o temporizador        
        await toggleInputBoolean(token, 'input_boolean.activar_desactivar_timer', activado ? 'turn_on' : 'turn_off'); 
        
        // Altera o estado de um input_boolean para activar/desactivar o temporizador nos dias da semana
        const weekDays = [
          "input_boolean.domingo", "input_boolean.segunda", "input_boolean.terca",
          "input_boolean.quarta", "input_boolean.quinta", "input_boolean.sexta",
          "input_boolean.sabado"
        ];
        
        const check_btn = shadow.querySelectorAll('.weekday-checkbox');
        for (let i = 0; i < check_btn.length; i++) {
            const estado = check_btn[i].checked ? 'turn_on' : 'turn_off';
            await toggleInputBoolean(token, weekDays[i], estado); 
        }  
       
        // Configura o temporizador através de um script no HA
        await executeScriptTimer(token, 'gerar_automacao_dinamica');

        // Após guardar as configurações, obter os próximos timers a serem executados
        await getNextTimer2Execute(token, ip_e_porta);   

        // Exibe uma mensagem de sucesso
        cardInstance._showToast?.('Configurações guardadas com sucesso!', '');       
        return true;
    } catch (error) {
        console.error('Erro ao guardar configurações:', error);
        cardInstance._showToast?.(`Falha na atualização: ${error.message}`, '#ff6242');
        return false;
    }
}

/**
 * Executa um shell_command no Home Assistant para obter dados dos próximos timers.
 */
async function getNextTimer2Execute(token, ip_e_porta) {
    const url = `${ip_e_porta}/api/services/shell_command/obter_dados_automacoes`;
    try {
        const response = await fetch(url, {
            method: "POST",
            headers: {
                "Authorization": `Bearer ${token}`,
                "Content-Type": "application/json"
            },
            body: JSON.stringify({})
        });
        if (!response.ok) throw new Error(`Erro: ${response.status}`);
        await response.json();
    } catch (error) {
        console.error("Erro ao executar shell_command:", error);
    }
}

/**
 * Executa um script no Home Assistant.
 */
async function executeScriptTimer(token, script_name) {
    try {
        const response = await fetch('/api/services/script/' + script_name, {
            method: 'POST',
            headers: {
                'Authorization': 'Bearer ' + token,
                'Content-Type': 'application/json'
            }
        });
        if (!response.ok) throw new Error(`HTTP: ${response.status}`);
        return await response.json();
    } catch (error) {
        console.error('Falha ao executar script', error);
        throw error;
    }
}

/**
 * Define a hora em um input_datetime.
 */
async function indicarHoraTimer(token, input_text_id, hora_str) {
    return fetch('/api/services/input_datetime/set_datetime', {
        method: 'POST',
        headers: {
            'Authorization': 'Bearer ' + token,
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            entity_id: input_text_id,
            time: hora_str
        })
    });
}

/**
 * Define a ação do timer em um input_text.
 */
async function indicarAccaoTimer(token, input_text_id, dominio, accao) {
    if (accao !== "turn_on" && accao !== "turn_off" && accao !== "toggle") {
        console.error('Indique uma Acção válida!');
        return;
    }
    const acaoComDominio = `${dominio}.${accao}`;
    return fetch("/api/services/input_text/set_value", {
        method: "POST",
        headers: {
            "Authorization": "Bearer " + token,
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            entity_id: input_text_id,
            value: acaoComDominio
        })
    });
}

/**
 * Define o identificador do dispositivo em um input_text.
 */
async function indicarIdDevice(token, input_text_id, iDentidade) {
    return fetch("/api/services/input_text/set_value", {
        method: "POST",
        headers: {
            "Authorization": "Bearer " + token,
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            entity_id: input_text_id,
            value: iDentidade
        })
    });
}

/**
 * Altera o estado de um input_boolean (turn_on / turn_off).
 */
async function toggleInputBoolean(token, entityId, estado) {
    try {
        const response = await fetch(`/api/services/input_boolean/${estado}`, {
            method: 'POST',
            headers: {
                'Authorization': 'Bearer ' + token,
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ entity_id: entityId })
        });
        if (!response.ok) throw new Error(`Erro HTTP: ${response.status}`);
        return await response.json();
    } catch (error) {
        console.error('Falha ao alternar input_boolean:', error);
        throw error;
    }
}

/**
 * Obtém os dados de uma automação específica e atualiza o formulário.
 */
async function getAutomationData(token, ip_e_porta, timer_index, id_entidade, shadow) {
    const chkActivar = shadow.getElementById('chk_activar');
    const customCheckbox = shadow.querySelector('.custom-checkbox');

    resetarElementos(shadow);

    const id_sem_dominio = id_entidade.split('.')[1];
    const AUTOMATION_ID = `Timer-${id_sem_dominio}_${timer_index}`;
    const url = `${ip_e_porta}/api/config/automation/config/${AUTOMATION_ID}`;

    try {
        const response = await fetch(url, {
            method: "GET",
            headers: {
                "Authorization": `Bearer ${token}`,
                "Content-Type": "application/json"
            }
        });
        const data = await response.json();

        const at = data.triggers[0]?.at || null;
        const weekday = data.conditions[0]?.weekday || null;
        const action = data.actions[0]?.action || null;

        const input = shadow.getElementById("time_hora");
        if (at) {
            input.value = at.substring(0, 5);
        }

        const select = shadow.getElementById("select_accao");
        if (action) {
            const comando = action.split(".")[1];
            for (let i = 0; i < select.options.length; i++) {
                if (select.options[i].value === comando) {
                    select.selectedIndex = i;
                    break;
                }
            }
        }

        if (data.variables?.estado === "true" && AUTOMATION_ID === data.id) {
            if (chkActivar) chkActivar.checked = true;
            if (customCheckbox) customCheckbox.setAttribute('aria-checked', 'true');
        } else if (data.variables?.estado === "false" && AUTOMATION_ID === data.id) {
            if (chkActivar) chkActivar.checked = false;
            if (customCheckbox) customCheckbox.setAttribute('aria-checked', 'false');
        }

        const checBtn = shadow.querySelectorAll('.weekday-checkbox');
        const mapaDias = { sun:0, mon:1, tue:2, wed:3, thu:4, fri:5, sat:6 };
        if (weekday && Array.isArray(weekday)) {
            weekday.forEach(dia => {
                const idx = mapaDias[dia];
                if (idx !== undefined && checBtn[idx]) checBtn[idx].checked = true;
            });
        }
    } catch (error) {
        resetarElementos(shadow);
        console.error("Erro ao buscar a automação:", error);
    }
}

/**
 * Obtém o estado de uma automação (on/off/not_created/error).
 */
async function getAutomationState(ip_e_porta, token, automationId) {    
    const url = `${ip_e_porta}/api/states/automation.${automationId}`;
    try {
        const res = await fetch(url, {
            method: "GET",
            headers: {
                "Authorization": `Bearer ${token}`,
                "Content-Type": "application/json"
            }
        });
        if (res.status === 404) return "not_created";
        const data = await res.json();
        return data.state;
    } catch (err) {
        console.error("Erro ao obter estado:", err);
        return "error";
    }
}

/**
 * Reseta os elementos do formulário para os valores padrão.
 */
function resetarElementos(shadow) {
    const chkActivar = shadow.getElementById('chk_activar');
    const customCheckbox = shadow.querySelector('.custom-checkbox');
    const input = shadow.getElementById("time_hora");
    const select = shadow.getElementById('select_accao');
    const checBtn = shadow.querySelectorAll('.weekday-checkbox');

    if (input) input.value = "";
    if (chkActivar) {
        chkActivar.checked = false;
        if (customCheckbox) customCheckbox.setAttribute('aria-checked', 'false');
    }
    if (select) select.selectedIndex = 0;
    if (checBtn) {
        for (let i = 0; i < checBtn.length; i++) {
            checBtn[i].checked = false;
        }
    }
}

/**
 * Obtém os dados dos timers a partir de um sensor específico do Home Assistant.
 */
async function obterDadosTimers(token, ip_e_porta) {
    const ENTITY_ID = 'sensor.info_dos_timers';

    try {
        const response = await fetch(
            `${ip_e_porta}/api/states/${ENTITY_ID}`,
            {
                headers: {
                    'Authorization': `Bearer ${token}`,
                    'Content-Type': 'application/json'
                }
            }
        );
        
        if (!response.ok) {
            throw new Error(`Erro HTTP: ${response.status}`);
        }
        
        const data = await response.json();
        const timers = data.attributes?.timers;
        
        return timers || null;
    } catch (error) {
        console.error('Erro ao obter dados:', error);
        return null;
    }
}

export { obterDadosTimers, getNextTimer2Execute };