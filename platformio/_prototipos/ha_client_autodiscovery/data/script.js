

///////////////////////
// variáveis globais //
///////////////////////
var get_dev_name = '';


/////////////////////////////////////////////////////
// Acções a serem executadas ao carre3gar a página //
/////////////////////////////////////////////////////
document.addEventListener("DOMContentLoaded", function () { 
    GetDeviceName();
});

async function GetDeviceName(){ 
    let txt_json = JSON.stringify({ CMD: "GET_DEV_NAME", Modo:""});
    let url_payload = "http://" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);   

    get_dev_name = result; 
    document.getElementById('lgd').innerHTML = '<b>&nbsp;' + get_dev_name + '&nbsp;</b>';
    document.getElementById("div_guardar_nome").disabled = true;
    document.getElementById("div_guardar_nome").querySelector('svg').style.stroke = "gray";
 }





////////////////////////////////////////
// Enviar Requisição HTTP ao Servidor //
////////////////////////////////////////
function req_http(method, url) {

    return new Promise(function (resolve, reject) {
        let xhr = new XMLHttpRequest();
        xhr.open(method, url);
        xhr.onload = function () {
            if (this.status >= 200 && this.status < 300) {
                resolve(xhr.response);
            } else {
                reject({
                    status: this.status,
                    statusText: xhr.statusText
                });
            }
        };
        xhr.onerror = function () {
            reject({
                status: this.status,
                statusText: xhr.statusText
            });
        };
        xhr.send();
    });
}





/////////////////////
// Exibir Mensagem //
/////////////////////
function showMsgBox(mensagem){   
    const pai = document.getElementById('div-msg-feedback-back');
    pai.style.display = 'block';
    pai.style.zIndex = 1; 
    pai.querySelector('.div_msg_feedback').innerHTML = mensagem;   
    document.getElementById('div_set_ota').style.zIndex = -999;      
    setTimeout(function(){ fecharShowMsgBox(); }, 5000);  
}


/////////////////////
// Fechar Mensagem //
/////////////////////
function fecharShowMsgBox(){
    document.getElementById('div-msg-feedback-back').style.display = 'none';       
    document.getElementById('div_set_ota').style.display = 'block'; 
    document.getElementById('div_set_ota').style.zIndex = 999;     
}



//////////////////////////////////////////////////////////////////////////////////////
// Envia comando aos clientes para cancelar o modo OTA, para carregar SKETCH/SPIFFS //
//////////////////////////////////////////////////////////////////////////////////////
async function stopOTA(){      
    
    let txt_json = JSON.stringify({ CMD: "CANCEL_OTA", Modo:""});
    let url_payload = "http://" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);      
}

async function limparDispositivo(){        
    let txt_json = JSON.stringify({ CMD: "RESET_DEVICE", Modo:""});
    let url_payload = "http://" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);      
}





//////////////////////////////////////////////////////
// Envia comando ao Dispositivo para alterar o nome //
//////////////////////////////////////////////////////
async function renomearDispositivo(){      
    
    const nome = document.getElementById('txt_dados_ota').value;

    if ( nome == "" || nome == null ) {
        showMsgBox('Efectue as alterações a guardar!');
        document.getElementById("div_guardar_nome").disabled = true;
        document.getElementById("div_guardar_nome").querySelector('svg').style.stroke = "gray";
        return;
    }

    let txt_json = JSON.stringify({ CMD: "RENAME_DEV", Nome: nome});
    let url_payload = "http://" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);     
    
    if(result == "OK"){
        showMsgBox('Processo Concluido.');
        document.getElementById('txt_dados_ota').value = '';
        document.getElementById('lgd').innerHTML = '<b>&nbsp;' + nome + '&nbsp;</b>';
        document.getElementById("div_guardar_nome").disabled = true;
        document.getElementById("div_guardar_nome").querySelector('svg').style.stroke = "gray";
    }
    
}





////////////////////////////////////////////////
// Controlar inserção de caracteres especiais //
////////////////////////////////////////////////
function restrictSpecialChars(input) {
    // Replace any non-alphanumeric characters with empty string
    input.value = input.value.replace(/[^a-zA-Z0-9]/g, '');

    if(input.value == get_dev_name || input.value.length <= 0){
        document.getElementById("div_guardar_nome").disabled = true;
        document.getElementById("div_guardar_nome").querySelector('svg').style.stroke = "gray";
    }
    else{
        document.getElementById("div_guardar_nome").disabled = false;
        document.getElementById("div_guardar_nome").querySelector('svg').style.stroke = "#39be5a";
    }
}


//Abrir pagina para actualizar firmeware
function actualizarFirmeware() { 
    var urlDaPagina = window.location.protocol + "//" + window.location.host + window.location.pathname + 'update';
    
   // Seletor da div onde você deseja carregar a página
   var divContainer = document.getElementById('content');
   const sair = document.getElementById('div_sair_ota');
   // Carregar a página na div
   divContainer.innerHTML = '<object type="text/html" data="' + urlDaPagina + '" width="100%" height="100%"></object>';
  
   document.getElementById('div_set_ota').style.display = 'none';
   document.getElementById('div_sair_ota_1').style.display = 'flex';
   document.body.style.backgroundColor = "#FFFFFF";
}

//Voltar a pagina principal
function Voltar(){
    location.reload();
}