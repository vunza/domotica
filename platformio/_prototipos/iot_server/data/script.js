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


///////////////////
// Evento onload //
///////////////////
window.addEventListener("load", (event) => {
    for(var cont = 0; cont < 8; cont++){
        CriarDevice(cont);
    }   
    
    /*socket = new WebSocket("ws:/" + "/" + location.host + ":6969");
    socket.onopen = function(e) {  console.log("[socket] socket.onopen "); };
    socket.onerror = function(e) {  console.log("[socket] socket.onerror "); };
    socket.onmessage = function(e) { console.log("[socket] " + e.data);  };*/

});



/*document.addEventListener('DOMContentLoaded', (event) => {
    // Crear una conexión WebSocket
    const socket = new WebSocket('ws://'+location.host+':6969');

    // Manejar el evento de apertura de conexión
    socket.addEventListener('open', (event) => {
        console.log('Conexión WebSocket abierta');
        // Enviar un mensaje al servidor
        socket.send('Hola, servidor!');
    });

    // Manejar mensajes recibidos del servidor
    socket.addEventListener('message', (event) => {
        console.log('Mensaje recibido del servidor:', event.data);
    });

    // Manejar errores
    socket.addEventListener('error', (event) => {
        console.error('Error en WebSocket:', event);
    });

    // Manejar el cierre de la conexión
    socket.addEventListener('close', (event) => {
        console.log('Conexión WebSocket cerrada');
    });

    // Función para enviar mensajes desde un botón
    document.getElementById('sendMessage').addEventListener('click', () => {
        const message = document.getElementById('messageInput').value;
        socket.send(message);
    });
});*/




// Cria Devices
async function CriarDevice(Id) {

    var nome = '[Nome]';
    var historico = '00:00:00, 00/00/0000';
    const img_hub_zb = '<svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="orange" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="2"></circle><path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path></svg>';
    const img_lampada = '<svg fill="lightgray" viewBox="-80 0 512 512" ><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>';
    const img_bomba = '<svg fill="lightgray" stroke="none" ;vertical-align: middle;overflow: hidden;" viewBox="0 0 1024 1024"><path d="M544.7 703.8v-320c-35.3 0-64 26-64 58.2v203.7c0 32.1 28.6 58.1 64 58.1zM576.7 383.8h288v44.5h-288zM576.7 556.3h288v64h-288zM576.7 460.3h288v64h-288zM960.7 645.6V442c0-32.1-28.7-58.2-64-58.2v320c35.4 0 64-26 64-58.2zM640.7 351.8h160c17.7 0 32-14.3 32-32v-64h-224v64c0 17.7 14.3 32 32 32zM288.7 223.8h64v-64h-64v-64h-64v64h-64v64h64z"  /><path d="M832.7 799.8v-64h32v-83.5h-288v83.5h32v64h-320v-160h64v-64h32v32h64v-128h-64v32h-32v-64h-64v-128h64v-64h-192v64h64v128h-32v192h32v96h-128c-35.3 0-64 28.7-64 64v128h960.1v-128H832.7z m-64 0h-96v-64h96v64z"  /><path d="M160.7 639.8v-192c-35.3 0-64 28.7-64 64h-64v64h64c0 35.4 28.6 64 64 64z"  /></svg>';
    const svg_clock = '<svg class = "svg_clk" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><polyline points="12 6 12 12 16 14"></polyline></svg>';


    // Tira o último digito
    const tipo = 9;
    //const tipo = Id % 10;
    let container = document.getElementById('card_wrapper');

    let cards_container = document.getElementById('cards_container');
  
    // Checar se o elemento ja existe
    const dev = document.getElementById(Id + 100);
    if(dev){ // Se Dev existe nao volta a criar
        return;
    }
 
    cards_container.innerHTML += `
        <div id=${Id + 100} class="card" onclick="">
            <div class="div_img" id=${Id} >${img_lampada}</div>  
            <div class="div_nome" id="div_nome">${nome}</div>    
            <div class="div_nome" id="div_nome">${nome}</div>                        
        </div>`;
        
        
    /*if(tipo == 1){ // LAMPADA
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
    }*/

}// Fim de CriarDevice()


//////////////////////
// Menu de Inferior //
//////////////////////
document.querySelectorAll('.nav-item').forEach(item => {
    item.addEventListener('click', function() {
        document.querySelectorAll('.nav-item').forEach(nav => {
            nav.classList.remove('active');
        });
        this.classList.add('active');
    });
});


