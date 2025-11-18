
const tokenInput = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJjMDUwYTVkNTMzZDU0ZjBmODJlY2NjZmQyZDgwZDRlYyIsImlhdCI6MTczNjcxNzU1NSwiZXhwIjoyMDUyMDc3NTU1fQ.oLIiyFN8c4lPCAgIPL_sFEWI12P3IcsLsid4KL8A2D0';
const http_port = window.location.protocol; // Guarda http: OU https:
const ip_e_porta = `${http_port}//${location.host}`; // Guarda o IP e porta do servidor    
var save_key_id = '';
var modo_prog_exec_del = null; // Variável para controlar o modo de abertura do painel IR
var cloned_btn = null;
var intervalpgrId = '';
var flick_bacground_color = '#444'; // Cor inicial do botão clonado
let id_entidade = null;
let nome_entidade = null;
const ws_mqtt_port = 9001; 
//const wss_mqtt_port = 9002; 
const tempo_leitura_codigo_ir = 10000;


///////////////////////////////////////////////////
// Tratamento do Click no menu controlos remotos //
///////////////////////////////////////////////////
document.addEventListener('DOMContentLoaded', function() {
    // Seleciona todos os itens de menu que têm submenu
    const menuItems = document.querySelectorAll('.has-submenu > a');
    
    // Adiciona evento de clique a cada item
    menuItems.forEach(item => {
        item.addEventListener('click', function(e) {
            // Previne o comportamento padrão do link
            e.preventDefault();
            
            // Encontra o item de menu pai
            const parentItem = this.parentElement;
            
            // Fecha outros submenus abertos no mesmo nível
            if (parentItem.classList.contains('active')) {
                parentItem.classList.remove('active');
            } else {
                // Fecha outros submenus no mesmo nível antes de abrir este
                const siblings = Array.from(parentItem.parentElement.children);
                siblings.forEach(sibling => {
                    if (sibling !== parentItem && sibling.classList.contains('has-submenu')) {
                        sibling.classList.remove('active');
                    }
                });
                
                // Abre o submenu atual
                parentItem.classList.add('active');
            }
        });
    });


    // Monitora processos cada X segundo
    // const intervalo = setInterval(() => {
        
    // }, 1000);

});// Fim de document.addEventListener('DOMContentLoaded', function()


////////////////////////////////////////////////////
// Script para controlar o carregamento da página //
////////////////////////////////////////////////////
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
                
        // Pega o valor de 'entity_id' e 'entity_name' da URL
        const params = new URLSearchParams(window.location.search);
        const entityId = params.get('entity_id');
        const entityName = params.get('name');

        if (entityId) {
            // Obter o ID (IEEE em hexadecimal) da entidade e o nome da entidade         
            const match = entityId.match(/0x[a-fA-F0-9]+/); // Procura por um padrão hexadecimal
            id_entidade = match ? match[0] : null; // Se encontrar, usa o primeiro match, senão, fica null
        } 
        else{
            id_entidade = null;
        }

        if (entityName) {
            nome_entidade = entityName;
        } 
        else{
            nome_entidade = null;
        }       
        
        //console.log(`ID da entidade: ${id_entidade}\nNome da entidade: ${nome_entidade}`);

        if (nome_entidade !== null && id_entidade !== null && id_entidade !== "0x0000000000000000" && nome_entidade !== "0x0000000000000000") {
            // Programar/Executar controlo remoto
            document.getElementById("div_ctrl_r_wrappper").style.display = "block";  
            document.getElementById("fieldset_div_ctrl_r_wrappper").style.display = "block";   
            document.getElementById("div_ir_panel").style.display = "block";                    
        }  
        else if (nome_entidade === null && id_entidade === null){
            // (*J*) TODO: Usar aqui Msg feedback para o utilizador
            document.getElementById("div_ctrl_r_wrappper").style.display = "none";  
            document.getElementById("fieldset_div_ctrl_r_wrappper").style.display = "none";   
            document.getElementById("div_ir_panel").style.display = "none"; 
        }  
        else if(id_entidade === "0x0000000000000000" && nome_entidade === "0x0000000000000000"){
            // Regstar controlo remoto (Hubs IR)
            document.getElementById("div_ctrl_r_wrappper").style.display = "block";
            document.getElementById("fieldset_div_ctrl_r_wrappper").style.display = "none";   
            document.getElementById("div_ir_panel").style.display = "none"; 
            

            // Processa dados dos Dispositivos/Hubs IR
            getDevices().then(devices => {

                // Filtrar devices pelo modelo (Hub IR) e/fabricante                
                const hubIRDevices = devices.filter(dev => 
                    dev.model?.toLowerCase().includes("remote") || 
                    dev.model?.toLowerCase().includes("ir") ||
                    dev.name?.toLowerCase().includes("infrared")
                );

                // Criar array com os dados relevantes
                const hubsData = hubIRDevices.map(device => ({
                    Id: device.id || 'N/A',
                    Nome: device.name || 'Sem Nome',                    
                    Fabricante: device.manufacturer || 'Desconhecido'
                }));               

                // Criar tabela com os hubs IR encontrados
                criarTabelaRapida(hubsData, 'div_ctrl_r_wrappper');

                var html_data = `&nbsp;&nbsp;${hubsData[0].Nome}&nbsp;&nbsp;`;
                document.getElementById("legenda_fieldset_hub_ir").innerHTML = html_data;

            });            
        }  

    }// Fim if
 
}




//////////////////////////////
// Criar Tabela Com Hubs IR //
//////////////////////////////
function criarTabelaRapida(array_hubs, containerId) {
  const container = document.getElementById(containerId);  
  const html = `
    <table class="tabela_hubs_ir">
      <thead>
        <tr>          
          <th>Nome</th>
          <th>Fabricante</th>
          <th>Comandos</th>
        </tr>
      </thead>
      <tbody>
        ${array_hubs.map(hub => `
          <tr>
            <td id="td_nome_hub_ir">${hub.Nome}</td>
            <td>${hub.Fabricante}</td>
            <td id="td_comandos_hub_ir">
              <a href="#" onclick="enviarComando('${hub.Nome}')"><i style="color: #4a4d4a; padding-right: 20px;" class="fas fa-pencil-alt"></i></a>
              <a href="#" onclick="enviarComando('${hub.Nome}')"><i style="color: #39be5a; padding-right: 20px;" class="fas fa-save"></i></a>
              <a href="#" onclick="enviarComando('${hub.Nome}')"><i style="color: #B03A2E;" class="fas fa-trash"></i></a>
            </td>
          </tr>
        `).join('')}
      </tbody>
    </table>
    
    <div id="div_rename_hub_ir" class="">
        <fieldset id="fieldset_hub_ir" class="">
            <legend id="legenda_fieldset_hub_ir"><b>&nbsp; &nbsp;</b></legend>                
                <input type="text" id="txt_nome_dispositivo_aux" value="" autocomplete="off" placeholder="Nome do Dispositivo" enabled>        
                <!-- <input type="text" id="txt_nome_dispositivo" maxlength="30" oninput="changeTxtHandler(this.id);" autocomplete="off" placeholder="Nome do Dispositivo" disabled> -->
                <div id="div_edit_name" onclick="enableDeviceNameEdiction();">
                    <a href="#" onclick=""><i style="color: #4a4d4a; padding-right: 20px;" class="fas fa-pencil-alt"></i></a>
                    <a href="#" onclick=""><i style="color: #39be5a; padding-right: 20px;" class="fas fa-save"></i></a>
                    <a href="#" onclick=""><i style="color: #B03A2E;" class="fas fa-trash"></i></a>
                </div>   
                <div id="div_save_name" onclick="saveDeviceName();">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 14.66V20a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h5.34"></path><polygon points="18 2 22 6 12 16 8 16 8 12 18 2"></polygon></svg>
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="green" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path><polyline points="17 21 17 13 7 13 7 21"></polyline><polyline points="7 3 7 8 15 8"></polyline></svg>
                </div>                
        </fieldset>
    </div>`;

  container.innerHTML = html;
}

 


/////////////////////////////////
// Enviar comando MQTT via API //
/////////////////////////////////
async function publishMQTT(ip_porta, topic, payload) {
  try {
    const url = `${ip_porta}/api/services/mqtt/publish`;
    const response = await fetch(url, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${tokenInput}`
      },
      body: JSON.stringify({
        topic: topic,
        payload: payload
      })
    });
    
    const data = await response.json();
    //console.log('Publicação MQTT realizada:', data);
    return data;
  } catch (error) {
    console.error('Erro ao publicar MQTT:', error);
  }
}


/////////////////////////////////
// Obter o codigo IR aprendido //
/////////////////////////////////
// async function obterCodigoIRaprendido(ip_porta, entity_id) {
//     const url = `${ip_porta}/api/states/sensor.${entity_id}_learned_ir_code`;
//     try {
//         const response = await fetch(url, {
//             headers: {
//                 "Authorization": `Bearer ${tokenInput}`,
//                 "Content-Type": "application/json"
//             }
//         });
//         const data = await response.json();
//         return data.state; 
//     } catch (err) {
//         console.error("Erro:", err);
//         throw err; 
//     }
// }




//////////////////////////////////
// Limpar o codigo IR aprendido //
//////////////////////////////////
// async function limparCodigoIRaprendido(ip_porta, entity_id) {
//     const url = `${ip_porta}/api/states/sensor.${entity_id}_learned_ir_code`;
//     try {
//         const response = await fetch(url, {
//             method: "POST", // Método POST para atualizar o estado
//             headers: {
//                 "Authorization": `Bearer ${tokenInput}`,
//                 "Content-Type": "application/json",
//             },
//             body: JSON.stringify({
//                 state: "", // Define o estado como vazio
//                 attributes: {} // Mantém os atributos (opcional)
//             }),
//         });
//         if (!response.ok) {
//             throw new Error(`Erro HTTP: ${response.status}`);
//         }
//         //console.log("Código IR limpo com sucesso!");
//         return true;
//     } catch (err) {
//         console.error("Falha ao limpar código IR:", err);
//         return false;
//     }
// }



//////////////////////////////////////////////////////////
// Activar/Desactivar modo de aprendizagem de codigo IR //
//////////////////////////////////////////////////////////
// async function ativarModoAprendizagemIR(ip_porta, entity_id, modo) {
//     const url = `${ip_porta}/api/services/switch/${modo === 'activar' ? 'turn_on' : 'turn_off'}`;
//     const entityId = `switch.${entity_id}_learn_ir_code`;

//     try {
//         const response = await fetch(url, {
//             method: "POST",
//             headers: {
//                 "Authorization": `Bearer ${tokenInput}`,
//                 "Content-Type": "application/json",
//             },
//             body: JSON.stringify({
//                 entity_id: entityId, // Identificador do switch IR
//             }),
//         });

//         if (!response.ok) {
//             throw new Error(`Erro HTTP: ${response.status}`);
//         }
//         originalString = modo;
//         const capitalizedString = originalString.charAt(0).toUpperCase() + originalString.slice(1);
//         console.log(`${capitalizedString} modo de aprendizagem de codigo IR.`);
//         return true;
//     } catch (err) {
//         console.error("Falha ao ativar modo de aprendizagem:", err);
//         return false;
//     }
// }



//////////////////////////////////////////////////////////////
// Exibir a página após o carregamento completo dos estilos //
//////////////////////////////////////////////////////////////
async function showPage() {
    document.getElementById("div_ctrl_r_wrappper").style.display = "block";  
    document.getElementById("fieldset_div_ctrl_r_wrappper").style.display = "block";   
    document.getElementById("div_ir_panel").style.display = "block";
}



///////////////////////////////////////////////////
// Envia comando IR para o dispositivo, via mqtt //
///////////////////////////////////////////////////
async function send_ir_code(btnId){     

    // Obter os códigos IR do arquivo JSON
    const irCodes = await obterDadosIR(ip_e_porta);

    // Substituir aspas simples por aspas duplas
    const textoComAspasDuplas = irCodes.replace(/'/g, '"');
 
    // Converter para objeto JavaScript
    let objetoJson;
    try {
        objetoJson = JSON.parse(textoComAspasDuplas);      
        const desiredCode = objetoJson.find(code => code.btn_id === btnId);
        if (desiredCode) {                   
            publishMQTT(ip_e_porta, `zigbee2mqtt/${desiredCode.hub_id}/set`, `{"ir_code_to_send": "${desiredCode.ir_code}"}`);     
        } 
        else{
            // (*J*) TODO: Usar aqui Msg feedback para o utilizador
            alert("A tecla preesionada não está programada!");
        }  
    } catch (e) {
        console.error("Erro na conversão:", e);
    }    
}





////////////////////////////////////////
// Guardar Codigos IR no arquivo json //
////////////////////////////////////////
async function guardaCodigosIR(ip_porta, json_ir_data) {

    // Obter conteudo objectos json com os codigos IR.
    var irCodes_aux = await obterDadosIR(ip_porta);  
    
    // Substituir aspas simples por aspas duplas
    const textoComAspasDuplas = irCodes_aux.replace(/'/g, '"');

    // Converter para objeto JavaScript
    let irCodes;
    try {
        irCodes = JSON.parse(textoComAspasDuplas);         
    } catch (e) {
        // (*J*) TODO: Usar aqui Msg feedback para o utilizador
        console.error("Erro na conversão:", e);
    }     
   

    // Se o código já existir, cancelar a adição
    const existe = irCodes.some(obj => obj.btn_id === save_key_id && obj.hub_id === id_entidade);
    if (existe && modo_prog_exec_del === "PROGRAMAR") {
        // (*J*) TODO: Usar aqui Msg feedback para o utilizador
        alert("Código IR já existe");
        return;
    }
    else if (!existe && modo_prog_exec_del === "PROGRAMAR") {
        // Adicionar ao array
        irCodes.push(json_ir_data); 
    }
    else if (existe && modo_prog_exec_del === "DESPROGRAMAR") {
        // Obter indice do objecto a ser eliminado
        const index = irCodes.findIndex(item => item.btn_id === save_key_id && item.hub_id === id_entidade);
        // Remover se encontrado
        if (index !== -1) {
            irCodes.splice(index, 1);
        }
    }
       
    
    // Guardar dados no arquivo "codigos_ir.json"
    const token = tokenInput;     
    const url = `${ip_porta}/api/services/shell_command/guardar_codigos_ir`;

    fetch(url, {
    method: "POST",
    headers: {
        "Authorization": `Bearer ${token}`,
        "Content-Type": "application/json"
    },
        body: JSON.stringify({"data": irCodes})
    })
    .then(response => {
    if (!response.ok) {
        throw new Error("Erro na requisição");
    }
    else if(response.ok){
        if(modo_prog_exec_del === "PROGRAMAR"){
            // (*J*) TODO: Usar aqui Msg feedback para o utilizador
            console.log('Processo concluido, para testar pressione o icon da tecla programada.');
        }
        else if(modo_prog_exec_del === "DESPROGRAMAR"){
            // (*J*) TODO: Usar aqui Msg feedback para o utilizador
            console.log('Processo concluido.');
        }        
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




//////////////////////////////////////
// Obter Codigos IR do arquivo json //
//////////////////////////////////////    
async function obterDadosIR(ip_porta) {    

    try {
        // (*J*) TODO: Retirar "timestamp" no codigo definitivo
        const timestamp = new Date().getTime(); 
        const url = `${ip_porta}/local/json_files/codigos_ir.json?t=${timestamp}`;
        const response = await fetch(url);
        if (!response.ok) {
            throw new Error('Arquivo não encontrado');
        }

        const data = await response.json();
        
        return data;

    } catch (error) {
        console.error('Erro ao carregar arquivo JSON:', error);
        return null;
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


/////////////////////////////////////
// Abrir menu controlo remoto para //
/////////////////////////////////////
function abrir_menu_ctrl_r(elemento_clicado) {
    document.getElementById("header_menu_ctrl_r").style.display = "none";    
    document.getElementById("nav_menu_ctrl_r").style.display = "block";

    //document.getElementById("div_ctrl_r_wrappper").style.border = "1px solid #4e94ab";
    document.getElementById("fieldset_div_ctrl_r_wrappper").style.border = "1px solid #4e94ab";
    
    // Colapsa todos os itens, do menu ctrl remoto, que têm submenu
    document.querySelectorAll(".has-submenu.active").forEach((item) => {
      item.classList.remove("active");
    }); 
    
    // Ocultar controles remotos
    document.getElementById("controlo_remoto_ac").style.display = "none";
    document.getElementById("controlo_remoto_tv").style.display = "none";
    document.getElementById("controlo_remoto_ventilador").style.display = "none";
    document.getElementById("controlo_remoto_som").style.display = "none";
    document.getElementById("controlo_remoto_decoder").style.display = "none";
}



/////////////////////////////////////////
// Exibir controlo remoto seleccionado //
/////////////////////////////////////////
function exibir_ctrl_remoto(elemento_clicado){
    // Encontra o <li> pai (com classe 'has-submenu')
    const liPai = elemento_clicado.closest('.has-submenu');
    
    if (liPai) {
        const idDoLiPai = liPai.id;         
        
        if(idDoLiPai === "ACN") {
            document.getElementById("controlo_remoto_ac").style.display = "block";
            document.getElementById("controlo_remoto_tv").style.display = "none";
            document.getElementById("controlo_remoto_ventilador").style.display = "none";
            document.getElementById("controlo_remoto_som").style.display = "none";
            document.getElementById("controlo_remoto_decoder").style.display = "none";
        }
        else if(idDoLiPai === "TLV") {
            document.getElementById("controlo_remoto_ac").style.display = "none";
            document.getElementById("controlo_remoto_tv").style.display = "block";
            document.getElementById("controlo_remoto_ventilador").style.display = "none";
            document.getElementById("controlo_remoto_som").style.display = "none";
            document.getElementById("controlo_remoto_decoder").style.display = "none"; 
        }
        else if(idDoLiPai === "FAN") {
            document.getElementById("controlo_remoto_ac").style.display = "none";
            document.getElementById("controlo_remoto_tv").style.display = "none";
            document.getElementById("controlo_remoto_ventilador").style.display = "block";
            document.getElementById("controlo_remoto_som").style.display = "none";
            document.getElementById("controlo_remoto_decoder").style.display = "none"; 
        }
        else if(idDoLiPai === "SND") {
            document.getElementById("controlo_remoto_ac").style.display = "none";
            document.getElementById("controlo_remoto_tv").style.display = "none";
            document.getElementById("controlo_remoto_ventilador").style.display = "none";
            document.getElementById("controlo_remoto_som").style.display = "block";
            document.getElementById("controlo_remoto_decoder").style.display = "none"; 
        }
        else if(idDoLiPai === "DCD") {
            document.getElementById("controlo_remoto_ac").style.display = "none";
            document.getElementById("controlo_remoto_tv").style.display = "none";
            document.getElementById("controlo_remoto_ventilador").style.display = "none";
            document.getElementById("controlo_remoto_som").style.display = "none";
            document.getElementById("controlo_remoto_decoder").style.display = "block"; 
        }
        else {
            console.error("ID do <li> pai não reconhecido:", idDoLiPai);
        }     
    } 
}
   

///////////////////////////////////////////
// Abrir controlo remoto para execucacao //
///////////////////////////////////////////
function executar_ctrl_remoto(elemento_clicado){

    // Modo de execução
    modo_prog_exec_del = "EXECUTAR";

    document.getElementById("header_menu_ctrl_r").style.display = "block";    
    document.getElementsByClassName("vertical-menu")[0].style.display = "none";
    //document.getElementById("div_ctrl_r_wrappper").style.border = "1px solid #39be5a";
    document.getElementById("div_ctrl_r_wrappper").style.width = "auto";

    document.getElementById("fieldset_div_ctrl_r_wrappper").style.border = "1px solid #39be5a";
    document.getElementById("fieldset_div_ctrl_r_wrappper").style.width = "auto";
    
    // Exibir controlo remoto seleccionado
    exibir_ctrl_remoto(elemento_clicado)
}


////////////////////////////////////////////
// Abrir controlo remoto para programacao //
////////////////////////////////////////////
function programar_ctrl_remoto(elemento_clicado){   

    // Modo de programação
    modo_prog_exec_del = "PROGRAMAR";

    document.getElementById("header_menu_ctrl_r").style.display = "block";    
    document.getElementsByClassName("vertical-menu")[0].style.display = "none";
    //document.getElementById("div_ctrl_r_wrappper").style.border = "1px solid #B03A2E";
    document.getElementById("div_ctrl_r_wrappper").style.width = "auto"; 
     document.getElementById("fieldset_div_ctrl_r_wrappper").style.border = "1px solid #B03A2E";
    document.getElementById("fieldset_div_ctrl_r_wrappper").style.width = "auto";
    // Exibir controlo remoto seleccionado
    exibir_ctrl_remoto(elemento_clicado)
}


////////////////////////////////////////////
// click nos botoes dos controlos remotos //
////////////////////////////////////////////
async function ctl_r_btn_click(elemento_clicado) {

    // Verificar se a teclav ja esta programada
    // Obter conteudo objectos json com os codigos IR.
    var irCodes_aux = await obterDadosIR(ip_e_porta);  
    
    // Substituir aspas simples por aspas duplas
    const textoComAspasDuplas = irCodes_aux.replace(/'/g, '"');

    // Converter para objeto JavaScript
    let irCodes;
    try {
        irCodes = JSON.parse(textoComAspasDuplas);         
    } catch (e) {
        // (*J*) TODO: Usar aqui Msg feedback para o utilizador
        console.error("Erro na conversão:", e);
    }     
   
    // Se o código já existir, cancelar a adição   
    const existe = irCodes.some(obj => obj.btn_id ===  elemento_clicado.id && obj.hub_id === id_entidade);
    if (existe && modo_prog_exec_del === "PROGRAMAR") {        
        modo_prog_exec_del = "DESPROGRAMAR"; 
    }

   
    // Chamar funcao correspondente a acao
    if (modo_prog_exec_del === "PROGRAMAR") {
        // Guardar o codigo IR associado ao botão clicado
        abrirPainelIR(elemento_clicado, modo_prog_exec_del);
    }
    else if (modo_prog_exec_del === "EXECUTAR") {
        // Enviar o comando IR associado ao botão clicado
        send_ir_code(elemento_clicado.id);
    }
    else if (modo_prog_exec_del === "DESPROGRAMAR") {
        abrirPainelIR(elemento_clicado, modo_prog_exec_del);
    }
    else {
        console.error("Modo de operação não reconhecido:", modo_prog_exec_del);
    }   
}



/////////////////////
// Abrir Painel IR //
/////////////////////
function abrirPainelIR(key_id, modo){

    save_key_id = key_id.id; // Guarda o ID da tecla clicada

    document.getElementById('div_ctrl_r_wrappper').style.display = 'none';
    
    if(modo == "DESPROGRAMAR"){
        document.getElementById('btn_programar').style.display = 'none';
        document.getElementById('btn_desprogramar').style.display = 'inline';
    }
    else if(modo == "PROGRAMAR"){        
        document.getElementById('btn_programar').style.display = 'inline';
        document.getElementById('btn_desprogramar').style.display = 'none';
    }

    let elem = document.getElementById('div_ir_panel');
    elem.style.display = 'block';
    
    // Clonar o botão e copiar os estilos
    let teclDestino = document.getElementById('div_tecla_painel_ir');
    teclDestino.innerHTML = '';   
    teclDestino.style.justifyItems = 'center';  
    teclDestino.style.justifyContent = 'center'; 
    teclDestino.style.alignItems = 'center';  

    copiarBotaoComEstilos(save_key_id, 'div_tecla_painel_ir');  

    const span = document.querySelector('#div_tecla_painel_ir span');
    cloned_btn = document.querySelector('#div_tecla_painel_ir button');
    cloned_btn.style.margin = 'auto';           // Centraliza o botão clonado
    cloned_btn.id = save_key_id + '_btn_clone'; // Altera o ID do botão clonado
    cloned_btn.style.pointerEvents = 'none';    // Desativa eventos de clique no botão clonado
    
    if (span) {    
        span.style.fontSize = '0.8rem';     
        span.style.marginTop = '5px'; 
    }

}




////////////////////////////////////////
// Clonar o botão e copiar os estilos //
////////////////////////////////////////
function copiarBotaoComEstilos(idBotaoOrigem, idDivDestino) {

    const original = document.getElementById(idBotaoOrigem);
    const destino = document.getElementById(idDivDestino);
    
    // Clona o elemento
    const clone = original.cloneNode(true);
    
    // Copia os estilos computados (para pegar todos os estilos aplicados)
    const estilos = window.getComputedStyle(original);
    
    // Aplica cada estilo como inline no clone
    for (let i = 0; i < estilos.length; i++) {
        const propriedade = estilos[i];
        clone.style[propriedade] = estilos.getPropertyValue(propriedade);
    }    
     
    destino.appendChild(clone);     
}



//////////////////////
// fechar painel IR //
//////////////////////
function fecharPainelIR(){
    
    document.getElementById('div_ir_panel').style.display = 'none';
    document.getElementById('div_ctrl_r_wrappper').style.display = 'block';
    let teclDestino = document.getElementById('div_tecla_painel_ir');
    
    while (teclDestino.firstChild) {
        teclDestino.removeChild(teclDestino.lastChild);
    }

    clearInterval(intervalpgrId);
    flick_bacground_color = '#444'; 
}



/////////////////////
// Programar Tecla //
/////////////////////
async function programarTecla(){   
    
    // Objecto associado ao botão a programar
    if(id_entidade != null && save_key_id != null) {  
        
        // Efeito de flicker no botão clonado
        intervalpgrId = setInterval(function() {
            if(flick_bacground_color == 'red'){
                flick_bacground_color = '#444';
            }
            else{
                flick_bacground_color = 'red';           
            }

            flickButtonPgr(cloned_btn);
            if(modo_prog_exec_del !== "PROGRAMAR") {
                clearInterval(intervalpgrId);
                flick_bacground_color = '#444';
            }  
        }, 800);
       

        // Substituicao do unmero da porta http pela porta websocket
        let url = new URL(window.location.href);        
        let gwUrl =  null;   

        if (http_port === 'http:') {
            url.port = ws_mqtt_port; // 9001
            gwUrl = `ws://${url.host}`;
        }
        else if (http_port === 'https:'){        
            gwUrl = 'wss://mqtt.domoticaza.org';
        }   


        // Conectar-se ao broker MQTT
        const client = mqtt.connect(gwUrl); 
        const receivedCodes = []; // Array para armazenar os códigos  
        
        client.on('connect', () => {
            console.log('Conectado ao Broker');
            client.subscribe(`zigbee2mqtt/${id_entidade}`, (err) => {
                if (!err) {
                    console.log('Subscrito no tópico.');
                }
                else{
                    console.log(`Erro: ${err}`);
                }
            });
            
            // Parar após 10 segundos
            setTimeout(() => {   
                // Encerrar conexão MQTT                      
                client.end(() => {                              
                });            

                if(receivedCodes.length > 0){

                    // Parar flicker com borda veremlha
                    clearInterval(intervalpgrId); 
                    
                    // Colocar borda verde, activar acao sobre o botao
                    cloned_btn.style.border = `5px solid green`;
                    cloned_btn.style.pointerEvents = 'auto'; 
                    cloned_btn.style.cursor = 'pointer'; 
                    
                    // Para Testar a tecla programada
                    cloned_btn.onclick = function() {
                        const original = cloned_btn.id;
                        let id_btn = original.replace("_btn_clone", "");
                        send_ir_code(id_btn);
                    };

                    document.getElementById('btn_programar').style.display = 'none';
                    const codigo_aprendido = receivedCodes[receivedCodes.length - 1].code;
                    //console.log(codigo_aprendido);

                    const hubs_ir_obj = {
                        "btn_id": save_key_id,
                        "hub_id": id_entidade,
                        'ir_code': codigo_aprendido
                    };

                    // Guarda os códigos IR
                    guardaCodigosIR(ip_e_porta, hubs_ir_obj);                      
                }
                else{
                    // (*J*) TODO: Usar aqui Msg feedback para o utilizador                
                    console.log("Nenhum código IR aprendido. Tente novamente.");   
                }
                        

            }, tempo_leitura_codigo_ir);     
            
        });     

        // Activar modo aprendizagem
        const msg = '{"learn_ir_code":"ON"}';
        client.publish(`zigbee2mqtt/${id_entidade}/set`, msg);

        // Processar mensagens recebidas via MQTT
        client.on('message', (topic, message) => {      
            
            try {

                const dados = JSON.parse(message.toString());  

                if ( dados.learned_ir_code ) {

                receivedCodes.push({
                        //timestamp: new Date().toISOString(),                    
                        //length: dados.learned_ir_code.length,
                        //raw: dados,
                        code: dados.learned_ir_code
                    });
                }

            } catch (e) {
                // (*J*) TODO: Usar aqui Msg feedback para o utilizador
                console.error("Erro ao processar mensagem!", e);
            }        
            
        });       

    }// if(id_entidade != null && save_key_id != null)
    
}// function programarTecla()




////////////////////////
// Desprogramar Tecla //
////////////////////////
async function desprogramarTecla(){   
    
    // Objecto associado ao botão a programar
    if(id_entidade != null && save_key_id != null) {  
        
        // Efeito de flicker no botão clonado
        intervalpgrId = setInterval(function() {
            if(flick_bacground_color == 'red'){
                flick_bacground_color = '#444';
            }
            else{
                flick_bacground_color = 'red';           
            }

            flickButtonPgr(cloned_btn);
            if(modo_prog_exec_del !== "PROGRAMAR") {
                clearInterval(intervalpgrId);
                flick_bacground_color = '#444';
            }  
        }, 800);
       

        // Parar flicker com borda veremlha
        clearInterval(intervalpgrId); 

        // Colocar borda verde, activar acao sobre o botao
        cloned_btn.style.border = `5px solid lightgray`;
        document.getElementById('btn_desprogramar').style.display = 'none';


        // Ajustar objecto para desprogramar a tecla indicada
        const hubs_ir_obj = {
            "btn_id": save_key_id,
            "hub_id": id_entidade,
            'ir_code': ''
        };  
        
        // Guarda os códigos IR
        guardaCodigosIR(ip_e_porta, hubs_ir_obj);        

    }// if(id_entidade != null && save_key_id != null)
    
}// function desprogramarTecla()




//////////////////////////////////////////
// função auxiliar para flick de botões //
//////////////////////////////////////////
function flickButtonPgr(button) {  
   button.style.border = `5px solid ${flick_bacground_color}`;
   button.style.color = flick_bacground_color;
}




////////////////////////////////////////
// Obter Devices Associados a Hubs IR //
////////////////////////////////////////
async function getDevices() {
   return new Promise((resolve, reject) => {

    let HA_URL =  null; 
    let url = new URL(window.location.href);  

    if (http_port === 'http:') {        
        HA_URL = `ws://${url.host}/api/websocket`;
    }
    else if (http_port === 'https:'){        
        HA_URL = `wss://${url.host}/api/websocket`;
    }      

     const ws = new WebSocket(HA_URL);

     ws.onopen = () => {
       console.log("Conectado ao WebSocket API");
     };

     ws.onmessage = (event) => {
       const msg = JSON.parse(event.data);

       // 1º passo → autenticação
       if (msg.type === "auth_required") {
         ws.send(
           JSON.stringify({
             type: "auth",
             access_token: tokenInput,
           })
         );
       }

       // 2º passo → confirmar autenticação
       else if (msg.type === "auth_ok") {
         // pedir a lista de devices do device registry
         ws.send(
           JSON.stringify({
             id: 1,
             type: "config/device_registry/list",
           })
         );
       }

       // 3º passo → resposta com devices
       else if (msg.id === 1) {
         resolve(msg.result);
         ws.close();
       } else if (msg.type === "auth_invalid") {
         reject("Token inválido!");
         ws.close();
       }
     };
   });
}









/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/

////////////////////////
// CONTROLO REMOTO TV //
////////////////////////

// Funcionalidade básica
// const numberBtns = document.querySelectorAll('.number-btn');
// const displayNumber = document.querySelector('.channel-number');
// const displayInfo = document.querySelector('.channel-info');
// const powerBtn = document.querySelector('.btn-power');

// // Canais fictícios para demonstração
// const channels = {
//     '101': 'RTP 1',
//     '102': 'RTP 2',
//     '103': 'SIC',
//     '104': 'TVI',
//     '105': 'CNN Portugal',
//     '106': 'Sport TV',
//     '107': 'FOX Movies',
//     '108': 'Discovery',
//     '109': 'National Geographic',
//     '110': 'Cartoon Network'
// };

// let currentNumber = '';

// numberBtns.forEach(btn => {
//     btn.addEventListener('click', function() {
//         const value = this.textContent.trim();
        
//         if (value === '#' || value === '*') return;
        
//         currentNumber += value;
        
//         if (currentNumber.length > 3) {
//             currentNumber = value; // Reinicia se mais de 3 dígitos
//         }
        
//         displayNumber.textContent = currentNumber;
        
//         // Atualiza informação do canal após pequeno delay
//         setTimeout(() => {
//             if (channels[currentNumber]) {
//                 displayInfo.textContent = channels[currentNumber];
//             } else {
//                 displayInfo.textContent = "Canal não disponível";
//             }
//         }, 300);
//     });
// });

// powerBtn.addEventListener('click', function() {
//     this.classList.toggle('active');
// });

// // Simula mudança de canal com as setas
// document.querySelector('.nav-btn.up').addEventListener('click', function() {
//     let num = parseInt(currentNumber) + 1;
//     if (num > 110) num = 101;
//     currentNumber = num.toString();
//     updateChannel();
// });

// document.querySelector('.nav-btn.down').addEventListener('click', function() {
//     let num = parseInt(currentNumber) - 1;
//     if (num < 101) num = 110;
//     currentNumber = num.toString();
//     updateChannel();
// });

// function updateChannel() {
//     displayNumber.textContent = currentNumber;
//     displayInfo.textContent = channels[currentNumber] || "Canal não disponível";
// }



////////////////////////////////
// CONTROLO REMOTO VENTILADOR //
////////////////////////////////
// const powerBtnVentilador = document.querySelector('.power-btn');
// const speedLevels = document.querySelectorAll('.speed-level');
// const modeBtns = document.querySelectorAll('.controls .btn');
// const timerBtns = document.querySelectorAll('.timer-btn');

// powerBtnVentilador.addEventListener('click', function() {
//     this.classList.toggle('active');
// });

// speedLevels.forEach(level => {
//     level.addEventListener('click', function() {
//         speedLevels.forEach(l => l.classList.remove('active'));
//         this.classList.add('active');
//     });
// });

// modeBtns.forEach(btn => {
//     btn.addEventListener('click', function() {
//         modeBtns.forEach(b => b.classList.remove('active'));
//         this.classList.add('active');
//     });
// });

// timerBtns.forEach(btn => {
//     btn.addEventListener('click', function() {
//         timerBtns.forEach(b => b.classList.remove('active'));
//         this.classList.add('active');
//     });
// });





////////////////////////////////
// CONTROLO REMOTO VENTILADOR //
////////////////////////////////
// const playBtn = document.querySelector('.btn-play');
// const powerBtnSom = document.querySelector('.power-btn');
// const sourceBtns = document.querySelectorAll('.source-btn');
// const volumeLevel = document.querySelector('.volume-level');
// let isPlaying = false;
// let volume = 70;

// playBtn.addEventListener('click', function() {
//     isPlaying = !isPlaying;
//     this.innerHTML = isPlaying ? '<i class="fas fa-pause"></i>' : '<i class="fas fa-play"></i>';
// });

// powerBtnSom.addEventListener('click', function() {
//     this.classList.toggle('active');
// });

// sourceBtns.forEach(btn => {
//     btn.addEventListener('click', function() {
//         sourceBtns.forEach(b => b.classList.remove('active'));
//         this.classList.add('active');
//     });
// });

// // Controle de volume simulado
// document.querySelector('.fa-volume-up').addEventListener('click', function() {
//     if (volume < 100) {
//         volume += 10;
//         volumeLevel.style.width = volume + '%';
//     }
// });

// document.querySelector('.fa-volume-down').addEventListener('click', function() {
//     if (volume > 0) {
//         volume -= 10;
//         volumeLevel.style.width = volume + '%';
//     }
// });



////////////////////////////////
// CONTROLO REMOTO VENTILADOR //
////////////////////////////////
// const numberBtnsDecoder = document.querySelectorAll('.number-btn');
// const displayNumberDecoder = document.querySelector('.channel-number');
// const displayInfoDecoder = document.querySelector('.channel-info');
// const powerBtnDecoder = document.querySelector('.btn-power');

// // Canais fictícios para demonstração
// const channels_decoder = {
//     '101': 'RTP 1',
//     '102': 'RTP 2',
//     '103': 'SIC',
//     '104': 'TVI',
//     '105': 'CNN Portugal',
//     '106': 'Sport TV',
//     '107': 'FOX Movies',
//     '108': 'Discovery',
//     '109': 'National Geographic',
//     '110': 'Cartoon Network'
// };

// let currentNumberDecoder = '';

// numberBtnsDecoder.forEach(btn => {
//     btn.addEventListener('click', function() {
//         const value = this.textContent.trim();
        
//         if (value === '#' || value === '*') return;
        
//         currentNumberDecoder += value;

//         if (currentNumberDecoder.length > 3) {
//             currentNumberDecoder = value; // Reinicia se mais de 3 dígitos
//         }
        
//         displayNumberDecoder.textContent = currentNumberDecoder;
        
//         // Atualiza informação do canal após pequeno delay
//         setTimeout(() => {
//             if (channels_decoder[currentNumberDecoder]) {
//                 displayInfoDecoder.textContent = channels_decoder[currentNumberDecoder];
//             } else {
//                 displayInfoDecoder.textContent = "Canal não disponível";
//             }
//         }, 300);
//     });
// });

// powerBtnDecoder.addEventListener('click', function() {
//     this.classList.toggle('active');
// });

// // Simula mudança de canal com as setas
// document.querySelector('.nav-btn.up').addEventListener('click', function() {
//     let num = parseInt(currentNumberDecoder) + 1;
//     if (num > 110) num = 101;
//     currentNumberDecoder = num.toString();
//     updateChannel();
// });

// document.querySelector('.nav-btn.down').addEventListener('click', function() {
//     let num = parseInt(currentNumberDecoder) - 1;
//     if (num < 101) num = 110;
//     currentNumberDecoder = num.toString();
//     updateChannel();
// });

// function updateChannel() {
//     displayNumberDecoder.textContent = currentNumberDecoder;
//     displayInfoDecoder.textContent = channels_decoder[currentNumberDecoder] || "Canal não disponível";
// }




////////////////////////
// CONTROLO REMOTO AC //
////////////////////////
// let powerOn = false;
// let currentTemp = 24;
// let currentMode = 'cool'; // cool, heat, fan, dry, auto

// // Elementos DOM
// const tempValue = document.getElementById('temp-value');
// const modeDisplay = document.getElementById('mode-display');
// const powerBtnAc = document.getElementById('power-btn');
// const tempUpBtn = document.getElementById('temp-up');
// const tempDownBtn = document.getElementById('temp-down');
// const modeBtn = document.getElementById('mode-btn');
// const fanBtn = document.getElementById('fan-btn');
// const swingBtn = document.getElementById('swing-btn');
// const timerBtn = document.getElementById('timer-btn');
// const turboBtn = document.getElementById('turbo-btn');
// const sleepBtn = document.getElementById('sleep-btn');

// // Funções de controle
// function togglePower() {
//     powerOn = !powerOn;
//     powerBtnAc.style.backgroundColor = powerOn ? '#e74c3c' : '#2ecc71';
//     powerBtnAc.innerHTML = powerOn 
//         ? '<i class="fas fa-power-off"></i> Desligar' 
//         : '<i class="fas fa-power-off"></i> Ligar';
//     updateDisplay();
// }

// function changeTemp(amount) {
//     if (!powerOn) return;
    
//     currentTemp += amount;
//     if (currentTemp < 16) currentTemp = 16;
//     if (currentTemp > 30) currentTemp = 30;
    
//     tempValue.textContent = currentTemp;
// }

// function cycleMode() {
//     if (!powerOn) return;
    
//     const modes = [
//         {name: 'Resfriar', icon: 'fa-snowflake'},
//         {name: 'Aquecer', icon: 'fa-sun'},
//         {name: 'Ventilar', icon: 'fa-fan'},
//         {name: 'Desumidificar', icon: 'fa-tint'},
//         {name: 'Automático', icon: 'fa-cog'}
//     ];
    
//     const currentIndex = modes.findIndex(m => m.name === modeDisplay.textContent.replace('Modo: ', ''));
//     const nextIndex = (currentIndex + 1) % modes.length;
    
//     modeDisplay.textContent = `Modo: ${modes[nextIndex].name}`;
//     modeBtn.innerHTML = `<i class="fas ${modes[nextIndex].icon}"></i> Modo`;
// }

// function updateDisplay() {
//     tempValue.style.opacity = powerOn ? 1 : 0.5;
//     modeDisplay.style.opacity = powerOn ? 1 : 0.5;
// }

// // Event listeners
// powerBtnAc.addEventListener('click', togglePower);
// tempUpBtn.addEventListener('click', () => changeTemp(1));
// tempDownBtn.addEventListener('click', () => changeTemp(-1));
// modeBtn.addEventListener('click', cycleMode);

// // Inicialização
// updateDisplay();