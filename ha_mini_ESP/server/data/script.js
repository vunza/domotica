

// variáveis globais //
const dev_ip = [
    "192.168.5.2",
    "192.168.5.3",
    "192.168.5.4",
    "192.168.5.5"
];

let get_post_data = "";
let get_parent_ip = "";
let svg_wifi_off = '<svg viewBox="0 0 24 24"><g fill="none"><circle cx="12" cy="18" r="2" stroke="#000" stroke-width="2"/><path stroke="#000" stroke-opacity=".2" stroke-width="2" d="M7.75735931,13.7573593 C10.1005051,11.4142136 13.8994949,11.4142136 16.2426407,13.7573593 M4.92893219,10.9289322 C8.83417511,7.02368927 15.1658249,7.02368927 19.0710678,10.9289322 M2.10050506,8.10050506 C7.56784515,2.63316498 16.4321549,2.63316498 21.8994949,8.10050506" opacity=".8"/></g></svg>';
let svg_cfg = '<svg viewBox="0 0 52 52" data-name="Layer 1" id="Layer_1" ><path d="M32.26,52a2.92,2.92,0,0,1-2.37-1.21l-3.48-4.73h-.82L22.11,50.8a2.93,2.93,0,0,1-3.5,1L13,49.45a2.93,2.93,0,0,1-1.78-3.17l.89-5.8-.59-.59-5.8.89A2.93,2.93,0,0,1,2.55,39L.23,33.39a2.92,2.92,0,0,1,1-3.5l4.73-3.48v-.82L1.21,22.11a2.92,2.92,0,0,1-1-3.5L2.55,13a2.93,2.93,0,0,1,3.17-1.78l5.8.89.59-.59-.89-5.8A2.93,2.93,0,0,1,13,2.55L18.61.23a2.93,2.93,0,0,1,3.5,1l3.48,4.74h.82l3.48-4.73a2.93,2.93,0,0,1,3.5-1L39,2.55a2.93,2.93,0,0,1,1.78,3.17l-.89,5.8.59.59,5.8-.89A2.93,2.93,0,0,1,49.45,13l2.32,5.61a2.92,2.92,0,0,1-1,3.5l-4.73,3.48v.82l4.73,3.48a2.92,2.92,0,0,1,1,3.5L49.45,39a2.93,2.93,0,0,1-3.17,1.78l-5.8-.89-.59.59.89,5.8A2.93,2.93,0,0,1,39,49.45l-5.61,2.32A2.82,2.82,0,0,1,32.26,52Zm-17-5.93,4.09,1.69,3.3-4.49a2.94,2.94,0,0,1,2.37-1.21H27a3,3,0,0,1,2.37,1.2l3.3,4.5,4.09-1.69-.85-5.51A3,3,0,0,1,36.68,38L38,36.69a3,3,0,0,1,2.53-.83l5.51.85,1.69-4.09-4.49-3.3A2.94,2.94,0,0,1,42.06,27v-1.9a2.94,2.94,0,0,1,1.21-2.37l4.49-3.3L46.07,15.3l-5.51.84A3,3,0,0,1,38,15.31L36.69,14a3,3,0,0,1-.83-2.53l.85-5.51L32.62,4.24l-3.3,4.5A3,3,0,0,1,27,9.94h-1.9a2.94,2.94,0,0,1-2.37-1.21l-3.3-4.49L15.29,5.93l.85,5.51A3,3,0,0,1,15.31,14L14,15.31a3,3,0,0,1-2.53.83L5.93,15.3,4.24,19.38l4.49,3.3a2.94,2.94,0,0,1,1.21,2.37V27a2.94,2.94,0,0,1-1.21,2.37l-4.49,3.3,1.69,4.09,5.51-.85a2.94,2.94,0,0,1,2.53.83L15.31,38a2.94,2.94,0,0,1,.83,2.53Zm31.6-30.9Zm-.31-2h0ZM26,38A12,12,0,1,1,38,26,12,12,0,0,1,26,38Zm0-20a8,8,0,1,0,8,8A8,8,0,0,0,26,18Z"/></svg>';
let svg_save = '<svg viewBox="0 0 24 24"><g><path fill="none" d="M0 0h24v24H0z"/><path d="M4 3h14l2.707 2.707a1 1 0 0 1 .293.707V20a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1zm3 1v5h9V4H7zm-1 8v7h12v-7H6zm7-7h2v3h-2V5z"/></g></svg>';
let svg_refresh = '<svg viewBox="0 0 512 512"><title>ionicons-v5-b</title><path d="M288,193s12.18-6-32-6a80,80,0,1,0,80,80" style="fill:none;stroke:#000;stroke-linecap:round;stroke-miterlimit:10;stroke-width:28px"/><polyline points="256 149 296 189 256 229" style="fill:none;stroke:#000;stroke-linecap:round;stroke-linejoin:round;stroke-width:28px"/><path d="M256,64C150,64,64,150,64,256s86,192,192,192,192-86,192-192S362,64,256,64Z" style="fill:none;stroke:#000;stroke-miterlimit:10;stroke-width:32px"/></svg>';
let svg_menu = '<svg viewBox="0 0 24 24"><g id="web-app" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd"><g id="app-menu" fill="#000000"><path d="M16,16 L20,16 L20,20 L16,20 L16,16 Z M10,16 L14,16 L14,20 L10,20 L10,16 Z M4,16 L8,16 L8,20 L4,20 L4,16 Z M16,10 L20,10 L20,14 L16,14 L16,10 Z M10,10 L14,10 L14,14 L10,14 L10,10 Z M4,10 L8,10 L8,14 L4,14 L4,10 Z M16,4 L20,4 L20,8 L16,8 L16,4 Z M10,4 L14,4 L14,8 L10,8 L10,4 Z M4,4 L8,4 L8,8 L4,8 L4,4 Z" id="Shape"></path></g></g></svg>';


// Actualiza os elementos cada X segundos  //
setInterval(function () {

    for (let i = 0; i < dev_ip.length; i++) {
        // Actualiza o estado dos dispositivos
        actualizarEstado(dev_ip[i]);

        // Actualiza os nomes dos Dispositivos    
        actualizarNomeDevice(dev_ip[i]);
    }

}, 5000);





// Acções a serem executadas ao carre3gar a página //
document.addEventListener("DOMContentLoaded", function () {

    for (let i = 0; i < dev_ip.length; i++) {
        // Actualiza data e hora dos Dispositivos    
        actualizaHoraData(dev_ip[i]);

        // Actualiza o estado dos dispositivos
        actualizarEstado(dev_ip[i]);

        // Actualiza os nomes dos Dispositivos    
        actualizarNomeDevice(dev_ip[i]);
    }


    // Atribui "Eventlistaner" aos radios buttons "select_timer".    
    const radios = document.querySelectorAll('input[name="select_timer"]');
    for (const radio of radios) {
        radio.onclick = (e) => {
            // Restituir estado do botão "Guardar"
            document.getElementById("btn_guardar_cfg").disabled = false;
            document.getElementById("btn_guardar_cfg").style.backgroundColor = "#47c266";
            obtemConfigTimer(get_parent_ip, e.target.value);
        }
    }

    // Image SVG no botão alterar nome do dispositivo
    document.getElementById("div_save_name").innerHTML = svg_save;

    // Image SVG no Botões Menu e Refresh
    document.getElementById("div_menu").innerHTML = svg_menu;
    document.getElementById("div_refresh").innerHTML = svg_refresh;

    // Obter Hora actual
    obter_data_hora();	

});




// Função para alterar mgem dos botões //
function alterarImagem(var_estado) {

    // Atribui imagem em função do estado  
    let imagem = "";
    let item_color = "";

    if (var_estado == "OFF") {
        item_color = "lightgray";
    }
    else if (var_estado == "ON") {
        item_color = "#e1e437";
    }

    return '<svg fill="' + item_color + '" viewBox="-80 0 512 512" ><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>';

}



// Actualiazar o estado do PIN //
function actualizarEstado(var_ip) {

    let url = "http://" + var_ip + "/cm?cmnd=Power%20STATE";

    var http = new XMLHttpRequest();
    http.open('GET', url, true);

    http.onload = function () {

        const objson = JSON.parse(this.responseText);
        let imagem = alterarImagem(objson.POWER);
        document.getElementById(var_ip).querySelectorAll(".svg_item")[0].innerHTML = imagem;

        document.getElementById(var_ip).querySelectorAll(".svg_div_cfg")[0].innerHTML = svg_cfg;

        document.getElementById(var_ip).disabled = false;
        document.getElementById(var_ip).style.pointerEvents = 'auto';
        document.getElementById(var_ip).style.backgroundColor = "#fffdfd";

    } // success case

    http.onerror = function () {
        document.getElementById(var_ip).disabled = true;
        document.getElementById(var_ip).style.pointerEvents = 'none';
        document.getElementById(var_ip).style.backgroundColor = "darkgray";
        document.getElementById(var_ip).querySelectorAll(".svg_item")[0].innerHTML = svg_wifi_off;
        document.getElementById(var_ip).querySelectorAll(".svg_div_cfg")[0].innerHTML = "";

    } // failure case
    http.send();

}



// Função para alternar o estado de PIN //
function togglePIN(element) {

    var http = new XMLHttpRequest();
    var url = 'http://' + element.parentNode.id + '/cm?cmnd=Power%20Toggle';
    http.open('GET', url, true);

    http.onload = function () {
        const objson = JSON.parse(this.responseText);
        element.childNodes[0].src = alterarImagem(objson.POWER);

    } // success case

    http.onerror = function () { console.log(http.responseText); } // failure case
    http.send();

    // Actualiza a imagem do elemento    
    actualizarEstado(element.parentNode.id);
}




// Actualizar Hora e Data dos dispositivos //
function actualizaHoraData(var_ip_url) {

    let xhttp = new XMLHttpRequest();
    let var_url = "";

    const date = new Date();
    const timestampInMs = date.getTime();
    const unixTimestamp = Math.floor(date.getTime() / 1000);

    var_url = "http://" + var_ip_url + "/cm?cmnd=Time%20" + unixTimestamp;

    var http = new XMLHttpRequest();
    http.open('GET', var_url, true);
    //http.onload = function() { console.log(http.responseText); } // success case
    http.onerror = function () { console.log(http.responseText); } // failure case
    http.send();

}




// Fechar DIV de configurações //
function exibirElemento(obj_caller, elemento) {
    document.getElementById(elemento).style.display = 'block';
    get_parent_ip = obj_caller.parentNode.id;

    // Desactiva elemnetos da DIV de configuração
    resetElementos();

    // Exibe na input text o nome do dispositivo
    obterNomeDevice(obj_caller);

    //alert(get_parent_ip); // debug
}




// Fechar DIV de configurações //
function ocultarElemento(elemento) {
    document.getElementById(elemento).style.display = 'none';
    document.getElementById("btn_guardar_cfg").disabled = true;
}



// Guardar configurações //
function guardarConfigs() {

    let txt_nome = "";
    let chk_activar = "";
    let timer = "";
    let saida = "";
    let accao = "";
    let hora = "";
    let dias_semana = "";
    let cmd_Backlog = "";

    txt_nome = document.getElementById("txt_nome").value;
    document.getElementById("chk_activar").checked ? chk_activar = 1 : chk_activar = 0;
    timer = document.querySelector('input[name=select_timer]:checked');
    let sel_saida = document.getElementById("select_saida");
    let sel_accao = document.getElementById("select_accao");
    saida = sel_saida.options[sel_saida.selectedIndex].text;
    sel_accao.options[sel_accao.selectedIndex].text == "Ligar" ? accao = "1" : accao = "0";
    hora = document.getElementById("time_hora").value;
    dias_semana = obtemDiaSemana();

    cmd_Backlog += "Backlog%20Timers%20on%3BTimer" + timer.value + "%20{%22Enable%22:";
    cmd_Backlog += chk_activar + ",%22Mode%22:0,%22Time%22:%22" + hora;
    cmd_Backlog += "%22,%22Window%22:0,%22Days%22:%22" + dias_semana;
    cmd_Backlog += "%22,%22Repeat%22:1,%22Output%22:" + saida + ",%22Action%22:" + accao + "}";

    configuraTimer(get_parent_ip, cmd_Backlog);

    //alert(cmd_Backlog);
    alert("Processo Concluido!"); // debug    

    // Limpa items da DIV de configuração
    resetElementos();
}



///////////////////////
// Configurar Timers //
///////////////////////
function configuraTimer(var_ip_url, cmd_backlog) {

    let xhttp = new XMLHttpRequest();
    let var_url = "";

    var_url = "http://" + var_ip_url + "/cm?cmnd=" + cmd_backlog;

    var http = new XMLHttpRequest();
    http.open('GET', var_url, true);
    //http.onload = function() { console.log(http.responseText); } // success case
    http.onerror = function () { console.log(http.responseText); } // failure case
    http.send();

}



//////////////////////////
// Obter dias de semana //
//////////////////////////
function obtemDiaSemana() {

    let weekDays = "";

    var checBtn = document.getElementsByName("sel_week_day");

    for (let i = 0; i < checBtn.length; i++) {
        checBtn[i].checked ? weekDays += "1" : weekDays += "0";
    }

    return weekDays;
}



// Obter configuraçõs do TimerX //
function obtemConfigTimer(ip_parent, TimerX) {

    let xhttp = new XMLHttpRequest();

    let var_url = "http://" + ip_parent + "/cm?cmnd=Timer" + TimerX;

    var http = new XMLHttpRequest();
    http.open('GET', var_url, true);

    http.onload = function () {

        let chck_nchck = "";
        let indx_saida = "";

        //alert(http.responseText); // Debug
        const objson = JSON.parse(this.responseText);

        objson["Timer" + TimerX]["Enable"] ? chck_nchck = true : chck_nchck = false;
        indx_saida = objson["Timer" + TimerX]["Output"] - 1;

        if (indx_saida = -1) indx_saida = 0;

        document.getElementById("chk_activar").checked = chck_nchck;
        document.getElementById('select_saida').selectedIndex = indx_saida;

        // (*J*) Se existir mais de 2 duas acções a lógica deve mudar
        document.getElementById('select_accao').selectedIndex = objson["Timer" + TimerX]["Action"];


        document.getElementById('time_hora').value = objson["Timer" + TimerX]["Time"];

        var checBtn = document.getElementsByName("sel_week_day");
        for (let i = 0; i < checBtn.length; i++) {
            checBtn[i].checked = parseInt(objson["Timer" + TimerX]["Days"][i]);
        }

    } // success case

    http.onerror = function () { console.log(http.responseText); } // failure case
    http.send();

}



// Obter nome do Device //
function obterNomeDevice(obj_pai_do_parag) {

    let txt_name = obj_pai_do_parag.parentNode.querySelectorAll("p")[0].innerHTML;
    document.getElementById("txt_nome").value = txt_name;
    //obj_div_dev = obj_pai_do_parag;
}



// Alterar nome do Device //
function alterarNomeDevice() {

    let dev_name = document.getElementById("txt_nome").value;

    if (dev_name == "" || dev_name == null) {
        alert("Indique um Nome!");
        return;
    }

    let xhttp = new XMLHttpRequest();
    let var_url = "";

    var_url = "http://" + get_parent_ip + "/cm?cmnd=DeviceName%20" + dev_name;

    var http = new XMLHttpRequest();
    http.open('GET', var_url, true);

    http.onload = function () {

        // Actualiza nome do Device
        const objson = JSON.parse(this.responseText);
        let obj_pai = document.getElementById(get_parent_ip);
        obj_pai.querySelectorAll("p")[0].innerHTML = objson.DeviceName;

        alert("Processo concluido!");

    } // success case

    http.onerror = function () { console.log(http.responseText); } // failure case
    http.send();
}




// Actualizar nome do Device //
function actualizarNomeDevice(parent_ip) {

    let xhttp = new XMLHttpRequest();
    let var_url = "";

    var_url = "http://" + parent_ip + "/cm?cmnd=DeviceName";

    var http = new XMLHttpRequest();
    http.open('GET', var_url, true);

    http.onload = function () {

        //alert(http.responseText); 

        // Actualiza nome do Device
        const objson = JSON.parse(this.responseText);
        let obj_pai = document.getElementById(parent_ip);
        obj_pai.querySelectorAll("p")[0].innerHTML = objson.DeviceName;;

    } // success case

    http.onerror = function () { console.log(http.responseText); } // failure case
    http.send();

    //alert(dev_name); // Debug
}



// Reset elementos da DIV de configuração //
function resetElementos() {
    // Mudar estado dos elementos
    document.getElementById("btn_guardar_cfg").disabled = true;
    document.getElementById("btn_guardar_cfg").style.backgroundColor = "gray";

    var radList = document.getElementsByName('select_timer');
    for (var i = 0; i < radList.length; i++) { radList[i].checked = false; }

    var chckList = document.getElementsByName('sel_week_day');
    for (var i = 0; i < chckList.length; i++) { chckList[i].checked = false; }

    document.getElementById("chk_activar").checked = false;
    document.getElementById('select_saida').selectedIndex = -1;
    document.getElementById('select_accao').selectedIndex = -1;
    document.getElementById("time_hora").value = 0;
}



// Actualização/Rfresh da Página //
function actualizarPagina() {
    window.location.reload();
}





// Obter Hora e Data //
function display_c(){   
    var refresh = 1000; 
    mytime = setTimeout('obter_data_hora()', refresh);
}

function obter_data_hora() {
    var strcount  
    const weekday = ["Dom","Seg","Ter","Qua","Qui","Sex","Sab"];
    var currentdate = new Date();
	//var formato = '<font color="black" face="Verdana, Geneva, sans-serif" size="+1">' + currentdate.toDateString() + '</font>';
	var formato = `<font color="white" face="Verdana, Geneva, sans-serif" size="+1">${weekday[currentdate.getDay()]}</font>`;
    formato += '<font color="yellow" face="Verdana, Geneva, sans-serif" size="+1">';	
		
	var hours = currentdate.getHours();
	var minutes = currentdate.getMinutes();
	var seconds = currentdate.getSeconds();
	
	if (hours < 10) hours = "0" + hours;
	if (minutes < 10) minutes = "0" + minutes;
	if (seconds < 10) seconds = "0" + seconds;

	formato += " " + hours + ":" + minutes + ":" + seconds;
	
	formato += "</font>";
  
    document.getElementById("hora_actual").innerHTML = formato;

    tt = display_c(); 
}



