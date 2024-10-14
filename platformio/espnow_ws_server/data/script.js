// variáveis globais
const MARCAZB = "00";           // Deve coincidir com valor do codigo arduino
const QTD_TMRS_ESPNOW = 16;     // Deve coincidir com valor do codigo arduino
const QTD_TMRS_ZB = 4;          // Deve coincidir com valor do codigo arduino
var get_srvr_id ='';
var save_dev_name = '';
var save_key_id = '';
var get_hub_ir_nome = '';
var save_dev_id = '';
var reconnect_time = 1000; // Tempo em milisegundos
var DEVICES_LIST_FROM_ESP = [];
var DEVICES_LIST_AUX = [];
var redes_wifi = [];
var webSocket = '';
var get_aparelho = '';
var modo_prog = false;
var intervalpgrId = '';
var isDragging = false;
var total_grids_dom = 0;
const porta_servidor_python = 8000;
const ip_servidor_python = '127.0.0.1';
var menu_inferior_modo = "LOCKED";
const svg_locked = '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="green" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect><path d="M7 11V7a5 5 0 0 1 10 0v4"></path></svg>';
const ctrl_r_svg = '<svg height="45px" width="45px" version="1.1" id="Layer_1" viewBox="0 0 505 505" xml:space="preserve" fill="#000000"><g id="SVGRepo_bgCarrier" stroke-width="0"></g><g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g><g id="SVGRepo_iconCarrier"> <circle style="fill:#ffffff;" cx="252.5" cy="252.5" r="252.5"></circle> <circle style="fill:#FFFFFF;" cx="252.5" cy="95.2" r="20.2"></circle> <path style="fill:#CED5E0;" d="M289.8,430h-74.7c-17.7,0-32.3-13.6-33.7-31.2l-21.1-272.1c-1.8-22.9,16.3-42.5,39.3-42.5h105.6 c23,0,41.1,19.6,39.3,42.5l-21,272.1C322.2,416.4,307.5,430,289.8,430z"></path> <g> <path style="fill:#54C0EB;" d="M230,340.4l-17-17c-7.1,8.7-11.3,19.8-11.3,31.9s4.2,23.2,11.3,31.9l17-17c-2.8-4.3-4.5-9.4-4.5-15 S227.1,344.7,230,340.4z M252.5,304.6c-12.1,0-23.2,4.2-31.9,11.3l17,17c4.3-2.8,9.4-4.5,15-4.5c5.5,0,10.7,1.7,15,4.5l17-17 C275.7,308.8,264.6,304.6,252.5,304.6z M292,323.4l-17,17c2.8,4.3,4.5,9.4,4.5,15s-1.7,10.7-4.5,15l17,17 c7.1-8.7,11.3-19.8,11.3-31.9C303.3,343.2,299.1,332.1,292,323.4z M252.5,382.4c-5.5,0-10.7-1.7-15-4.5l-17,17 c8.7,7.1,19.8,11.3,31.9,11.3c12.1,0,23.2-4.2,31.9-11.3l-17-17C263.2,380.7,258,382.4,252.5,382.4z"></path> <circle style="fill:#54C0EB;" cx="252.5" cy="355.4" r="16.6"></circle> </g> <circle style="fill:#FF7058;" cx="198.2" cy="124.7" r="15.2"></circle> <g> <circle style="fill:#ACB3BA;" cx="206.4" cy="175.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="252.5" cy="175.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="298.6" cy="175.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="206.4" cy="217.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="252.5" cy="217.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="298.6" cy="217.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="206.4" cy="259.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="252.5" cy="259.2" r="14.8"></circle> <circle style="fill:#ACB3BA;" cx="298.6" cy="259.2" r="14.8"></circle> </g> </g></svg>';
const svg_list = '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="8" y1="6" x2="21" y2="6"></line><line x1="8" y1="12" x2="21" y2="12"></line><line x1="8" y1="18" x2="21" y2="18"></line><line x1="3" y1="6" x2="3.01" y2="6"></line><line x1="3" y1="12" x2="3.01" y2="12"></line><line x1="3" y1="18" x2="3.01" y2="18"></line></svg>';
const svg_close = '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="18" y1="6" x2="6" y2="18"></line><line x1="6" y1="6" x2="18" y2="18"></line></svg>';
const svg_unlocked = '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="red" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect><path d="M7 11V7a5 5 0 0 1 9.9-1"></path></svg>';
const svg_save = '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path><polyline points="17 21 17 13 7 13 7 21"></polyline><polyline points="7 3 7 8 15 8"></polyline></svg>';
const http_prot = window.location.protocol; // Guarda http:/https:

const db_name = "domoticaDB";
const tbl_name = "dispositivos";
const devices_row_key = 1;
const db_version = 1;
let db;

const aparelhos = {
    // (*J*) Ao alterar este array deve-se ajustar também os arquivos
    // .html (linhas 419 ...) e .css (linha 1078)
    ARC:"Ar Condicionado",
    VNT:"Ventilador",
    TLV:"Televisor",
    DST:"DsTV",
    ZAP:"Zap",
    RAD:"Sistema de Som"
};


connect();

function connect() {    
    var gwUrl =  "";

    if (http_prot == 'http:') {
        gwUrl = "ws://" + location.host + "/ws";
    }
    else if (http_prot == 'https:'){
        gwUrl = "wss://" + location.host + "/wss";
    }

    try{
        webSocket = new WebSocket(gwUrl);
    }
    catch(e){
        console.log(`Erro 7: ${e.message}`);
    }
    
   
// Evento onopen
webSocket.onopen = function (e) {
    const date = new Date();
    const unixTimestamp = Math.floor(date.getTime() / 1000);
    const data_json = {
        MSG: unixTimestamp,
        CMD: "UPT",
        MAC: window.location.protocol,
        ID: 0,
        PST: 2,
        WCH: 0
    };
    webSocket.send(JSON.stringify(data_json));
}// Fim de webSocket.onopen 

// Evento onclose
webSocket.onclose = function (e) {
    console.log(`Conexão terminada, tentar restabeler em ${reconnect_time/1000} segundo(s)`);
    setTimeout(function() {
        connect();
    }, reconnect_time);
}// Fim do Evento onclose


// Evento onmessage
webSocket.onmessage = async function (d) {    
       
    // Checar consistência do JSON proveniente do Servidor ESPxx    
    var msg; 

    try{
        msg = JSON.parse(d.data);             
    }
    catch(err){
        console.log(`Erro parse_json: ${d.data}`);
        console.log(`Erro parse_json: ${err.message}`);
        return;
    }

    // Checar se o Servidor existe na DOM, inclui-lo
    try{
        get_srvr_id = msg.MSG_TYPE[0]; 
        const elem = document.getElementById(get_srvr_id);         
        if (typeof(elem) == 'undefined' || elem == null){
            CriarDevice(get_srvr_id);
        }                                     
    }
    catch(err){
        //console.log(`Erro add_server_to_dom: ${err.message}`);       
    }          
    

    // Registar elementos
    if ( msg.CMD == "REG" && msg ) {        
        let dev_obj = null;
        const size = msg.MSG_TYPE.length; 
        get_srvr_id = msg.MSG_TYPE[0];           
        total_grids_dom = msg.MSG_TYPE.length; 
 
        // Guarda array de ID's dos Dispositivos, vindos do Servidpr ESP
        try{
            if(get_srvr_id.toString().length === 7){                                  
                for (let i = 0; i < size; i++) {                    
                    dev_obj = { Id: msg.MSG_TYPE[i], Nome: '', Historico: '', Alive: 0 };                    
                    DEVICES_LIST_FROM_ESP[i] = dev_obj;                                          
                }                    
            }                
        }
        catch(err){
            console.log(`Erro reg_devs: ${err.message}`);             
        }         

        // Criar cards com os ID's dos dispositivos fora da lista do BD/Servidor Python nao fazem parte do BD.
        for (let j = 0; j < DEVICES_LIST_FROM_ESP.length; j++) {                  
            try{
                const elem = document.getElementById(DEVICES_LIST_FROM_ESP[j].Id);
                if(!elem) {
                    CriarDevice(DEVICES_LIST_FROM_ESP[j].Id); 
                    // Atribuir numero de ordem a card
                    //document.getElementById(DEVICES_LIST_FROM_ESP[j].Id + 100).style.order = j;
                }                    
            }
            catch(e){
               console.log(`Erro create_devs_2: ${e.message}`);          
            }    
        }    
        
    }
    else if (msg.CMD == "STT") {   
       
        let pai = document.getElementById(get_srvr_id);
        let filho = pai.querySelector(".srvr_indicator");

        // Se for Bridge ZigBee, ocultar a card correspondente, ocultar servidor
        if (get_srvr_id % 10 === 4) {
          pai.style.display = "none";
        } else if (filho) {
          filho.style.display = "block";
        }             
        
        //actualiza_pos_bd();
        ActualizaCard(msg.ID, msg); 
    }
    else if (msg.CMD == "SWS") {          
        var jsondata = JSON.parse(msg.MSG);  
        document.getElementById("txt_wifi_ip").value = jsondata.IPAddress;       
        if(jsondata.WifiState == 1){
            document.getElementById("div_ssid_svg").querySelector('svg').style.stroke = "green";
        }
        else if(jsondata.WifiState == 0){
            document.getElementById("div_ssid_svg").querySelector('svg').style.stroke = "lightgray";
        }
        
    }
    else if (msg.CMD == "ECHO") {
       console.log(msg);     
    }
    else if (msg.CMD == "GDT") {
        const date = new Date();
        const unixTimestamp = Math.floor(date.getTime() / 1000);
        EnviaJSON(unixTimestamp, "SDT", "", msg.ID, 0, 0);        
    }
    else if (msg.CMD == "TCG" || msg.CMD == "TCA") {              
        fill_dlg_timers(msg);            
    }
    else if (msg.CMD == "SAS" || msg.CMD == "SAC") {  
        
        var accao_timer = msg.WCH % 10;
                    
        // msg.MAC -- Nome
        // msg.MSG -- Historico
        // msg.WCH -- Alive
        // Guarda o ID do server, usado par exibir DLG WiFi
        if (msg.CMD == "SAS") {
            get_srvr_id = msg.ID;
        }   
        
        // Actualiza dados (Nome, Historico e Estado) dos elementos
        //const cards = [];
        document.querySelectorAll('.grid_container').forEach( async function (card) {
        //document.querySelectorAll('.grid_container').forEach( function (card) {    
   
            var Id = card.id - 100;    

            // Remover, da DOM, elementos duplicados
            removeDuplicateEelementes('grid_container');

            if(msg.ID == Id){                
                var pai = document.getElementById(Id);
                pai.style.pointerEvents = "auto";
                pai.disabled = false;
                pai.style.opacity = 1;  

                let num = Id;
                let numStr = num.toString();
                const marca = `${numStr[4]}${numStr[5]}`;
                if(marca !== MARCAZB ){
                    pai.querySelector('.div_nome').innerHTML = msg.MAC;    
                }
                
                pai.querySelector('.div_historico').innerHTML = msg.MSG;                    
                pai.querySelector('.div_img').innerHTML = alterarImagem(msg.PST, Id); 
                                     
                const devobj = DEVICES_LIST_AUX.find(objeto => objeto['Id'] === msg.ID);
                if(devobj !== 'undefined' && (msg.MAC === '' || msg.MAC === '[Nome]')){                   
                    pai.querySelector('.div_nome').innerHTML = devobj.Nome;                     
                }
                

                // Alterar cor do icon do historico
                if(accao_timer == 0){
                    pai.querySelector('.svg_clk').setAttribute("fill", "red");   
                }
                else if(accao_timer == 1){
                    pai.querySelector('.svg_clk').setAttribute("fill", "green");   
                }
                else{
                    pai.querySelector('.svg_clk').setAttribute("fill", "gray"); 
                }   
                                
                // Actualizar dados na idexedDB //               
                for (let cont = 0; cont < DEVICES_LIST_FROM_ESP.length; cont++) {
                    if (DEVICES_LIST_FROM_ESP[cont] != null && DEVICES_LIST_FROM_ESP[cont].Id == Id && get_srvr_id != Id) {
                        DEVICES_LIST_FROM_ESP[cont].Nome = msg.MAC;
                        DEVICES_LIST_FROM_ESP[cont].Historico = msg.MSG;                           
                    }   
                    
                    if(DEVICES_LIST_FROM_ESP[cont] == null){
                        DEVICES_LIST_FROM_ESP.splice(cont, cont);
                    }                   
                }

                if( DEVICES_LIST_FROM_ESP.length > 0){

                    // Actualizar indexedDB
                    actualizarRegisto(devices_row_key, DEVICES_LIST_FROM_ESP);
                          
                    // Carrega a lista de dispositivos guardados na indexedDB
                    var size_obj = await lerRegisto(devices_row_key); 
                    
                    // Criar dispositivo atraves do registo da indexedDB
                    var getId = 0;
                    var getName = '';
                    for(var cont = 0; cont < size_obj.length; cont++){                        
                        var elemento = document.getElementById( (size_obj[cont].Id + 100) );
                        if( elemento){
                            getId = 0;                           
                        }
                        else{
                            getId = size_obj[cont].Id;
                            getName = size_obj[cont].Nome;
                        }                        
                    }
                    
                    if(getId > 0){
                        CriarDevice(getId);

                        const data_json = {
                            MSG: "",
                            CMD: "AFO",
                            MAC: getName,
                            ID: getId,
                            PST: 0,
                            WCH: 0
                        };
            
                        webSocket.send(JSON.stringify(data_json));
                    }

                    // Elimina null
                    for (let cont = 0; cont < size_obj.length; cont++) {
                        if (size_obj[cont] === null) {
                            size_obj.splice(cont, 1);                           
                        }   
                            
                        DEVICES_LIST_FROM_ESP = size_obj;
                    }

                    // Elimina posicoes acima do total de boards    
                    if(size_obj.length > total_grids_dom){
                        for (let cont = 0; cont < size_obj.length; cont++) {
                            if (cont >= total_grids_dom) {
                                size_obj.splice(cont, 1);                           
                            }                    
                        }

                        DEVICES_LIST_FROM_ESP = size_obj;                       
                    }

                
                    
                    // Eliminar elementos duplicados
                    const arraySemDuplicatas = DEVICES_LIST_FROM_ESP.filter((item, index) => DEVICES_LIST_FROM_ESP.indexOf(item) === index);
                        
                    // Conforma dispositivos da dom com da ndexedDB
                    var dev_existe = false;
                    if(total_grids_dom != size_obj.length){
                        document.querySelectorAll('.grid_container').forEach(function (card) {
                                
                            var devId = card.id - 100;
                            var pai = document.getElementById(devId);
                            const devNome = pai.querySelector('.div_nome').innerHTML;
                            const devHistorico = pai.querySelector('.div_historico').innerHTML;
                            const dev_obj = { Id: devId, Nome: devNome, Historico: devHistorico, Alive: 0};
                                                        
                                
                            for(let i = 0; i < size_obj.length; i++){
                                if(size_obj[i].Id == dev_obj.Id){ 
                                    dev_existe = true;                                      
                                    break;
                                }
                                else{
                                    dev_existe = false;  
                                }
                            } 
                                
                            // Guarda o device se nao existe no array da localstorage e se onome nao é "lixo", restos de operação anterior
                            if(dev_existe == false && devNome.indexOf('Nome') == -1 && devNome.indexOf('"}') === -1 && devNome.indexOf('}') === -1 && devNome.length > 0){
                                size_obj.push(dev_obj);
                                dev_existe = true;                                
                            }
                        });

                        DEVICES_LIST_FROM_ESP = size_obj;
                    }

                    for( var cont = 0; cont < DEVICES_LIST_FROM_ESP.length; cont++){
                        if(DEVICES_LIST_FROM_ESP[cont].Nome == ''){
                            DEVICES_LIST_FROM_ESP.splice(cont, 1);
                        }
                    }
                    
                    // Guardar elementos ordenados na indexedDB                
                    actualizarRegisto(devices_row_key, DEVICES_LIST_FROM_ESP);                                                                            
                    //console.log(DEVICES_LIST_FROM_ESP);
                    
                }
            }             
                        
        });
        
    }
    else if (msg.CMD == "OFF") {               
        let pai = document.getElementById(msg.ID);
        if (typeof(pai) == 'undefined' || pai == null){

            // Solicita o estado dos devices e envia DT actual, para actualização dos mesmos, caso necessário.    
            const date = new Date();
            const unixTimestamp = Math.floor(date.getTime() / 1000);

            const data_json = {
                MSG: unixTimestamp,
                CMD: "UPT",
                MAC: "",
                ID: 0,
                PST: 0,
                WCH: 0
            };

            webSocket.send(JSON.stringify(data_json));

            return;
        }
        else{
            try{

                // Nao processar id do servidor nem os invalidos
                if( get_srvr_id == msg.ID || (msg.ID).toString().length < 7){
                    return;
                } 

                let pai = document.getElementById(msg.ID);
                pai.style.pointerEvents = "none";
                pai.disabled = true;
                pai.style.opacity = 0.3;                              
                pai.querySelector('.div_img').innerHTML = alterarImagem(2, msg.ID);

                // Obter Nome e Historico do dispositivo na indexedDB          
                for(let cont = 0; cont < DEVICES_LIST_AUX.length; cont++){
                    if(DEVICES_LIST_AUX[cont].Id === msg.ID && DEVICES_LIST_AUX[cont].Nome !== ''){
                        pai.querySelector('.div_nome').innerHTML = DEVICES_LIST_AUX[cont].Nome;
                        pai.querySelector('.div_historico').innerHTML = DEVICES_LIST_AUX[cont].Historico;
                    }
                }

            }
            catch(e){
                console.log(`Erro 6: ${msg.ID}`);
                console.log(`Erro 6: ${e}`);
            }        
        }
       
    }
    else if (msg.CMD == "UDT"){
        // Solicita o estado dos devices e envia DT actual, para actualização dos mesmos, caso necessário.    
        const date = new Date();
        const unixTimestamp = Math.floor(date.getTime() / 1000);

        const data_json = {
            MSG: unixTimestamp,
            CMD: "UPT",
            MAC: "",
            ID: 0,
            PST: 0,
            WCH: 0
        };

        webSocket.send(JSON.stringify(data_json));
    } 
    else if (msg.CMD == "END"){
        showMsgBox("Número de conexões, ao Servidor, esgotado.");
    }    
    else if (msg.CMD == "GRA"){
        // MSG_TYPE: Tipo de Aparelho/Comandos (TV, AC, etc.)
        let comandos = msg.MSG;
        let arrayComados = comandos.split(",");

        document.querySelectorAll('.submenu').forEach(function(lielm) {
            lielm.querySelector(".liexec").style.display = 'none';
        });

        for(let cont = 0; cont < arrayComados.length; cont++){
            Object.entries(aparelhos).forEach(([key, value]) => {
                if(key == arrayComados[cont]){
                    
                    document.querySelectorAll('.submenu').forEach(function(lielm) {
                        if(lielm.innerText.includes(value)){   
                            lielm.querySelector(".liexec").style.display = 'block';
                            lielm.querySelector(".liexec").setAttribute("onclick", `enviarCodigoIR('${key}', '${value}');`); 
                        }                                         
                    });
                }
            });            
        }       

    }
    else if (msg.CMD == "NRG" && msg.MSG == "SNS"){
        // enrg.V -- Tensao
        // enrg.A -- Corrente
        // enrg.W -- Potencia
        // enrg.Wh -- Consumo
        // Actualiza dados da energia        
        const enrg = JSON.parse(msg.MAC);          
        const energia = `<b><font color='blue'>${enrg.V}V</font> | <font color='red'>${enrg.A}A</font></b>`;        
        let div_energy = document.getElementById('div_energy');
        if(div_energy){
            div_energy.innerHTML = energia;
            
            // Actualiza imagem de electrobomba
            let pai = document.getElementById(msg.ID);
            if(!pai) return;        
            pai.querySelector('.div_img').innerHTML = alterarImagem(msg.PST, msg.ID);
        }                       
    }  
    else if (msg.CMD == "KNP"){
        // MSG_TYPE: Tecla(ONE, PWR, etc.)
        save_key_id = msg.MSG;
        if(modo_prog == false){
            showMsgBox("A Tecla pressionada não está programada!");
        }
        else if(get_aparelho.length > 0 && modo_prog == true){
            abrirPainelIR(save_key_id, "PROGRAMAR");         
        }    
    }  
    else if (msg.CMD == "UKA"){
        // MSG_TYPE: Tecla(ONE, PWR, etc.)
        save_key_id = msg.MSG;
        document.getElementById('btn_fechar').click();
        showMsgBox("A Tecla indicada foi desprogramada.");
    }
    else if (msg.CMD == "USD"){
        // MSG_TYPE: Tecla(ONE, PWR, etc.)
        save_key_id = msg.MSG;
        EnviaJSON("", "SRI", "", save_dev_id, 2, 0); 
        document.getElementById('btn_fechar').click();
        showMsgBox("Código Infra-Vermelho inválido!");
    }  
    else if (msg.CMD == "KAP"){
        // MSG_TYPE: Tecla(ONE, PWR, etc.)
        save_key_id = msg.MSG;
        if(modo_prog == false){
            EnviaJSON("", "SRI", "", save_dev_id, 2, 0);
        }
        else if(get_aparelho.length > 0 && modo_prog == true){
            abrirPainelIR(save_key_id, "DESPROGRAMAR");
        }      
    }  
    else if (msg.CMD == "KPG"){
        // MSG_TYPE: Tecla(ONE, PWR, etc.)
        save_key_id = msg.MSG;      
        // Envia comando SRI -- STOP RECORD IR CODE
        EnviaJSON("", "SRI", "", save_dev_id, 2, 0); 
        document.getElementById('btn_fechar').click();
        showMsgBox("Tecla programada com sucesso.");
    }
}// Fim de webSocket.onmessage

}


// Cria Devices
async function CriarDevice(Id) {

    var nome = '[Nome]';
    var historico = '00:00:00, 00/00/0000';
    const img_hub_zb = '<svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="orange" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="2"></circle><path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path></svg>';
    const img_lampada = '<svg fill="lightgray" viewBox="-80 0 512 512" ><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>';
    const img_bomba = '<svg fill="lightgray" stroke="none" ;vertical-align: middle;overflow: hidden;" viewBox="0 0 1024 1024"><path d="M544.7 703.8v-320c-35.3 0-64 26-64 58.2v203.7c0 32.1 28.6 58.1 64 58.1zM576.7 383.8h288v44.5h-288zM576.7 556.3h288v64h-288zM576.7 460.3h288v64h-288zM960.7 645.6V442c0-32.1-28.7-58.2-64-58.2v320c35.4 0 64-26 64-58.2zM640.7 351.8h160c17.7 0 32-14.3 32-32v-64h-224v64c0 17.7 14.3 32 32 32zM288.7 223.8h64v-64h-64v-64h-64v64h-64v64h64z"  /><path d="M832.7 799.8v-64h32v-83.5h-288v83.5h32v64h-320v-160h64v-64h32v32h64v-128h-64v32h-32v-64h-64v-128h64v-64h-192v64h64v128h-32v192h32v96h-128c-35.3 0-64 28.7-64 64v128h960.1v-128H832.7z m-64 0h-96v-64h96v64z"  /><path d="M160.7 639.8v-192c-35.3 0-64 28.7-64 64h-64v64h64c0 35.4 28.6 64 64 64z"  /></svg>';
    const svg_clock = '<svg class = "svg_clk" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><polyline points="12 6 12 12 16 14"></polyline></svg>';


    // Tira o último digito
    const tipo = Id % 10;
    let container = document.getElementById('card_wrapper');
    
    // Checar se o elemento ja existe
    const dev = document.getElementById(Id + 100);
    if(dev){ // Se Dev existe nao volta a criar
        return;
    }


    // Carrega a lista de dispositivos guardados na indexedDB
    DEVICES_LIST_AUX = await lerRegisto(devices_row_key); ; 

    // Verificar o nome do dispositivo na indexedDB, se existe atribui-lo... 
    const devobj = DEVICES_LIST_AUX.find((objeto) => objeto["Id"] === Id);
    if (devobj && (devobj.Nome !== '' || devobj.Nome === '[Nome]')) {
    //if (devobj !== 'undefined' && devobj.Nome != '') {    
        nome = devobj.Nome;
        historico = devobj.Historico;             
    }    
        
        
    if(tipo == 1){ // LAMPADA
        container.innerHTML += `<div id=${Id + 100} oncontextmenu="clickDireito(${Id});" class="grid_container">
                
        <div id=${Id} class="card card1" style="display: block;">
            <div class="srvr_indicator" id="indicador_servidor"></div>           
            <div class="text_container">                
                <table id="tbl_hist_clk">
                    <tr>
                        <th rowspan="3"><div class="div_img" id=${Id} onclick="on_click_btn(this.id);">${img_lampada}</div></th>
                    </tr>

                    <tr>
                        <td>
                            <div class="div_nome" id="div_nome">${nome}</div>
                        </td>
                        <td>
                            <div id=${Id} oncontextmenu="on_click_cfg(this.id);">
                                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>
                            </div>
                        </td>
                    </tr>
                    
                    <tr id="tr_historico">                       
                        <td>
                            <div class="div_img_clk" id="" onclick="">${svg_clock}</div>
                        </td>  
                        <td>  
                            <div class="div_historico" id="div_historico">${historico}</div>
                        </td>
                    </tr>
                </table>               
            </div>
        </div>
        </div>`;         
    }
    else if(tipo == 3){ // ELECTROBOMBA
        container.innerHTML += `<div id=${Id + 100} oncontextmenu="clickDireito(${Id});" class="grid_container">
        <div id=${Id} class="card card1" style="display: block;">
            <div class="srvr_indicator" id="indicador_servidor"></div>            
            <div class="text_container"> 

                <table id="tbl_hist_clk">
                    <tr>
                        <th rowspan="4"><div class="div_img" id=${Id} onclick="on_click_btn(this.id);">${img_bomba}</div></th>
                    </tr>

                    <tr>
                        <td>
                            <div class="div_nome" id="div_nome">${nome}</div>
                        </td>
                        <td>
                            <div id=${Id} oncontextmenu="on_click_cfg(this.id);">
                                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>
                            </div>
                        </td>
                    </tr>
                    
                    <tr id="tr_historico">  
                    
                        <td>
                            <div class="div_img_clk" id="" onclick="">${svg_clock}</div>
                        </td>  
                        <td>  
                            <div class="div_historico" id="div_historico_bomba">${historico}</div>
                        </td>                                    

                    </tr>

                    <tr id="tr_historico2">
                        <td>
                            <div class="div_historico" id="div_energy">${""}</div>  
                        </td>    
                    </tr>
                </table>           

            </div>
        </div>
        </div>`;
    }
    else if(tipo == 2){ // HUB IR           
        container.innerHTML += `<div id=${Id + 100} oncontextmenu="clickDireito(${Id});" class="grid_container">
        <div id=${Id} class="card card1" style="display: block;">
            <div class="srvr_indicator" id="indicador_servidor"></div>
           
            <div class="text_container">                
            <table id="tbl_hist_clk">
                <tr>
                    <th rowspan="3"><div class="div_img" id=${Id} onclick="on_click_btn(this.id);">${ctrl_r_svg}</div></th>
                </tr>

                <tr>
                    <td>
                        <div class="div_nome" id="div_nome">${nome}</div>
                    </td>
                    <td>
                        <div id=${Id} oncontextmenu="on_click_cfg(this.id);">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>
                         </div>
                    </td>
                </tr>
                
                <tr id="tr_historico">                       
                    <td>
                        <div class="div_img_clk" id="" onclick="">${svg_clock}</div>
                    </td>  
                    <td>  
                        <div class="div_historico" id="div_historico">${historico}</div>
                    </td>
                </tr>
            </table>               
        </div>
            
        </div>
        </div>`;
       
    }  
    else if(tipo == 4){ // BRDGE_ZB (*J*) style="display: none;     
        container.innerHTML += `<div id=${Id + 100} oncontextmenu="clickDireito(${Id});" class="grid_container">
        <div id=${Id} class="card card1" style="display: none;">
            <div class="srvr_indicator" id="indicador_servidor"></div>
            
            <div class="text_container">                
            <table id="tbl_hist_clk">
                <tr>
                    <th rowspan="3"><div class="div_img" id=${Id} onclick="on_click_btn(this.id);">${img_hub_zb}</div></th>
                </tr>

                <tr>
                    <td>
                        <div class="div_nome" id="div_nome">${nome}</div>
                    </td>
                    <td>
                        <div id=${Id} oncontextmenu="on_click_cfg(this.id);">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>
                         </div>
                    </td>
                </tr>
                
                <tr id="tr_historico">                       
                    <td>
                        <div class="div_img_clk" id="" onclick="">${svg_clock}</div>
                    </td>  
                    <td>  
                        <div class="div_historico" id="div_historico">${historico}</div>
                    </td>
                </tr>
            </table>               
        </div>

        </div>
        </div>`;
    }

}// Fim de CriarDevice()


// Click Botão da Card
function on_click_btn(btn_id){
    save_dev_id = btn_id;
    if( (btn_id % 10) == 1 || (btn_id % 10) == 3){ // LAMPADA e ELECTROBOMBA
        const card_state = document.getElementById('span_text_centro_menu_inferior').innerText;
        if(card_state == "Desbloquear") EnviaJSON("", 'TGL', "", btn_id, 0, 0);
    }  
    else if((btn_id % 10) == 2){ // VENTILADOR
        const card_state = document.getElementById('span_text_centro_menu_inferior').innerText;
        if(card_state == "Bloquear") return;
        EnviaJSON("", 'GRC', "", btn_id, 0, 0);
        document.getElementById('div_ctrl_r_wrappper').style.display = 'block';
        document.getElementById('card_wrapper').style.display = 'none';
        // Obter nome de Hub IR
        let pai = document.getElementById(btn_id);
        get_hub_ir_nome = pai.querySelector('.div_nome').innerHTML;

        document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_close;
        document.getElementById('span_text_centro_menu_inferior').innerText = 'Fechar';
        menu_inferior_modo = "FECHAR";
                
        modo_prog = true;        
    }
    
} // Fim de on_click_btn()

 
// Fechar Dlg CTRL Remoto 
function fecharControloR(){
    modo_prog = false;
    get_aparelho = '';
    document.getElementById('ctrl_remoto').style.display = 'none';
    document.getElementById('div_ctrl_r_wrappper').style.display = 'none';
    document.getElementById('card_wrapper').style.display = 'block';     
    // Envia comando SRI -- STOP RECORD IR CODE
    EnviaJSON("", "SRI", "", save_dev_id, 2, 0);
}

// Função para enviar JSON
function EnviaJSON(msg_type, cmd, mac, id, pin_stt, wifi_ch){

    const data_json = {
        MSG : msg_type,
        CMD : cmd,
        MAC : mac,
        ID : id,
        PST: pin_stt,
        WCH : wifi_ch
    };   
    webSocket.send(JSON.stringify(data_json));             
} // Fim de EnviaJSON()


// Actualiza dados na Card 
function ActualizaCard(Id, msg_obj){ 
   
    if (get_srvr_id.toString().length != 7 || get_srvr_id == "" || get_srvr_id == 0 || Id == 0 || Id == null || Id == "") {               
        return;
    }   
    
    try{
        let pai = document.getElementById(Id);
        if(!pai || pai == null) return;
        // Actualiza imagem de lâmpadas, electrobomba
        pai.querySelector('.div_img').innerHTML = alterarImagem(msg_obj.PST, Id);
        pai.querySelector('.div_nome').innerHTML = msg_obj.MAC; // Nome;
        pai.querySelector('.div_historico').innerHTML = msg_obj.MSG; // DT;
        
        // Obter Nome do dispositivo na indexedDB          
        for(let cont = 0; cont < DEVICES_LIST_AUX.length; cont++){
            if(DEVICES_LIST_AUX[cont].Id === Id && DEVICES_LIST_AUX[cont].Nome !== ''){
                pai.querySelector('.div_nome').innerHTML = DEVICES_LIST_AUX[cont].Nome;
            }
        }

        
        pai.style.pointerEvents = "auto";
        pai.disabled = false;
        pai.style.opacity = 1;  
        // Dados do device para json 
        for(let i = 0; i < DEVICES_LIST_FROM_ESP.length; i++){
            if(DEVICES_LIST_FROM_ESP[i].Id == Id){
                DEVICES_LIST_FROM_ESP[i].Nome = msg_obj.MAC;
                DEVICES_LIST_FROM_ESP[i].Historico = msg_obj.MSG;
                
                var obj = new Object();
                obj.Id = Id;
                obj.Nome = msg_obj.MAC;             
                EnviaJSON(JSON.stringify(obj), "DST", "", 2, 0, 0); 
            }
        }
    }
    catch(e){
        console.log(`Erro 5: ${e.message}`);
        console.log(Id, msg_obj);
    }
    
} // Fim de ActualizaCard()


// Preenche Dialogo de Configuração de Timers 
function fill_dlg_timers(objson){   
    let chck_nchck = "";
    let indx_saida = "";

    objson.Enable ? chck_nchck = true : chck_nchck = false;
    indx_saida = objson.Output - 1;

    if (indx_saida = -1) indx_saida = 0;

    document.getElementById("chk_activar").checked = chck_nchck;
    document.getElementById('select_saida').selectedIndex = indx_saida;

    // (*J*) Se existir mais de 2 duas acções a lógica deve mudar
    document.getElementById('select_accao').selectedIndex = objson.Action;
    document.getElementById('time_hora').value = objson.Time;
    var checBtn = document.getElementsByName("sel_week_day");
    for (let i = 0; i < checBtn.length; i++) {
        checBtn[i].checked = parseInt(objson.WeekDays[i]);
    }

} // Fim de fill_dlg_timers()


// Evento onload
window.addEventListener("load", (event) => {

    // Cria, se nao existe, o Banco de Dados IndexedDB
    criarBancoDados();

    document.getElementById('lgd').innerHTML = '<b>&nbsp;' + 'Configurações de Rede' + '&nbsp;</b>';
    obtemDadosWiFi();
    document.getElementById("select_mod").selectedIndex = "0";   

    // Atribui "Eventlistaner" aos radios buttons "select_timer".    
    const radios = document.querySelectorAll('input[name="select_timer"]');
    for (const radio of radios) {
        radio.onclick = (e) => {
            // Restituir estado do botão "Guardar"
            document.getElementById("div_share_timer").disabled = false;
            document.getElementById("div_share_timer").querySelector('svg').style.stroke = "#39be5a";
            
            document.getElementById("div_guardar_tmrs").disabled = false;
            document.getElementById("div_guardar_tmrs").querySelector('svg').style.stroke = "#39be5a";

            obtemConfigTimer(save_dev_id, e.target.value);
        }
    }

    document.getElementById("div_guardar").disabled = true;
    document.getElementById("div_guardar").querySelector('svg').style.stroke = "gray";
   
    // Obter Hora actual
    obter_data_hora();
    // Adicionar elemntos aos Menu do controlo remoto
    var ul = document.getElementById("ul_wrapper");
    Object.entries(aparelhos).forEach(([key, value]) => {
        var li = document.createElement("li");
        var children = ul.children.llocalStorageength + 1
        li.setAttribute("id", "element"+children);
        li.appendChild(document.createTextNode("Element "+children));
        li.innerHTML = `${value} <ul><li class="liexec">Executar</li><li class="liprog" onclick="gravarCodigoIR('${key}', '${value}');">Programar</li></ul>`;  
        li.classList.add('submenu');          
        ul.appendChild(li);
       
        document.querySelectorAll('.liprog').forEach(function(lielm) {
            lielm.style.color = 'red'; 
            lielm.style.fontWeight= 'bold'; 
        });

        document.querySelectorAll('.liexec').forEach(function(lielm) {
            lielm.style.color = 'green';
            lielm.style.fontWeight= 'bold';  
            lielm.style.display = 'none';  
        });
    });    
    menu_inferior_modo = "UNLOCKED";
    document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_locked;
    document.getElementById('span_text_centro_menu_inferior').innerText = 'Desbloquear';

});



// Exibir Data/Hora no Dlg de configurações de Timer
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
	if (minutes < 10) minutes = "0" + minutes;
	if (seconds < 10) seconds = "0" + seconds;

	formato += " " + hours + ":" + minutes + ":" + seconds;	
	formato += "</font>";
    document.getElementById("hora_actual").innerHTML = formato;
    tt = display_c(); 

} // Fim de obter_data_hora()


// Exibir Data/Hora no Dlg de configurações de Timer
function display_c(){   
    var refresh = 1000; 
    mytime = setTimeout('obter_data_hora()', refresh);    
}

// Guardar configurações de Timer
// tmr_mod -- determina se as configs vao para um ou todos Devs
function auxiliarGuardarTimers(tmr_mod){
    
    let txt_nome = "";
    let chk_activar = "";
    //let timer = "";
    let saida = "";
    let accao = "";
    let hora = "";
    let dias_semana = "";
   
    document.getElementById("chk_activar").checked ? chk_activar = 1 : chk_activar = 0;
    var timer = document.querySelector('input[name=select_timer]:checked');
    let sel_saida = document.getElementById("select_saida");
    let sel_accao = document.getElementById("select_accao");
    saida = sel_saida.options[sel_saida.selectedIndex].text;
    sel_accao.options[sel_accao.selectedIndex].text == "Ligar" ? accao = "1" : accao = "0";
    hora = document.getElementById("time_hora").value;
    dias_semana = obtemDiaSemana();

    let TimerX = timer.value - 1;

    const txt_json = JSON.stringify({ ID:save_dev_id, CMD:tmr_mod, Timer:TimerX, Enable:chk_activar, Time:hora, WeekDays:dias_semana, Output:saida, Action:accao });   
    webSocket.send(txt_json);   
    showMsgBox('Processo Concluido!');
    // Limpa items da DIV de configuração
    resetElementos();    
}// Fim de auxiliarGuardarTimers()


// Guardar configurações num Dispositivo
function guardarConfigsTimers() {
    auxiliarGuardarTimers("STM");
} // Fim de guardarConfigsTimers()

// Para configurações em todos Dispositivos
function partilharTimers(){
    auxiliarGuardarTimers("SHT");
}// Fim de partilharTimers()


// Fechar Mensagem 
function fecharShowMsgBox(){
    document.getElementById('div-msg-feedback-back').style.display = 'none';   
    document.getElementById('div_cfg_tmrs_wrapper').style.zIndex = 999;   
    document.getElementById('div_wrapper_set_name').style.zIndex = 999;     
}


// Reset elementos da DIV de configuração
function resetElementos() {
    // Mudar estado dos elementos
    document.getElementById("div_guardar_tmrs").disabled = true;
    document.getElementById("div_guardar_tmrs").querySelector('svg').style.stroke = "gray";

    document.getElementById("div_share_timer").disabled = true;
    document.getElementById("div_share_timer").querySelector('svg').style.stroke = "gray";

    document.getElementById("div_copy_timer").disabled = true;
    document.getElementById("div_copy_timer").querySelector('svg').style.stroke = "gray";

    document.getElementById("div_past_timer").disabled = true;
    document.getElementById("div_past_timer").querySelector('svg').style.stroke = "gray";


    var radList = document.getElementsByName('select_timer');
    for (var i = 0; i < radList.length; i++) { radList[i].checked = false; }

    var chckList = document.getElementsByName('sel_week_day');
    for (var i = 0; i < chckList.length; i++) { chckList[i].checked = false; }

    document.getElementById("chk_activar").checked = false;
    document.getElementById('select_saida').selectedIndex = -1;
    document.getElementById('select_accao').selectedIndex = -1;
    document.getElementById("time_hora").value = "00:00:00";
    
} // Fim de resetElementos()


// Obter configuraçõs do TimerX
function obtemConfigTimer(dev_id, TimerX) {  
    // Solicita consulta de Timers
    var TimerX = TimerX - 1; // Para conformar com os indices no arquivo SPIFFS, começam em zero
    const txt_json = JSON.stringify({MSG: "", CMD: "GTM", MAC:"", ID: dev_id, PST: 2, WCH: 0, Timer: TimerX});   
    webSocket.send(txt_json);    
    //console.log(txt_json);     
}// Fim de obtemConfigTimer()



// Limpar os Timers 
function reset_device() {                   
    const txt_json = JSON.stringify({MSG: "", CMD: "RSD", MAC:"", ID: save_dev_id, PST:2, WCH: 0, Timer:0});   
    webSocket.send(txt_json);  
} // Fim de reset_device()



// Abrir Dialogo p/ renomear device 
function openDlgRenameDevs(Id){
    save_dev_id = Id;
    document.getElementById('div_wrapper_set_name').style.display = 'block';   
    const pai = document.getElementById(Id);
    const dlg = document.getElementById('div_set_name');   
    dlg.style.position = 'absolute';  
    dlg.style.display = 'block';        
    dlg.style.top = (window.scrollY + pai.getBoundingClientRect().top + 0) + 'px';
    // Carrega Nome do Dispositivo
    save_dev_name = pai.querySelector('.div_nome').innerText;  
    document.getElementById('txt_nome').value = save_dev_name;  
    
}// Fim de openDlgRenameDevs()


// Fechar Dialogo p/ renomear device 
function fecharAlterarNomeDevice(){
    document.getElementById("div_guardar").disabled = true;
    document.getElementById("div_guardar").querySelector('svg').style.stroke = "gray";
   
    document.getElementById('div_wrapper_set_name').style.display = 'none'; 
    document.getElementById("txt_nome").value = "";
} // Fim de fecharAlterarNomeDevice()


// Envia comando para tasmota
async function openTasmotaCmndDlg(){
    
    EnviaJSON("", 'GWS', "", save_dev_id, 0, 0);

    fecharAlterarNomeDevice();

    document.getElementById('send_tasmota_cmnd').style.display = 'block';   
    document.getElementById('send_tasmota_cmnd').style.zIndex = 1;
    
}// Fim de openTasmotaCmndDlg()

// Fechar Dialogo p/ configuracoes do tasmota 
function fecharTasmotaCmndDlg(){
    document.getElementById('send_tasmota_cmnd').style.display = 'none';   
    document.getElementById('send_tasmota_cmnd').style.zIndex = -10000;
    document.getElementById("txt_ssid_wifi").value = "";
    document.getElementById("txt_senha_wifi").value = "";

}// Fim de fecharTasmotaCmndDlg()


// Ligar WiFi de Tasmota
function tasmotaWifiON(){
    EnviaJSON("", 'TWN', "", save_dev_id, 0, 0);
    showMsgBox('Processo Concluido!');
}// Fim de tasmotaWifiON()


// Desligar WiFi de Tasmota
function tasmotaWifiOFF(){
    EnviaJSON("", 'TWF', "", save_dev_id, 0, 0);
    showMsgBox('Processo Concluido!');
}// Fim de tasmotaWifiOFF()


// Conecta tasmota a rede wifi
function conecta2RedeWiFi(){
    const ssid = document.getElementById("txt_ssid_wifi").value;
    const senha = document.getElementById("txt_senha_wifi").value;

    if(ssid == ""){
        showMsgBox('Indique o SSID!');
    }
    else{  
        // MSG = ssid
        // MAC = senha           
        EnviaJSON(ssid, 'TCW', senha, save_dev_id, 0, 0);
        showMsgBox('Processo Concluido!');
    }

}// Fim de conecta2RedeWiFi

// Fechar Dlg Config Timers 
function fecharDlgConfigTimers(){
    document.getElementById('div_cfg_tmrs_wrapper').style.display = 'none';   
    document.getElementById('div_cfg_tmrs_wrapper').style.zIndex = -999;
} // Fimde fecharDlgConfigTimers()


  
// Alterar nome do Device
async function alterarNomeDevice() {
    let dev_name = document.getElementById("txt_nome").value;
    if ( dev_name == "" || dev_name == null || dev_name.trim() == save_dev_name.trim()) {
        showMsgBox('Efectue as alterações a guardar!');
        document.getElementById('div-msg-feedback-back').style.display = 'block';         
        document.getElementById('div_wrapper_set_name').style.zIndex = -999; 
        return;
    }
    else{

        if (dev_name.length > 0 && dev_name != null && (dev_name.trim() != save_dev_name.trim())){
            const txt_json = JSON.stringify({MSG: "", CMD: "RNM", MAC:dev_name, ID: save_dev_id, PST:0, WCH: 0});   
            webSocket.send(txt_json);               
        } 

        document.getElementById("div_guardar").disabled = true;
        document.getElementById("div_guardar").querySelector('svg').style.stroke = "gray";
        showMsgBox('Processo Concluido.');
        fecharAlterarNomeDevice();
    }    
} // Fimde alterarNomeDevice()



// Abrir Dialogo de configuração OTA nos Clientes
async function openDlgSetOTAClients(){    
    fecharDlgConfigTimers();
    const txt_json = JSON.stringify({MSG: "", CMD: "ENABLE_OTA", MAC: "", ID: save_dev_id, PST:2, WCH: 0});   
    webSocket.send(txt_json);    
} // Fim de openDlgSetOTAClients()

// Obter dias de semana 
function obtemDiaSemana() {
    let weekDays = "";
    var checBtn = document.getElementsByName("sel_week_day");
    for (let i = 0; i < checBtn.length; i++) {
        checBtn[i].checked ? weekDays += "1" : weekDays += "0";
    }
    return weekDays;

} // Fim de obtemDiaSemana()


// Abrir Dialogo p/ configurar Timers 
function openDlgConfigTimers(Id){    
    save_dev_id = Id;
    resetElementos();    
    document.getElementById('div_cfg_tmrs_wrapper').style.display = 'block'; 
    document.getElementById('div_cfg_tmrs_wrapper').style.zIndex = 999;  
    const pai = document.getElementById(Id);
    document.getElementById('div_separador_cfg_top').innerHTML = '<p><b>' + pai.querySelector('.div_nome').innerHTML + '</b></p></div>';    
    document.getElementById('div_estado').innerHTML = pai.querySelector('.div_img').innerHTML;
    
    // Desactivar/Activar Radio Button de Timers, em funcao do dispositivo (ZigBee/Esp_now)
    let num = Id;
    let numStr = num.toString();
    const marca = `${numStr[4]}${numStr[5]}`;
    if(marca == MARCAZB ){
       for(let cont = (QTD_TMRS_ZB + 1); cont <= QTD_TMRS_ESPNOW; cont++){
            document.getElementById(`lbl${cont}`).disabled = true;
            document.getElementById(`lbl${cont}`).classList.add('disabled');
       }
    } 
    else if(save_dev_id == get_srvr_id ){
        for(let cont = 1; cont <= QTD_TMRS_ESPNOW; cont++){
            document.getElementById(`lbl${cont}`).disabled = true;
            document.getElementById(`lbl${cont}`).classList.add('disabled');
        }
     } 
    else{
        for(let cont = 1; cont <= QTD_TMRS_ESPNOW; cont++){
            document.getElementById(`lbl${cont}`).disabled = false;
            document.getElementById(`lbl${cont}`).classList.remove('disabled');
        }
    }  

} // Fim de openDlgConfigTimers

// Exibir Mensagem 
function showMsgBox(mensagem){   
    const pai = document.getElementById('div-msg-feedback-back');
    pai.style.display = 'block';
    pai.style.zIndex = 1; 
    pai.querySelector('.div_msg_feedback').innerHTML = mensagem;   
    document.getElementById('div_cfg_tmrs_wrapper').style.zIndex = -999;      
    setTimeout(function(){ fecharShowMsgBox(); }, 5000);  
}

// Função para alterar imagem dos botões
function alterarImagem(var_estado, Id) {
   
    let img = "";

    if( (Id % 10) == 1){ // Lampadas
        if(var_estado == 1){
            img = '<svg fill="#e1e437" viewBox="-80 0 512 512" ><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>';
        }    
        else if(var_estado == 0){
            img = '<svg fill="lightgray" viewBox="-80 0 512 512" ><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>';
        }
        else {
            img = '<svg fill="lightgray" viewBox="-80 0 512 512" ><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>';
        }
    }
    else if( (Id % 10) == 2){ // Controlo Remote
        img = ctrl_r_svg
    }
    else if( (Id % 10) == 3){ // Electrobomba
        if(var_estado == 1){
            img = '<svg fill="green" stroke="none" ;vertical-align: middle;overflow: hidden;" viewBox="0 0 1024 1024"><path d="M544.7 703.8v-320c-35.3 0-64 26-64 58.2v203.7c0 32.1 28.6 58.1 64 58.1zM576.7 383.8h288v44.5h-288zM576.7 556.3h288v64h-288zM576.7 460.3h288v64h-288zM960.7 645.6V442c0-32.1-28.7-58.2-64-58.2v320c35.4 0 64-26 64-58.2zM640.7 351.8h160c17.7 0 32-14.3 32-32v-64h-224v64c0 17.7 14.3 32 32 32zM288.7 223.8h64v-64h-64v-64h-64v64h-64v64h64z"  /><path d="M832.7 799.8v-64h32v-83.5h-288v83.5h32v64h-320v-160h64v-64h32v32h64v-128h-64v32h-32v-64h-64v-128h64v-64h-192v64h64v128h-32v192h32v96h-128c-35.3 0-64 28.7-64 64v128h960.1v-128H832.7z m-64 0h-96v-64h96v64z"  /><path d="M160.7 639.8v-192c-35.3 0-64 28.7-64 64h-64v64h64c0 35.4 28.6 64 64 64z"  /></svg>';
        }    
        else if(var_estado == 0){
            img = '<svg fill="lightgray" stroke="none" ;vertical-align: middle;overflow: hidden;" viewBox="0 0 1024 1024"><path d="M544.7 703.8v-320c-35.3 0-64 26-64 58.2v203.7c0 32.1 28.6 58.1 64 58.1zM576.7 383.8h288v44.5h-288zM576.7 556.3h288v64h-288zM576.7 460.3h288v64h-288zM960.7 645.6V442c0-32.1-28.7-58.2-64-58.2v320c35.4 0 64-26 64-58.2zM640.7 351.8h160c17.7 0 32-14.3 32-32v-64h-224v64c0 17.7 14.3 32 32 32zM288.7 223.8h64v-64h-64v-64h-64v64h-64v64h64z"  /><path d="M832.7 799.8v-64h32v-83.5h-288v83.5h32v64h-320v-160h64v-64h32v32h64v-128h-64v32h-32v-64h-64v-128h64v-64h-192v64h64v128h-32v192h32v96h-128c-35.3 0-64 28.7-64 64v128h960.1v-128H832.7z m-64 0h-96v-64h96v64z"  /><path d="M160.7 639.8v-192c-35.3 0-64 28.7-64 64h-64v64h64c0 35.4 28.6 64 64 64z"  /></svg>';
        }
        else {
            img = '<svg fill="lightgray" stroke="none" ;vertical-align: middle;overflow: hidden;" viewBox="0 0 1024 1024"><path d="M544.7 703.8v-320c-35.3 0-64 26-64 58.2v203.7c0 32.1 28.6 58.1 64 58.1zM576.7 383.8h288v44.5h-288zM576.7 556.3h288v64h-288zM576.7 460.3h288v64h-288zM960.7 645.6V442c0-32.1-28.7-58.2-64-58.2v320c35.4 0 64-26 64-58.2zM640.7 351.8h160c17.7 0 32-14.3 32-32v-64h-224v64c0 17.7 14.3 32 32 32zM288.7 223.8h64v-64h-64v-64h-64v64h-64v64h64z"  /><path d="M832.7 799.8v-64h32v-83.5h-288v83.5h32v64h-320v-160h64v-64h32v32h64v-128h-64v32h-32v-64h-64v-128h64v-64h-192v64h64v128h-32v192h32v96h-128c-35.3 0-64 28.7-64 64v128h960.1v-128H832.7z m-64 0h-96v-64h96v64z"  /><path d="M160.7 639.8v-192c-35.3 0-64 28.7-64 64h-64v64h64c0 35.4 28.6 64 64 64z"  /></svg>';
        }
    }
    else if( (Id % 10) == 4){ // Bridge Zigbee
       img = '<svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="orange" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="2"></circle><path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path></svg>';
    }
            
    return img;
}


// Controlar acção de teclas 
document.onkeydown = function (e) {
    //console.log(e.code);
    let sel = document.getElementById("select_mod");
    if(e.code == 'Escape' && document.activeElement === sel){
     sel.selectedIndex = "0";
    }
 };
 
 

 // Envia comando aos clientes para activar modo OTA, carregar SKETCH/SPIFFS //
  async function configurarWiFi() { 
     var modo = "";
     var sel = document.getElementById("select_mod");
     //modo = sel.options[sel.selectedIndex].text;
     if (sel.selectedIndex == 1) {
         modo = "OPRA"; // Modo de Operação AP       
     }
     else if (sel.selectedIndex == 2) {
         modo = "OPRS"; // Modo de Operação STA
     }         
 
     let txt_rede = document.getElementById("txt_rede").value;
     let txt_senha = document.getElementById("txt_senha").value;
     let txt_ip = document.getElementById("txt_ip").value;
     let txt_gw = document.getElementById("txt_gw").value;
     let txt_subnet = document.getElementById("txt_subnet").value;
     let txt_dns1 = document.getElementById("txt_dns1").value;
     let txt_dns2 = document.getElementById("txt_dns2").value;
 
     
     if (txt_ip == "" || txt_gw == "" || txt_subnet == "") {
         txt_gw = txt_ip = txt_gw = txt_subnet = '0.0.0.0';
     } 
     if (txt_dns1 == "") { txt_dns1 = '0.0.0.0'; } 
     if (txt_dns2 == "") {txt_dns2 = '0.0.0.0';} 
     
 
     if ( ((txt_rede == "" || txt_rede == null)) && modo == "OPRS") {
         document.getElementById("txt_rede").style.backgroundColor = 'red';
         return;
     }
     else{
        document.getElementById("txt_rede").style.backgroundColor = 'white';
     }
  
     let txt_json = JSON.stringify({ Comando: 'SET_WIFI_CFG', Modo: modo, SSID: txt_rede, PWD: txt_senha, IP: txt_ip, Gateway: txt_gw, Subnet: txt_subnet, Dns1: txt_dns1, Dns2: txt_dns2 });
     let url_payload = http_prot + "//" + location.host + "/cm?cmnd=" + txt_json;
     let result = await req_http("GET", url_payload);
 }
 
 
 // Enviar Requisição HTTP ao Servidor
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
 
 
  // Scanear Redes WiFi 
 async function scanWiFi() { 
     const elem = document.getElementById('div_redes_wifi');
     const display = window.getComputedStyle(elem).display; 
     if (display == 'none') {elem.style.display = 'block';} 
     const url = http_prot + "//" + location.host + "/scan_wifi";
     httpGet(url); 
 }
 
 
 function httpGet(theUrl) {
      const tmp_array = [];
     var xmlHttp = new XMLHttpRequest();
     xmlHttp.open("GET", theUrl, true); // false for synchronous request
     xmlHttp.send(null);
 
     const segundos = 59;
     const minutos = 0;
     var minute = minutos;
     var sec = segundos;
     document.getElementById('div_lista_redes_wifi').innerHTML = '';
 
     let ctrltmr = setInterval(function () {
         let msg = `<div id="div_msgbox">AGUARDE: ${sec}</div>`;
         document.getElementById('div_lista_redes_wifi').innerHTML += '.';
         document.getElementById('div_lista_redes_wifi').style.color = 'black';
         document.getElementById('div_lista_redes_wifi').style.fontSize = '25px';
         document.getElementById('div_lista_redes_wifi').style.fontWeight = 'bold';
         document.getElementById('div_lista_redes_wifi').style.textAlign = 'center';
 
         sec--;
 
         if (sec <= 0 || xmlHttp.readyState == XMLHttpRequest.DONE) {
             sec = 0;
             clearInterval(ctrltmr);
 
             if (xmlHttp.responseText == "") {
                 document.getElementById('div_lista_redes_wifi').innerHTML = 'Repita a operação!';
                 document.getElementById('div_lista_redes_wifi').style.color = 'red';
                 return;
             }
 
             // Checar JSON
             let json = xmlHttp.responseText;
             try {
                 json = JSON.parse(json);
             } catch (err) {
                 console.log(err);
                 document.getElementById('div_lista_redes_wifi').innerHTML = 'JSON Inválido!';
                 document.getElementById('div_lista_redes_wifi').style.color = 'red';
                 return;
             }
 
             if (!json) {
                 document.getElementById('div_lista_redes_wifi').innerHTML = 'JSON Nulo!';
                 document.getElementById('div_lista_redes_wifi').style.color = 'red';
                 return;
             }
 
 
             for (let i = 0; i < Object.keys(json).length; i++) {
 
                 if (json[i].SSID != "") {
 
                     if (!tmp_array.includes(json[i].SSID)) {
                         tmp_array.push(json[i].SSID);
                     }
                 }
             }
 
             // Carregar lista de redes WiFi
             document.getElementById('div_lista_redes_wifi').innerHTML = '';
 
             for (let i = 0; i < tmp_array.length; i++) {
                 let linha = '<li><a id=anchora"' + i + '" href="#" onclick="getWiFiName(this.id);">' + tmp_array[i] + '</a></li>';
                 document.getElementById('div_lista_redes_wifi').innerHTML += linha;
                 document.getElementById('div_lista_redes_wifi').style.color = 'blue';
                 document.getElementById('div_lista_redes_wifi').style.fontSize = '16px';
                 document.getElementById('div_lista_redes_wifi').style.fontWeight = 'normal';
                 document.getElementById('div_lista_redes_wifi').style.textAlign = 'left';
             }
         }
 
     }, 250);
 
 }
 
 
 // Envia request HTTP
 async function sendRequest() {
     const Http = new XMLHttpRequest();
     const url = http_prot + "//" + location.host + "/scan_wifi";
     Http.open("GET", url);
     Http.send();
 
     if (Http.readyState === XMLHttpRequest.DONE) {
         return Http;
     }
 
     let res;
     const p = new Promise((r) => res = r);
     Http.onreadystatechange = () => {
         if (Http.readyState === XMLHttpRequest.DONE) {
             res(Http);
         }
     }
     return p;
 }
 
 
 
 //Obter Nome de rede WiFi
 function getWiFiName(Id) {
    document.getElementById('txt_rede').value = document.getElementById(Id).textContent;
 }
 
 
  // Checar tag "input/txt"
 function changeTxtHandler(Id) {
    var modo = "";
    var sel = document.getElementById("select_mod");
    //modo = sel.options[sel.selectedIndex].text;
    if (sel.selectedIndex == 1) {
        modo = "OPRA"; // Modo de Operação AP       
    }
    else if (sel.selectedIndex == 2) {
        modo = "OPRS"; // Modo de Operação STA
    }    
     var data = document.getElementById(Id).value;
 
     if (data == "" && modo == "OPRS") {
         document.getElementById(Id).style.backgroundColor = 'red';
     }
     else if (data != "" && modo == "OPRS"){
         document.getElementById(Id).style.backgroundColor = 'white';
     }
     else if(modo == "OPRA"){
        document.getElementById(Id).style.backgroundColor = 'white';
     }
 }
 
 //Limpar Dados WiFi
 async function limparDispositivo() {
    let txt_json = JSON.stringify({ Comando: 'RESET_DEVICE', MAC: "" });
    let url_payload = http_prot + "//" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);
 }

 //Cancelar o modo OTA
 async function cancelarOTA() {    
    let txt_json = JSON.stringify({ Comando: 'CANCEL_OTA', MAC: "" });
    let url_payload = http_prot+ "//" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);
 }

 //Abrir pagina para actualizar firmeware
 function actualizarFirmeware() { 
    var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + 'update';
    window.history.pushState({path:newurl},'',newurl); 
    location.reload(); 
 }
 
 
 // Fechar Dialogo Config. Rede
 async function cancelarModoCFG() {
    document.getElementById('div_redes_wifi').style.display = 'none';
    document.getElementById('div_wifi_wrappper').style.display = 'none';
 }

 // Abrir Dialogo Config. Rede
 function openDlgWifiConfgs(){
    document.getElementById('div_wifi_wrappper').style.display = 'block';
 } 

 // Obter Dados WiFi
 async function obtemDadosWiFi() {
     let txt_json = JSON.stringify({ Comando: 'GET_WIFI_CFG', MAC: "" });     
     let url_payload = http_prot + "//" + location.host + "/cm?cmnd=" + txt_json;
     let result = await req_http("GET", url_payload);
     
     let json = "";
     
     // Checar JSON
     try {
         json = JSON.parse(result);         
     } catch (err) {
         console.log(err);
         console.log('JSON Inválido!');
         return;
     }
 
     if (!result) {
         console.log('JSON Nulo!');
         return;
     } 
 
     if(json.Modo == 'OPRA'){
         document.getElementById("select_mod").selectedIndex = "1";       
     }
     else if(json.Modo == 'OPRS'){
         document.getElementById("select_mod").selectedIndex = "2";        
     }
     
     document.getElementById("txt_rede").value = json.SSID;
     document.getElementById("txt_senha").value = json.PWD;
     document.getElementById("txt_ip").value = json.IP;
     document.getElementById("txt_gw").value = json.GW;
     document.getElementById("txt_subnet").value = json.MSK;
     document.getElementById("txt_dns1").value = json.DNS1;
     document.getElementById("txt_dns2").value = json.DNS2;
 
 }
 

 // Exibe Menu contextual ao clicar nos tres pontos
function mostrarMenuContextual(){ 
    const card_state = document.getElementById('span_text_centro_menu_inferior').innerText;
   
    save_dev_id = get_srvr_id;    
    document.getElementById('li_rnm').style.display = 'block';
    document.getElementById('li_tmr').style.display = 'block';
    document.getElementById('li_cfg').style.display = 'block';
      
    const menu = document.querySelector('#div_menu_contextual');  
    window.addEventListener('click', event => menu.style.display = 'none');
    // show the menu when right-clicked
    window.addEventListener('contextmenu', event => {
        event.preventDefault();
        menu.style.position = 'absolute';
        menu.style.top = (window.scrollY + 50) + 'px';        
        if(card_state == "Bloquear"){
            menu.style.display = 'none';
        }
        else if(card_state == "Desbloquear"){
            menu.style.display = 'block';
        }
    });
}

 
// Processa Click Direito sobre Card (Menu Contextual)
function clickDireito(devId){      
    save_dev_id = devId;
    const menu = document.querySelector('#div_menu_contextual');
    
    // Ocultar/Exibir item (Configuraões), do menu contextual
    if(get_srvr_id == save_dev_id){
        document.getElementById('li_rnm').style.display = 'block';
        document.getElementById('li_tmr').style.display = 'block';
        document.getElementById('li_cfg').style.display = 'block';
    }
    else if( (save_dev_id % 10) == 2 ){
        document.getElementById('li_rnm').style.display = 'block';
        document.getElementById('li_tmr').style.display = 'block';
        document.getElementById('li_cfg').style.display = 'none';
    }
    else{
        document.getElementById('li_rnm').style.display = 'block';
        document.getElementById('li_tmr').style.display = 'block';
        document.getElementById('li_cfg').style.display = 'none';
    }

    // hide the menu
    window.addEventListener('click', event => menu.style.display = 'none');
    // show the menu when right-clicked
    window.addEventListener('contextmenu', event => {
        event.preventDefault();

        const card_state = document.getElementById('span_text_centro_menu_inferior').innerText;

        if (card_state == "Desbloquear") {            
            const pai = document.getElementById(save_dev_id);            
            menu.style.position = 'absolute';
            menu.style.top = (window.scrollY + pai.getBoundingClientRect().top + 5) + 'px';

            if (event.target.nodeName === "HTML" || event.target.id === "card_wrapper" || event.target.id === "div_geral") {
                menu.style.display = 'none';
            }
            else{
                // Exibir menu contextual, apenas, em elementos activados
                if(event.target.id == get_srvr_id && document.getElementById(event.target.id).disabled == false){
                    menu.style.display = 'block';
                }
                else if(document.getElementById(event.target.id).disabled == false){
                    menu.style.display = 'block';
                }               
            }
        }
        else if (card_state == "Bloquear") {   
            document.querySelector('#div_menu_contextual').style.display = 'none';           
            var dragDiv = document.getElementById('div_dragg');
            dragDiv.style.display = 'block';
            dragDiv.style.background = 'rgba(223, 212, 212, 0.4)';
            dragDiv.style.border = 'dashed 2px red';
            dragDiv.addEventListener("touchstart", touchStart);
            dragDiv.addEventListener("touchmove", touchMove);
            dragDiv.addEventListener("touchend", touchEnd);              
            
            const card = document.getElementById(save_dev_id);
            dragBox = card.closest('.grid_container');

            document.querySelectorAll('.fundo').forEach(function (item) {
                item.classList.remove('fundo');
            });

            card.classList.add('fundo');
            dragDiv.style.left = dragBox.getBoundingClientRect().left + 20 + "px";
            dragDiv.style.top = dragBox.getBoundingClientRect().top + 20 + "px";
            originId = save_dev_id;
        }
    });
}


function on_click_cfg(devId){ 
    save_dev_id = devId;    
    const menu = document.querySelector('#div_menu_contextual');
    menu.style.display = 'block';    
}

// Impede click direito nos elementos indicados
document.addEventListener("contextmenu", function(e){
    if ( e.target.nodeName === "HTML" || e.target.id === "div_cfg_sys" || e.target.id === "card_wrapper" || e.target.id === "div_geral" ) {
        e.preventDefault();
    }    
}, false);

// Processa Click no Menu Contextual)
function menuContexto(Id){
    if( Id == "a_rnm"){
        openDlgRenameDevs(save_dev_id);
    }
    else  if( Id == "a_tmr"){
        openDlgConfigTimers(save_dev_id);
    }
    else  if( Id == "a_cfg" && get_srvr_id == save_dev_id){openDlgWifiConfgs();}
}


// impedir Acentuação nos nomes nos dispositivos ZigBee
function validateASCII(event) {

    let num = save_dev_id;
    let numStr = num.toString();
    const marca = `${numStr[4]}${numStr[5]}`;
    if(marca == MARCAZB ){
        var charCode = event.which || event.keyCode;
        if (charCode > 127) {
          event.preventDefault();
        }
    }    
}

// Controlar inserção de caracteres especiais 
function restrictSpecialChars(input) {
    
    //input.value = input.value.replace(/[^a-zA-Z0-9]/g, '');

    if(input.value == save_dev_name || input.value.length <= 0){
        document.getElementById("div_guardar").disabled = true;
        document.getElementById("div_guardar").querySelector('svg').style.stroke = "gray";
    }
    else{
        document.getElementById("div_guardar").disabled = false;
        document.getElementById("div_guardar").querySelector('svg').style.stroke = "#39be5a";
    }
}

// Solicitação p/ processamento de código IR
function send_ir_cmd(btnId){   
    EnviaJSON(get_aparelho, "CKS", btnId, save_dev_id, 2, 0); 
    if(modo_prog == false){
        EnviaJSON(get_aparelho, "SIR", btnId, save_dev_id, 2, 0);      
    }    
}

// Guarda o nome do aparelho seleccionado
function gravarCodigoIR(idAparelho, nomeAparelho){  
    document.getElementById('div_ir_panel').style.display = 'none';
    mostrarOcultarBlocos(idAparelho);
    get_aparelho = idAparelho;
    modo_prog = true;
    menu_inferior_modo = "MENU";
    document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_list;
    document.getElementById('span_text_centro_menu_inferior').innerText = 'Menu';
    document.getElementById('menu_ctrl_r').style.display = 'none';
    document.getElementById('div_head').innerHTML = `${nomeAparelho} (${get_hub_ir_nome})`;
    document.getElementById('div_head').style.color = 'red';
}

// Guarda o nome do aparelho seleccionado
function enviarCodigoIR(idAparelho, nomeAparelho){  
    document.getElementById('div_ir_panel').style.display = 'none';
    mostrarOcultarBlocos(idAparelho);
    get_aparelho = idAparelho;
    modo_prog = false;
    menu_inferior_modo = "MENU"
    document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_list;
    document.getElementById('span_text_centro_menu_inferior').innerText = 'Menu';
    document.getElementById('menu_ctrl_r').style.display = 'none';
    document.getElementById('div_head').innerHTML = `${nomeAparelho} (${get_hub_ir_nome})`;
    document.getElementById('div_head').style.color = 'green';
    EnviaJSON("", "SRI", "", save_dev_id, 2, 0);       
}


// fechar painel IR
function fecharPainelIR(){
    let elem = document.getElementById('div_ir_panel');
    elem.style.display = 'none';
    document.getElementById('ctrl_remoto').style.display = 'block';
    let teclDestino = document.getElementById('div_tecla_painel_ir');
    
    while (teclDestino.firstChild) {teclDestino.removeChild(teclDestino.lastChild);}

    const button = document.getElementById(save_key_id);
    clearInterval(intervalpgrId);
    button.classList.remove("flickedPgr");
    EnviaJSON("", "SRI", "", save_dev_id, 2, 0);
}


// Abrir Painel IR
function abrirPainelIR(key_id, modo){
    save_key_id = key_id;
    document.getElementById('ctrl_remoto').style.display = 'none';

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
    let teclDestino = document.getElementById('div_tecla_painel_ir');
    let teclaOrigem = document.getElementById(key_id).cloneNode(true);
    while (teclDestino.firstChild) {
        teclDestino.removeChild(teclDestino.lastChild);
    }
    teclDestino.appendChild(teclaOrigem);
}


// Programar Tecla
function programarTecla(){
    EnviaJSON(get_aparelho, "RIR", save_key_id, save_dev_id, 2, 0);
    const button = document.getElementById(save_key_id);
    // Call flickButton every 500 milliseconds (0.5 seconds)
    intervalpgrId = setInterval(function() {
        flickButtonPgr(button);
        if(modo_prog == false){
            clearInterval(intervalpgrId);
            button.classList.remove("flickedPgr");
        }  
    }, 500);
}


// função auxiliar para flick de botões 
function flickButtonPgr(button) {
    button.classList.toggle("flickedPgr");   
}


// Desprogramar Tecla
function desprogramarTecla(){
    EnviaJSON(get_aparelho, "UPK", save_key_id, save_dev_id, 2, 0);
}

// Exibir/ocultar blocos do controlo remoto
function mostrarOcultarBlocos(idAparelho){
    document.getElementById('ctrl_remoto').style.display = 'block';
    Object.entries(aparelhos).forEach(([key, value]) => {
        if(idAparelho == 'RAD' || idAparelho == 'DST' || idAparelho == 'ZAP'){
            idAparelho = 'TLV';
        }        
        if(idAparelho == key){
            let els = document.getElementsByClassName(key);        
            Array.prototype.forEach.call(els, function(el) {
                el.style.display = 'inline-flex';
            });
        }        
        else{
            let els = document.getElementsByClassName(key);        
            Array.prototype.forEach.call(els, function(el) {
                el.style.display = 'none';
            });
        }
    });   
}


// MENU DE NAVEGAÇÂO INFERIOR 
const list = document.querySelectorAll('.list');
function activeLink(){
    list.forEach((item) => {
        item.classList.remove('active');
        this.classList.add('active');
    });
}


function click_menu_bottom(element){   

    document.querySelectorAll('.fundo').forEach(function (item) {
        item.classList.remove('fundo');
    });
    document.getElementById('div_dragg').style.display = 'none'; 
      
    if(element.classList.contains('active')){
       
        if(menu_inferior_modo == "FECHAR"){
            document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_locked;
            document.getElementById('span_text_centro_menu_inferior').innerText = 'Desbloquear';
            menu_inferior_modo = "LOCKED";
            fecharControloR();
        }
        else if(menu_inferior_modo == "MENU"){
            menu_inferior_modo = "FECHAR";
            document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_close;
            document.getElementById('span_text_centro_menu_inferior').innerText = 'Fechar';
            document.getElementById('div_head').innerHTML = '';
            document.getElementById('ctrl_remoto').style.display = 'none';
            document.getElementById('menu_ctrl_r').style.display = 'block';
        }
        else if(menu_inferior_modo == "LOCKED"){
            document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_locked;
            document.getElementById('span_text_centro_menu_inferior').innerText = 'Desbloquear';
            menu_inferior_modo = "UNLOCKED";
            //document.getElementById('card_wrapper').style.display = 'flexbox';            
        }
        else if(menu_inferior_modo == "UNLOCKED"){
            document.getElementById('div_btn_centro_menu_inferior').innerHTML = svg_unlocked;
            document.getElementById('span_text_centro_menu_inferior').innerText = 'Bloquear';
            menu_inferior_modo = "LOCKED";
        }
    }
}
// Fim de MENU DE NAVEGAÇÂO INFERIOR




async function obterTunelNGROK(){
    /*let payload = `http://${ip_servidor_python}:${porta_servidor_python}/GETTUNELNGROK`;
    const resp = await req_http('GET', payload); 

    if(resp =='NOTUNELNGROK'){
        showMsgBox('Falha, repita a operação!');
    }
    else if(resp == 'TUNELNGROKOK'){
        showMsgBox('Operação concluida com sucesso.');
    }*/
}


const colors = ['#3aa7ff', 'rgb(20, 20, 20)'];
let currentColorIndex = 0;
var contador = 0;
// Função para alternar a cor de fundo
function toggleBackgroundColor(button) {
    currentColorIndex = (currentColorIndex + 1) % colors.length;
    button.style.backgroundColor = colors[currentColorIndex];      
}


async function emparelharZigBee(){   

    const pai = document.getElementById('div-msg-feedback-back');
    pai.style.display = 'block';
    pai.style.zIndex = 1; 
    pai.querySelector('.div_msg_feedback').innerHTML = "Aguarde, por favor!";   
    document.getElementById('div_cfg_tmrs_wrapper').style.zIndex = -999;      
           
    const button = document.getElementById('div-msg-feedback-wrapper');
    intervalpgrId = setInterval(function() {
        toggleBackgroundColor(button);  
        if( (contador++) >= 120){
            clearInterval(intervalpgrId);
            button.style.backgroundColor = 'rgb(20, 20, 20)'; 
            fecharShowMsgBox();             
        }   
    }, 500);

    let txt_json = JSON.stringify({ Comando: 'PERMITJOIN', MAC: "" });     
    let url_payload = http_prot + "//" + location.host + "/cm?cmnd=" + txt_json;
    let resp = await req_http("GET", url_payload);

    if(resp =='PERMITJOIN'){
        //showMsgBox('Falha, repita a operação!');
    }
    else if(resp == 'PERMITJOIN'){
        //showMsgBox('Operação concluida com sucesso.');
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


document.addEventListener("DOMContentLoaded", function () {      
    GetDeviceName();   
});

async function GetDeviceName(){    
    let txt_json = JSON.stringify({ Comando: "GET_DEV_NAME", Modo:""});
    let url_payload = "http://" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);   
    get_dev_name = result; 
}


//////////////////////////////////////////////////////
// Envia comando ao Dispositivo para alterar o nome //
//////////////////////////////////////////////////////
async function renomearDispositivo(){      
    
    const nome = document.getElementById('txt_dados_ota').value;

    if ( nome == "" || nome == null ) {
        return;
    }

    let txt_json = JSON.stringify({ Comando: "RENAME_DEV", Nome: nome});
    let url_payload = "http://" + location.host + "/cm?cmnd=" + txt_json;
    let result = await req_http("GET", url_payload);     
    
    if(result == "OK"){       
        document.getElementById('txt_dados_ota').value = '';
    }
    
}

////////////////////////////////////
// Criar Banco de Dados indexedDB //
////////////////////////////////////
function criarBancoDados(){

    window.indexedDB = window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
    
    if(!window.indexedDB){
        console.log("O Navegador não suporta o recurso HTML5 IndexedDB");
        return;
    }

    // Open a Database: Before any operations, you must open a database. If the database doesn’t exist, it’s created automatically.
    var openRequest = indexedDB.open(db_name, db_version);

    // Define Object Stores: Once a database is opened or created, you can define object stores.
    openRequest.onupgradeneeded = function(e) {
        db = e.target.result;
        let store = db.createObjectStore(tbl_name);            
    };

    // Create: Add data to an object store.
    openRequest.onsuccess = function (event){
        db = openRequest.result;            
        var transaction = db.transaction([tbl_name], "readwrite").objectStore(tbl_name).add([], devices_row_key);
    };    
}


/////////////////////////////////////
// Actualizar Registo no indexedDB //
/////////////////////////////////////
function actualizarRegisto(reg_number, obj_data){ 
    
    criarBancoDados();     

    // Actualiza s dados da indexedDB, com os recem recebidos do Servidor ESP
    for(var i = 0; i < obj_data.length; i++){

        for(var j = 0; j < DEVICES_LIST_AUX.length; j++){
            if(DEVICES_LIST_AUX[j].Id == obj_data[i].Id &&  obj_data[i].Nome != '' && obj_data[i].Nome.length > 0){
                DEVICES_LIST_AUX[j].Nome = obj_data[i].Nome;
                DEVICES_LIST_AUX[j].Historico = obj_data[i].Historico;
            }
        }

        // Verificar o nome do dispositivo na indexedDB, se existe atribui-lo... 
        const devobj = DEVICES_LIST_AUX.find((objeto) => objeto["Id"] === obj_data[i].Id);
        if (!devobj && obj_data[i].Nome != '' && obj_data[i].Nome != '[Nome]' && obj_data[i].Nome.length > 0) {   
            DEVICES_LIST_AUX.push(obj_data[i]);     
        }   
    }    

    var request = window.indexedDB.open(db_name, reg_number);
    request.onsuccess = function (event){
        return new Promise((resolve, reject) => {
            resolve(db = request.result);                
            let transaction = db.transaction(tbl_name, 'readwrite');
            let store = transaction.objectStore(tbl_name);   
            obj_data = DEVICES_LIST_AUX;     
            store.put(obj_data, reg_number);              
        }); 
    };    	
}


////////////////////////////////////////////
// Ler um Registo especifico na indexedDB //
////////////////////////////////////////////
async function lerRegisto(reg_number){	
    var transaction;
    return new Promise((resolve, reject) => {    
        try{
            transaction = db.transaction(tbl_name, "readonly");
        } 
        catch(e){            
            criarBancoDados();
            return;
        }           
        //var transaction = db.transaction(tbl_name, "readonly");        
        var store = transaction.objectStore(tbl_name);
        var request = store.get(reg_number);
        request.onsuccess = function (event) {            
            resolve(event.target.result);            
        }       
    });
}


//////////////////////////////////
// Elimina elementos duplicados //
//////////////////////////////////
function removeDuplicateEelementes(classe) {
    // Coleta todos os elementos com o ID 'duplicate-id'
    const elements = document.querySelectorAll(`.${classe}`);
    const seen = new Set();
    const elementsToRemove = [];
    
    // Identificar elementos duplicados
    elements.forEach(element => {
        if (seen.has(element.id)) {
            elementsToRemove.push(element);            
        } else {
            seen.add(element.id);            
        }
    });

    // Remover elementos duplicados
    elementsToRemove.forEach(element => {
        element.remove();
    });
}

