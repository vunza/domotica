
// Variável para armazenar o token durante a sessão
let id_entidade = null;
let estado_entidade = null;
let nome_entidade = null;
let accao_timer = null;
let authToken = '';
var timer_index = 0;
const max_char_div_name = 26;
const ENTITY_ID = 'input_datetime.horario_automacao';
const tokenInput = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJjMDUwYTVkNTMzZDU0ZjBmODJlY2NjZmQyZDgwZDRlYyIsImlhdCI6MTczNjcxNzU1NSwiZXhwIjoyMDUyMDc3NTU1fQ.oLIiyFN8c4lPCAgIPL_sFEWI12P3IcsLsid4KL8A2D0';
const http_prot = window.location.protocol; // Guarda http: OU https:
const accoes = {
    ligar:"Ligar",
    desligar:"Desligar",
    inverter:"Inverter"
};

/////////////////////////
// Navegador carregado //
/////////////////////////
document.addEventListener("DOMContentLoaded", function(e) {

});



/////////////////////////
// Navegador carregado //
/////////////////////////
document.onreadystatechange = function () {

     // Verifica se o documento está completamente carregado
    const stylesheets = document.querySelectorAll('link[rel="stylesheet"]');
    let loadedSheets = 0;
    
    stylesheets.forEach(sheet => {
        sheet.onload = function() {
            loadedSheets++;
            if (loadedSheets === stylesheets.length) {
                // Todos os CSS carregados
                showPage();                
            }
        };
        
        // Para folhas de estilo já carregadas antes do evento
        if (sheet.sheet) {
            loadedSheets++;
            if (loadedSheets === stylesheets.length) {
                showPage();
            }
        }
    });


    if (document.readyState == "complete") {
        
        authenticate();
                
        // Pega o valor de 'entity_id' e 'entity_name' da URL
        const params = new URLSearchParams(window.location.search);
        const entityId = params.get('entity_id');
        const entityName = params.get('name');

        if (entityId) {
            id_entidade = entityId;
        } 

        if (entityName) {
            nome_entidade = entityName;
        } 

        document.getElementById('txt_nome_dispositivo_aux').value = nome_entidade;
        document.getElementById('div_nome_dispositivo').textContent = nome_entidade;
        

        // Eveto Listener para opcao selecionada
        document.getElementById('action-select').addEventListener('change', function() {
            var e = document.getElementById("action-select");
            accao_timer = e.options[e.selectedIndex].text;                     
        });

        // Preencher opcao select
        const slt = document.getElementById('action-select');       
        Object.entries(accoes).forEach(([key, value]) => {
            let newOption = document.createElement("option");
            newOption.text = `${value}`;
            slt.appendChild(newOption);
        }); 


        // Inicializar input time
        const agora = new Date();
        const horas = String(agora.getHours()).padStart(2, '0');
        const minutos = String(agora.getMinutes()).padStart(2, '0');
        const input = document.getElementById('time_hora');
        input.value = `${horas}:${minutos}`;

        // Desabilita edição na DIV para alterar nome de entidades
        document.getElementById('div_nome_dispositivo').contentEditable = false;
        document.getElementById('div_nome_dispositivo').style.backgroundColor = 'lightgray';

        
        // Liitar o numero de caracteres na DIV para alterar nome das entidades
        const div = document.getElementById('div_nome_dispositivo')
        div.addEventListener('input', () => {
            if (div.innerText.length > max_char_div_name) {
              div.innerText = div.innerText.substring(0, max_char_div_name);
              // Move o cursor para o final
              const range = document.createRange();
              const sel = window.getSelection();
              range.selectNodeContents(div);
              range.collapse(false);
              sel.removeAllRanges();
              sel.addRange(range);
            }            
        });

        // Obter Hora actual
        obter_data_hora();
        
    }// Fim if

}// Fim onreadystatechange


//////////////////////////////////////////////////////////////
// Exibir a página após o carregamento completo dos estilos //
//////////////////////////////////////////////////////////////
async function showPage() {
    document.getElementById("div_container").style.display = "block";    
}


////////////////////////////
// Função para autenticar //
////////////////////////////
function authenticate() {
       
    if (!tokenInput) {
        showTooltip('Por favor, insira um token válido', 'error');
        return;
    }
    
    authToken = tokenInput;
    document.getElementById('authPanel').classList.add('hidden');
    document.getElementById('controlPanel').classList.remove('hidden');
    
}




/////////////////////////////////////////////
// Função principal para definir o horário //
/////////////////////////////////////////////
async function setTime() { 
    
    // Validar Temporizador
    if(timer_index === 0){
        showTooltip('Indique um Temporizador!', 'error');    
        return; 
    }
    
    seleccionaDiaSemana();    
  
    const hora = document.getElementById("time_hora").value;   
    
    // Validação
    if (hora.length !== 5) {       
        showTooltip('Indique a Hora desejada', 'error');   
        return;
    }

      
    try {
        
        // Actualiza card datetime
        const timeStr = `${hora}:00`;
        const response = await fetch('/api/services/input_datetime/set_datetime', {
            method: 'POST',
            headers: {
                'Authorization': 'Bearer ' + authToken,
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                entity_id: ENTITY_ID,
                time: timeStr
            })
        });
        
        if (!response.ok) {
            const errorData = await response.json();
            throw new Error(errorData.message || 'Erro desconhecido');
        }        
    
        
        
        // Atualiza o input_text com a ação
        const dominio = id_entidade.split('.')[0]; 
        
        if(accao_timer == null || accao_timer == undefined || accao_timer == ''){            
            var sel = document.getElementById("action-select");
            accao_timer = sel.options[sel.selectedIndex].text;            
        }

        

        if(accao_timer == accoes.ligar){
            indicarAccaoTimer('input_text.accao_do_timer', `${dominio}.turn_on`); 
        }
        else if(accao_timer == accoes.desligar){
            indicarAccaoTimer('input_text.accao_do_timer', `${dominio}.turn_off`);    
        }
        else if(accao_timer == accoes.inverter){
            indicarAccaoTimer('input_text.accao_do_timer', `${dominio}.toggle`);         
        }
        else{
            showTooltip('Indique uma Acção!', 'error'); 
            return; 
        }
        

        // Atualiza o input_text com o id do dispositivo
        indicarIdDevice("input_text.identificador_do_dispositivo", `${id_entidade}_${timer_index}`);

        // Executa o script de automação
        executeScriptTimer('gerar_automacao_dinamica'); 
        
        showTooltip(`Horário atualizado para ${hora}`, 'success'); 

        // Actualizar dados dos Timers nas cards
        actualizarDadosTimersCards(`${http_prot}//${location.host}`);
              
        
    } catch (error) {        
        showTooltip(`Falha na atualização: ${error.message}`, 'error');
        
        console.error('Erro:', error);
        
        // Se erro de autenticação, volta para o painel de login
        if (error.message.includes('401') || error.message.includes('autenticação')) {
            authToken = '';
            document.getElementById('authPanel').classList.remove('hidden');
            document.getElementById('controlPanel').classList.add('hidden');
        }
    }
}


//////////////////////////////
// Mostra mensagens Tooltip //
//////////////////////////////
function showTooltip(mensagem, type = '') {
    const span_tooltip = document.getElementById('span_tooltip');
    const btn_tooltip = document.getElementById('btn_salvar_timer');
    
    span_tooltip.style.visibility = 'visible'; 
    btn_tooltip.style.visibility = 'visible'; 
    span_tooltip.style.opacity = '1';
    btn_tooltip.style.opacity = '1'; 
        
    span_tooltip.innerHTML = mensagem;

    span_tooltip.className = 'status tooltiptext ' + type;
    
    // Configura o tempo para fechar automaticamente (3 segundos)
    setTimeout(() => {
        span_tooltip.style.visibility = 'hidden'; 
        
        //tooltip.style.display = 'none';        
    }, 5000);
}







////////////////////////////////////////////////////////
// Script para Criar/Actualizar Automação/Agendamento //
////////////////////////////////////////////////////////
async function executeScriptTimer(scriptName) {
    try {
      const response = await fetch('/api/services/script/' + scriptName, {
        method: 'POST',
        headers: {
          'Authorization': 'Bearer ' + tokenInput,
          'Content-Type': 'application/json'
        }
      });
  
      if (!response.ok) {
        throw new Error(`Erro HTTP: ${response.status}`);
      }
  
      //console.log('Script executado com sucesso!');
      return await response.json();
    } catch (error) {
      console.error('Falha ao executar script:', error);
      throw error;
    }
}



///////////////////////////////
// Obter Nomes das Entidades //
//////////////////////////////
async function getAllFriendlyNames() {
    const response = await fetch('/api/states', {
        headers: {
            'Authorization': 'Bearer ' + tokenInput
        }
    });
    
    if (!response.ok) {
        throw new Error('Falha ao obter entidades');
    }
    
    const entities = await response.json();
    const names = {};
    
    entities.forEach(entity => {
        names[entity.entity_id] = 
            entity.attributes.friendly_name || 
            entity.entity_id.split('.').pop().replace(/_/g, ' ');
    });
    
    return names;
}


/////////////////////////////////////////
// Altera o estado de um input_boolean //
/////////////////////////////////////////  
async function toggleInputBoolean(entityId, estado) {
    try {
      const response = await fetch(`/api/services/input_boolean/${estado}`, {
        method: 'POST',
        headers: {
          'Authorization': 'Bearer ' + tokenInput,
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


//////////////////////////////
// Seleccina dias de semana //
//////////////////////////////
function seleccionaDiaSemana() {
    let estado = "";
    const weekDays = [];
    weekDays.push("input_boolean.domingo","input_boolean.segunda", "input_boolean.terca");
    weekDays.push("input_boolean.quarta","input_boolean.quinta", "input_boolean.sexta");
    weekDays.push("input_boolean.sabado");

    var checBtn = document.getElementsByName("sel_week_day");
    for (let i = 0; i < checBtn.length; i++) {
        checBtn[i].checked ? estado = 'turn_on' : estado = 'turn_off';
        toggleInputBoolean(weekDays[i], estado); 
    }     
}


///////////////////////////////
// Indicar id do dispositivo //
///////////////////////////////
async function indicarIdDevice(input_text_id, iDentidade) {
    await fetch("/api/services/input_text/set_value", {
        method: "POST",
        headers: {
          "Authorization": "Bearer " + tokenInput,
          "Content-Type": "application/json"
        },
        body: JSON.stringify({
          entity_id: input_text_id, 
          value: iDentidade
        })
    });
}


///////////////////////////////
// Indicar id do dispositivo //
///////////////////////////////
async function indicarAccaoTimer(input_text_id, accao) {
    await fetch("/api/services/input_text/set_value", {
        method: "POST",
        headers: {
          "Authorization": "Bearer " + tokenInput,
          "Content-Type": "application/json"
        },
        body: JSON.stringify({
          entity_id: input_text_id, 
          value: accao
        })
    });
}



//////////////////////////////////
// Função para seleccionar tabs //
//////////////////////////////////
function obterDadosAutomacoes(elemento) {
    // Declare all variables
    var i, tablinks;    
  
    // Get all elements with class="tablinks" and remove the class "active"
    tablinks = document.getElementsByClassName("tablinks");
    for (i = 0; i < tablinks.length; i++) {
      tablinks[i].className = tablinks[i].className.replace(" active", "");
    }
  
    // Show the current tab, and add an "active" class to the button that opened the tab
    document.getElementById(elemento.id).style.display = "block";    
    document.getElementById(elemento.id).classList.add('active');

    if(elemento.id == 'btn_timer_1'){
        timer_index = 1;
    }
    else if(elemento.id == 'btn_timer_2'){
        timer_index = 2;
    }
    else if(elemento.id == 'btn_timer_3'){
        timer_index = 3;
    }

    // Retirar seleccao dos dias da semana
    var chckList = document.getElementsByName('sel_week_day');
    for (var i = 0; i < chckList.length; i++) { chckList[i].checked = false; }

    // Obter dados dos Timers
    getAutomationData(`${http_prot}//${location.host}`);
     
} 


////////////////////////////
// Obter dados dos Timers //
////////////////////////////
async function getAutomationData(ip_porta) {
    
    const HA_TOKEN =  tokenInput;

    //(*J*) Baseado no ID da automação, no 'rest_command:' do arquivo configuration.yaml
    const id_sem_dominio = id_entidade.split('.')[1]; 
    const AUTOMATION_ID = `Timer-${id_sem_dominio}_${timer_index}`;   

    const url = `${ip_porta}/api/config/automation/config/${AUTOMATION_ID}`;

    fetch(url, {
    method: "GET",
    headers: {
        "Authorization": `Bearer ${tokenInput}`,
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
        const select = document.getElementById("action-select");     
        const comando = action.split(".")[1];  
        
        if( comando == 'turn_on'){            
            document.getElementById('action-select').selectedIndex = 1; // Ligar
        }
        else if( comando == 'turn_off'){            
            document.getElementById('action-select').selectedIndex = 2; // Desligar
        }
        else if( comando == 'toggle'){            
            document.getElementById('action-select').selectedIndex = 3; // Inverter
        }
        else{
            document.getElementById('action-select').selectedIndex = 0; // Nenhum
        }
        

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



////////////////////////////
// Actualiza FriendlyName //
////////////////////////////
async function actualizaFriendlyName(ip_porta, entityId, friendlyName, entityState) {

    const token = tokenInput;  
   
    const url = `${ip_porta}/api/services/shell_command/atualiza_friendly`;

    fetch(url, {
    method: "POST",
    headers: {
        "Authorization": `Bearer ${token}`,
        "Content-Type": "application/json"
    },
    body: JSON.stringify({
        entity_id: entityId,
        friendly_name: friendlyName,
        entity_state: entityState
    })
    })
    .then(response => {
    if (!response.ok) {
        throw new Error("Erro na requisição");
    }
    return response.json();
    })
    .then(data => {
    console.log("Comando enviado com sucesso:", data);
    })
    .catch(error => {
    console.error("Erro:", error);
    });

}


//////////////////////////////////////////
// Actualiza dados dos Timers das Cards //
//////////////////////////////////////////
async function actualizarDadosTimersCards(ip_porta) {

    const token = tokenInput;  
   
    const url = `${ip_porta}/api/services/shell_command/obter_dados_automacoes`;

    fetch(url, {
        method: "POST",
        headers: {
          "Authorization": `Bearer ${token}`,
          "Content-Type": "application/json"
        },
        body: JSON.stringify({}) // se o comando precisar de parâmetros, coloque aqui
      })
      .then(response => {
        if (!response.ok) {
          throw new Error(`Erro: ${response.status}`);
        }
        return response.json();
      })
      .then(data => {
        console.log("Comando executado:", data);
      })
      .catch(error => {
        console.error("Erro ao executar shell_command:", error);
      });      

}
  


//////////////////////////////////
// Obter estado de uma entidade //
//////////////////////////////////
async function obterEsatdoEntidade(ip_porta, entityId) {
    const token = tokenInput;  
    const url = `${ip_porta}/api/states/${entityId}`;
    
    try {
        const response = await fetch(url, {
            method: "GET",
            headers: {
                "Authorization": "Bearer " + token,
                "Content-Type": "application/json"
            }
        });
        const data = await response.json();
        estado_entidade = data.state;
        console.log("Estado da entidade:", estado_entidade);
        console.log("Atributos:", data.attributes);
        return estado_entidade; // Retorna o valor
    } catch (error) {
        console.error("Erro ao obter estado:", error);
        throw error; // Propaga o erro
    }
}
  


/////////////////////////
// Limpar os elementos //
/////////////////////////
function resetarElementos(){
    // Resetar input time
    const input = document.getElementById("time_hora");           
    input.value = "";

    // Resetar option select
    document.getElementById('action-select').selectedIndex = 0;

    // Resetar dias da semana    
    var checBtn = document.getElementsByName("sel_week_day");
    for (let i = 0; i < checBtn.length; i++) {
        checBtn[i].checked = false;;
    }
}


//////////////////////////////////////////////////////////////
// Verifica se o Browser Mod está carregado e fecha o popup //
//////////////////////////////////////////////////////////////
function closeBrowserModPopup() {
   
    if (window.browser_mod?.closePopup) {
        window.browser_mod.closePopup();
    } else if (window.parent.browser_mod?.closePopup) {
        window.parent.browser_mod.closePopup();
    } else {
        console.error("Browser Mod não encontrado!");
        // Fallback: Chamar o serviço via Home Assistant
        hass.callService("browser_mod", "close_popup", {});
    }
}

///////////////////////////////////////////
// Activar edição do nome do dispositivo //
///////////////////////////////////////////
function enableDeviceNameEdiction() {
    const input = document.getElementById('div_nome_dispositivo');    
    input.style.border = '2px solid green'; 
    document.getElementById('div_nome_dispositivo').contentEditable = true;
    document.getElementById('div_nome_dispositivo').style.backgroundColor = 'white'; 
    //input.focus();  
    var html_data = `&nbsp;&nbsp;${input.innerText}&nbsp;&nbsp;`;
    document.getElementById("legenda_fieldset_renomear").innerHTML = html_data; 
}

/////////////////////////////
// Checar tag "input/txt" //
////////////////////////////
function changeTxtHandler(Id) {   
    const saved_name = document.getElementById('txt_nome_dispositivo_aux').value;
    const new_name = document.getElementById('div_nome_dispositivo').textContent;    

   if (new_name.trim() !== saved_name.trim()) {
     document.getElementById("div_edit_name").style.display = "none";
     document.getElementById("div_save_name").style.display = "block";
   } else {
     document.getElementById("div_edit_name").style.display = "block";
     document.getElementById("div_save_name").style.display = "none";
   }
    
 }


 
////////////////////////////////////// 
// Salvar novo nome do didspositivo //
//////////////////////////////////////
async function saveDeviceName(){
    const new_name = (document.getElementById('div_nome_dispositivo').innerText).trim();
   
    const estado = await obterEsatdoEntidade(`${http_prot}//${location.host}`, id_entidade);           
        
    actualizaFriendlyName(`${http_prot}//${location.host}`, id_entidade, new_name, estado);

    document.getElementById('div_nome_dispositivo').contentEditable = false;
    document.getElementById('div_nome_dispositivo').style.backgroundColor = 'lightgray'; 

    document.getElementById("div_edit_name").style.display = "block";
    document.getElementById("div_save_name").style.display = "none";
    document.getElementById("div_nome_dispositivo").disabled = true;
    document.getElementById('txt_nome_dispositivo_aux').value = new_name;
    showTooltip(`Nome alterado: ${new_name}`, 'success');
}


///////////////////////////////////////////////////////
// Exibir Data/Hora no Dlg de configurações de Timer //
///////////////////////////////////////////////////////
function obter_data_hora() {
    //var strcount  
    const weekday = ["Domingo","Segunda-feira","Terça-feira","Quarta-feira","Quinta-feira","Sexta-feira","Sabado"];
    var currentdate = new Date();
	//var formato = '<font color="black" face="Verdana, Geneva, sans-serif" size="+1">' + currentdate.toDateString() + '</font>';
	var formato = `<font color="white" face="Verdana, Geneva, sans-serif" size="+1">${weekday[currentdate.getDay()]}</font>`;
    formato += '<font color="yellow" face="Verdana, Geneva, sans-serif" size="+1">';	
	
    var formato1 = `<font color="blue" face="Verdana, Geneva, sans-serif" size="+1">${weekday[currentdate.getDay()]}</font>`;
    formato1 += '<font color="black" face="Verdana, Geneva, sans-serif" size="+1">';	
		

	var hours = currentdate.getHours();
	var minutes = currentdate.getMinutes();
	var seconds = currentdate.getSeconds();
	
	if (hours < 10) hours = "0" + hours;
	if (minutes < 10) minutes = "0" + minutes;``
	if (seconds < 10) seconds = "0" + seconds;

	formato += " " + hours + ":" + minutes + ":" + seconds;	
	formato += "</font>";

    var html_data = `&nbsp;&nbsp;${formato}&nbsp;&nbsp;`;
    document.getElementById("legenda_fieldset_automatizacoes").innerHTML = html_data;   
    tt = display_c(); 

} // Fim de obter_data_hora()


///////////////////////////////////////////////////////
// Exibir Data/Hora no Dlg de configurações de Timer //
///////////////////////////////////////////////////////
function display_c(){   
    var refresh = 1000; 
    mytime = setTimeout('obter_data_hora()', refresh);    
}


/////////////////////////////////////////////////////
// Exibe Menu contextual ao clicar nos tres pontos //
/////////////////////////////////////////////////////
function mostrarMenuContextual(){ 
          
    const menu = document.querySelector('#div_menu_contextual');  
    window.addEventListener('click', event => menu.style.display = 'none');
    // show the menu when right-clicked
    window.addEventListener('contextmenu', event => {
        event.preventDefault();
        menu.style.position = 'absolute';
        menu.style.top = (window.scrollY + 0) + 'px';       
        menu.style.display = 'block';    
    });
}