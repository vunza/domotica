const horaActualElement = document.getElementById('hora_actual');
const diaSemanaElement = document.getElementById('dia_semana_actual');
const timeInput = document.getElementById('time_hora');
const customCheckbox = document.querySelector('.custom-checkbox');
const chkActivar = document.getElementById('chk_activar');
const timerSelectors = document.querySelectorAll('.timer-selector-input');
const weekdayCheckboxes = document.querySelectorAll('.weekday-checkbox');
const weekdaysLabel = document.getElementById('weekdays-label');
const selectAccao = document.getElementById('select_accao');
const diasDaSemana = [
    'Domingo', 'Segunda-feira', 'Terça-feira', 'Quarta-feira',
    'Quinta-feira', 'Sexta-feira', 'Sábado'
];

import {getToken, ip_e_porta} from './vars_funcs_globais.js';

function atualizarDataHora() {
    const agora = new Date();
    
    // Atualizar hora
    const horas = String(agora.getHours()).padStart(2, '0');
    const minutos = String(agora.getMinutes()).padStart(2, '0');
    const segundos = String(agora.getSeconds()).padStart(2, '0');
    horaActualElement.textContent = `${horas}:${minutos}:${segundos}`;
    
    // Atualizar dia da semana
    const diaSemana = agora.getDay();
    diaSemanaElement.textContent = diasDaSemana[diaSemana];
}




//function guardarConfigsTimers() {
window.guardarConfigsTimers = async function() {    
    
    const id_entidade = document.getElementById('timer_save_device_id').value;
    let timer_index = null;
    const activado = chkActivar.checked;
    const accao = selectAccao.value;
    const hora = timeInput.value;
 
    // Obtém todos os radio buttons
    const radios = document.querySelectorAll('.timer-selector-input');
    let selecionado = false;    

    // Verifica cada um
    radios.forEach(radio => {
        if (radio.checked) {
            selecionado = true;
            timer_index = radio.value;            
        }
    });
        

    // Validação: ação deve ser selecionada
    if (!selecionado) {
        alert('Por favor, selecione um Teporizador!');        
        return;
    }
        
    // Validação: ação deve ser selecionada
    if (!accao) {
        alert('Por favor, selecione uma ação!');
        selectAccao.focus();
        return;
    }
    
    // Validação: hora deve ser preenchida
    if (!hora) {
        alert('Por favor, defina uma hora!');
        timeInput.focus();
        return;
    }   

    
    try {

        // Obter Token de acesso ao HA
        const api_token = '/local/json_files/token_api.json';
        const token = await getToken(api_token);


        // Actualiza input_datetime no HA
        const hora_str = `${hora}:00`;
        indicarHoraTimer(token, 'input_datetime.horario_automacao', hora_str);

        // Actualiza o input_text com a ação selecionada        
        const dominio = id_entidade.split('.')[0];       
        indicarAccaoTimer(token, 'input_text.accao_do_timer', dominio, accao);               

        // Atualiza o input_text com o id do dispositivo
        indicarIdDevice(token, "input_text.identificador_do_dispositivo", `${id_entidade}_${timer_index}`);   
        
        // Altera o estado do input_boolean para activar/desactivar o temporizador
        toggleInputBoolean(token, 'input_boolean.activar_desactivar_timer', activado ? 'turn_on' : 'turn_off'); 
        
        // Altera o estado de um input_boolean para activar/desactivar o temporizador
        let estado = "";
        const weekDays = [];
        weekDays.push("input_boolean.domingo","input_boolean.segunda", "input_boolean.terca");
        weekDays.push("input_boolean.quarta","input_boolean.quinta", "input_boolean.sexta");
        weekDays.push("input_boolean.sabado");

        const check_btn = document.getElementsByName("sel_week_day");
        for (let i = 0; i < check_btn.length; i++) {
            check_btn[i].checked ? estado = 'turn_on' : estado = 'turn_off';
            toggleInputBoolean(token, weekDays[i], estado); 
        }  

        
        
        // Configura o temporizador atraves de um script no HA (gerar_automacao_dinamica), 
        // que lê os valores dos input_datetime, input_text e input_boolean para criar/actualizar a automação correspondente
        executeScriptTimer(token, 'gerar_automacao_dinamica').then((data) => {
            //console.log('Resposta do script:', data);
            //showTooltip('Configurações do Temporizador guardadas com sucesso!', 'success'); 
            //alert('Configurações do Temporizador guardadas com sucesso!');            

            // Obter dados dos Timers atualizados e trabalhar com eles (ex: actualizar a interface, mostrar uma mensagem, etc.)
            obterDadosTimers(token, ip_e_porta).then(timers => {
                if (timers) {
                    // Trabalhar com os dados
                    console.log('Dados obtidos:', timers);
                }
            });

        }).catch((error) => {
            console.error('Erro ao executar o script:', error);
        });               

    } catch (error) {
        //showTooltip(`Falha na atualização: ${error.message}`, 'error');        
        console.error('Erro:', error);      
    }
   
} // Fim guardarConfigsTimers(...)




// Obter dados dos Timers da automação no HA, actualiza os elementos do formulário
async function getAutomationData(token, ip_e_porta, timer_index, id_entidade) { 
    
    resetarElementos();

    //(*J*) Baseado no ID da automação, no 'rest_command:' do arquivo configuration.yaml
    const id_sem_dominio = id_entidade.split('.')[1]; 
    const AUTOMATION_ID = `Timer-${id_sem_dominio}_${timer_index}`;   

    const url = `${ip_e_porta}/api/config/automation/config/${AUTOMATION_ID}`;

    fetch(url, {
    method: "GET",
    headers: {
        "Authorization": `Bearer ${token}`,
        "Content-Type": "application/json"
    }
    }).then(response => response.json()).then(data => {           
                
        const at = data.triggers[0]?.at || null;
        const weekday = data.conditions[0]?.weekday || null;
        const action = data.actions[0]?.action || null;                
       
        // Actualizar input time
        const input = document.getElementById("time_hora");
        // Como o input time só aceita "HH:MM", precisamos cortar os segundos
        const horarioSemSegundos = at.substring(0,5);
        input.value = horarioSemSegundos;


        // Actualizar selsct option accao
        const select = document.getElementById("select_accao");     
        const comando = action.split(".")[1];  
        
        for (let i = 0; i < select.options.length; i++) {            
            if (select.options[i].value === comando) {
                select.selectedIndex = i; // Define o índice do select
                break;
            }
        }

        if (comando !== "turn_on" && comando !== "turn_off" && comando !== "toggle"  ){
            select.selectedIndex = 0; // Nenhum
        } 
        
        // Actualizar estado do checkbox activar/desactivar
        const automationId = `timer_${id_sem_dominio}_${timer_index}`; ;
        getAutomationState(ip_e_porta, token, automationId).then(state => {
            if (state === "on") {
                chkActivar.checked = true;
                customCheckbox.setAttribute('aria-checked', 'true');
            } else {
                chkActivar.checked = false;
                customCheckbox.setAttribute('aria-checked', 'false');
            }
        });
               

        // Actualizar dias da semana
        var checBtn = document.getElementsByName("sel_week_day");
        // ["sun", "mon", "tue", "wed", "thu", "fri", "sat"];
        weekday.forEach(dia => {

            if(dia == 'sun'){
                checBtn[0].checked = true;
            }
            else if(dia == 'mon'){
                checBtn[1].checked = true;
            }
            else if(dia == 'tue'){
                checBtn[2].checked = true;
            }
            else if(dia == 'wed'){
                checBtn[3].checked = true;
            }
            else if(dia == 'thu'){
                checBtn[4].checked = true;
            }
            else if(dia == 'fri'){
                checBtn[5].checked = true;
            }
            else if(dia == 'sat'){
                checBtn[6].checked = true;
            }
        });
    })
    .catch(error => {
        resetarElementos();
        console.error("Erro ao buscar a automação:", error);
    });
}


// Obter estado da automação (on/off/not_created/error)
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

        // Não existe ainda
        if (res.status === 404) {
            return "not_created";
        }

        const data = await res.json();
        return data.state; // "on" | "off"

    } catch (err) {
        console.error("Erro ao obter estado:", err);
        return "error";
    }
}



// Função para obter os dados do sensor
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
        
        // O estado principal contém informações básicas
        console.log('Estado:', data.state);
        
        // Os atributos dinâmicos estão em data.attributes.timers
        const timers = data.attributes.timers;
        
        if (timers) {
            console.log('Timers encontrados:');
            Object.keys(timers).forEach(entityId => {
                console.log(`Entity: ${entityId}`, timers[entityId]);
            });
            
            // Exemplo de uso específico
            return timers;
        }
        
        return null;
    } catch (error) {
        console.error('Erro ao obter dados:', error);
        return null;
    }
}



// Função para executar um script no HA (ex: gerar_automacao_dinamica)
async function executeScriptTimer(token, script_name) {
    try {
      const response = await fetch('/api/services/script/' + script_name, {
        method: 'POST',
        headers: {
          'Authorization': 'Bearer ' + token,
          'Content-Type': 'application/json'
        }
      });
  
      if (!response.ok) {
        throw new Error(`HTTP: ${response.status}`);
      }
  
      //console.log('Script executado com sucesso!');
      return await response.json();
    } catch (error) {
      console.error('Falha ao executar script', error);
      throw error;
    }
}



async function indicarHoraTimer(token, input_text_id, hora_str) {
    const response = await fetch('/api/services/input_datetime/set_datetime', {
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


// Indicar id do dispositivo e acção ao input_text
async function indicarAccaoTimer(token, input_text_id, dominio, accao) {

    if(accao == "turn_on" || accao == "turn_off" || accao == "toggle"){
        accao = `${dominio}.${accao}`;
    } else{
        //showTooltip('Indique uma Acção!', 'error'); 
        console.error('Indique uma Acção!');
        return; 
    }

    await fetch("/api/services/input_text/set_value", {
        method: "POST",
        headers: {
          "Authorization": "Bearer " + token,
          "Content-Type": "application/json"
        },
        body: JSON.stringify({
          entity_id: input_text_id, 
          value: accao
        })
    });
}


async function indicarIdDevice(token, input_text_id, iDentidade) {
    await fetch("/api/services/input_text/set_value", {
        method: "POST",
        headers: {
          "Authorization": "Bearer " + token,
          "Content-Type": "application/json"
        },
        body: JSON.stringify({
          entity_id: input_text_id, 
          value: iDentidade // Exemplo: "light.lampada_sala_1"
        })
    });
}


async function toggleInputBoolean(token, entityId, estado) {
    try {
      const response = await fetch(`/api/services/input_boolean/${estado}`, {
        method: 'POST',
        headers: {
          'Authorization': 'Bearer ' + token,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          entity_id: entityId
        })
      });
  
      if (!response.ok) {
        throw new Error(`Erro HTTP: ${response.status}`);
      }
  
      //console.log('input_boolean alternado com sucesso!');
      return await response.json();
    } catch (error) {
      console.error('Falha ao alternar input_boolean:', error);
      throw error;
    }
}


// Limpar os elementos do formulário
function resetarElementos(){
    // Resetar input time
    const input = document.getElementById("time_hora");           
    input.value = "";

    // Resetar checkbox activar/desactivar
    chkActivar.checked = false;
    customCheckbox.setAttribute('aria-checked', 'false');

    // Resetar option select
    document.getElementById('select_accao').selectedIndex = 0;

    // Resetar dias da semana    
    var checBtn = document.getElementsByName("sel_week_day");
    for (let i = 0; i < checBtn.length; i++) {
        checBtn[i].checked = false;;
    }
}


// Configurar eventos
customCheckbox.addEventListener('click', () => {
    chkActivar.checked = !chkActivar.checked;
    customCheckbox.setAttribute('aria-checked', chkActivar.checked);
});

chkActivar.addEventListener('change', () => {
    customCheckbox.setAttribute('aria-checked', chkActivar.checked);
});



// Selecionar todos os dias com duplo clique no label
weekdaysLabel.addEventListener('dblclick', () => {
    const todosMarcados = Array.from(weekdayCheckboxes).every(cb => cb.checked);
    weekdayCheckboxes.forEach(checkbox => {
        checkbox.checked = !todosMarcados;
    });
});

// Limpar a marca d'água quando o select for clicado
selectAccao.addEventListener('focus', function() {
    if (this.value === "") {
        // Opcional: alterar a cor quando focado
        this.style.color = 'white';
    }
});

// Quando uma opção for selecionada, garantir que não é a placeholder
selectAccao.addEventListener('change', function() {
    if (this.value !== "") {
        this.style.color = 'white';
    } else {
        this.style.color = 'rgba(255, 255, 255, 0.5)';
    }
});

// Atribui "Eventlistaner" aos radios buttons "select_timer".    
const radios = document.querySelectorAll('input[name="select_timer"]');
for (const radio of radios) {
    radio.onclick = (e) => {
       
        const label_innerHTML = `Activar Temporizador: <span style="color: #41bdf5; font-size: 1.2em; font-weight: bold;">${e.target.value}</span>`;
        document.getElementById('lbl_activar_temporizador').innerHTML = label_innerHTML;
        

        //obtemConfigTimer(save_dev_id, e.target.value);
    }
}

// Inicializar
atualizarDataHora();
setInterval(atualizarDataHora, 1000);

// Limpar selecção dos temporizadores
if (timerSelectors.length > 0) {
    //timerSelectors[0].checked = true;   
    for (var i = 0; i < timerSelectors.length; i++) { timerSelectors[i].checked = false; }
}

customCheckbox.setAttribute('aria-checked', 'false');

// Inicializar o select com a cor da marca d'água
selectAccao.style.color = 'rgba(255, 255, 255, 0.5)';


window.fecharConfigTimers = function () {
    document.getElementById('cards_main_wrapper').style.display = 'block';
    document.getElementById('config-timers-container').style.display = 'none';   
    window.location.href = 'index.html';   
}


// Seleciona todos os elementos com a classe 'timer-selector-number'
const elementos = document.querySelectorAll('.timer-selector-number');

// Para cada elemento, adiciona um event listener de clique
elementos.forEach(elemento => {
  elemento.addEventListener('click', async function() {

    const timer_index = this.innerHTML;
    const id_entidade = document.getElementById('timer_save_device_id').value;

    // Obter Token de acesso ao HA
    const api_token = '/local/json_files/token_api.json';
    const token = await getToken(api_token); 
    getAutomationData(token, ip_e_porta, timer_index, id_entidade);
  });

});

//export {  };