

///////////////////////
// Variaveis Globaal //
///////////////////////
var ieeeaddress = null;
var timer_index = null;
let arrays_tmrs = {};
var deviceList = [];
const porta_ws_mqtt = 8091;
const client_mqtt = mqtt.connect(`ws://${location.host}:${porta_ws_mqtt}`);

//////////////////////
// Documento pronto //
//////////////////////
window.addEventListener('DOMContentLoaded', function () {

    window.addEventListener('contextmenu', event => {
        // Nao exibir o menu contexto do click direito
        event.preventDefault();
    });

    // Limpar select option da Acção
    document.getElementById('select_accao').selectedIndex = -1;


    // Solicita lista dos dispositivos
    tx_data2python('DEVS_LIST');    
    
   

    // Conectar-se ao servidor MQTT   
    client_mqtt.on('connect', () => {
        //console.log('Conectado ao Broker');
        client_mqtt.subscribe('zigbee2mqtt/#', (err) => {
            if (!err) {
                //console.log('Subscrito no tópico:');
            }
        });
    });


    // Processar mensgens vindas do Servidor MQTT
    client_mqtt.on('message', (topic, message) => {

        // Actualiza estado dos devices 
        updateDevState(topic, message);

        //tx_data2python('DEVS_STATE');

        // Processar Timers do dispositivos
        processDevicesTimers(message, topic);

    });


    // Intervalo para verificar o array
    let intervalo = setInterval(() => {
        if (deviceList.length > 0) {
            clearInterval(intervalo);
            ask_dev_state();
            // Solicita o estado dos dispositivos 
            //tx_data2python('DEVS_STATE');           
        }
    }, 500);    
   

}); // window.addEventListener('DOMContentLoaded', function () 


////////////////////////////////////////////////////
// Tx/Rx dados para/do Servidor http Flask/Python //
////////////////////////////////////////////////////
function tx_data2python(cmnd) { 

    fetch('/tx_rx__dados', {        
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: `comando=${encodeURIComponent(cmnd)}`,
    }).then(response => response.text()).then(data => {

        
        if(cmnd == 'DEVS_LIST'){
            // Processa resposta do comando 'DEVS_LIST'
            deviceList = JSON.parse(data);
            putDevices2DOM(deviceList);
        }
        else if(cmnd == 'DEVS_STATE'){
            // Processa resposta do comando 'DEVS_STATE' 
            console.log(data);           
        }
        
    }).catch(error => {
        console.error('Erro:', error);
    });

}



////////////////////////////////////
// Solicitar estado dos elementos //
////////////////////////////////////
function ask_dev_state() {

    deviceList.forEach((item) => {
        // String original
        let id_alterado = item.ieeeAddress;
        // Remove o último caractere
        let id_orig = id_alterado.slice(0, -1);

        setTimeout(() => {
            const topic = 'zigbee2mqtt/' + id_orig + '/get';
            const payload = '{"state":""}';
            client_mqtt.publish(topic, payload);           
        }, 1000);
    });

}




//////////////////////////////
// Create devices from array //
//////////////////////////////
function putDevices2DOM(devicesArray) {    
    // Criar Dispositivos
    const msg = devicesArray;
    for (let cont = 0; cont < msg.length; cont++) {
        // Cria os dispositivos
        //friendlyName
        const objdev = new Dispositivo(msg[cont].ieeeAddress, msg[cont].ieeeAddress, img_lampada);
        // Criar card, se nao existe na DOM
        if (document.getElementById(msg[cont].ieeeAddress + 'devCardWrapper')) {
            continue;
        } else {
            criar_device(objdev);
        }
    }
}




//////////////////////////////////
// Processar Estado dos devices //
//////////////////////////////////
function processDevicesState(msg) {

    let estado = null;
    let devId = null;

    if (msg.Estado.state) {
        estado = msg.Estado.state;
        devId = `${msg.Id}1`;
        setDevState(devId, devId, estado);
    }

    if (msg.Estado.state_left) {
        estado = msg.Estado.state_left;
        devId = `${msg.Id}1`;
        setDevState(devId, devId, estado);
    }

    if (msg.Estado.state_center) {
        estado = msg.Estado.state_center;
        devId = `${msg.Id}2`;
        setDevState(devId, devId, estado);
    }

    if (msg.Estado.state_right && msg.Estado.state_center) {
        estado = msg.Estado.state_right;
        devId = `${msg.Id}3`;
        setDevState(devId, devId, estado);
    }
    else if (msg.Estado.state_right && !msg.Estado.state_center) {
        estado = msg.Estado.state_right;
        devId = `${msg.Id}2`;
        setDevState(devId, devId, estado);
    }

} //processDevicesState()



//////////////////////////////////
// Processar Timers dos devices //
//////////////////////////////////
function processDevicesTimers(message, topic) {

    // Inspeccionar JSON
    let parsedMessage;

    try {
        parsedMessage = JSON.parse(message.toString());
    } catch (error) {
        console.error('Erro ao converter a mensagem para JSON:', error);
        return;
    }

    if (topic == 'GET_DEV_TIMER') {

        // Obter, do objecto JSON, os elementos do Timer
        var timerobj = JSON.parse(parsedMessage);

        if (parsedMessage == null) {
            // Limpar elementos
            resetElementosTimers();

            // Marca o elemento, radio selecionar timee, marcado
            var radList = document.getElementsByName('select_timer');
            for (var i = 0; i < radList.length; i++) {
                if ((timer_index - 1) == i) radList[i].checked = true;
            }

            // Restituir estado do botão "Guardar"
            document.getElementById("div_guardar_tmrs").disabled = false;
            document.getElementById("div_guardar_tmrs").querySelector('svg').style.stroke = "#39be5a";

            return;
        }

        // Estado do CheckBox Activar
        if (timerobj.Activo == true) {
            document.getElementById("chk_activar").checked = true;
        }
        else if (timerobj.Activo == false) {
            document.getElementById('chk_activar').checked = false;
        }

        // Opcao do select Accao
        // (*J*) Se existir mais de 2 duas acções a lógica deve mudar
        if (timerobj.Accao == 'ON') {
            document.getElementById('select_accao').selectedIndex = 1;
        }
        else if (timerobj.Accao == 'OFF') {
            document.getElementById('select_accao').selectedIndex = 0;
        }


        // Separa os elementos da expressao do cron
        const partes = timerobj.HoraWeekDays.split(' ');
        const segundos = partes[0];
        const minutos = partes[1];
        const horas = partes[2];
        //const diames = partes[3];
        //const mes = partes[4];
        const diasdasemana = partes[5];

        // Exibir Hora do Timer
        document.getElementById('time_hora').value = `${horas}:${minutos}`;

        // Marcar os dias da semana do Timer 
        let arraydiassemana = diasdasemana.split(',');
        var checBtn = document.getElementsByName("sel_week_day");
        for (let i = 0; i < arraydiassemana.length; i++) {
            var index = arraydiassemana[i];
            checBtn[index].checked = true;
        }
    }
    else if (topic == 'GET_ALL_TIMERS') {
        // Obter, do objecto JSON, os elementos dos Timers
        //var timersobj = JSON.parse(msg.payload);
        let timersarray = parsedMessage;

        // Guarda os Timers nos arrays dos Devices correspondentes
        for (let cont = 0; cont < timersarray.length; cont++) {
            let Nome = timersarray[cont].name;
            let Id = Nome.replace(/-.*/, "");
            let Payload = timersarray[cont].payload;
            let Expressao = timersarray[cont].expression;
            let IsRunning = timersarray[cont].isrunning;

            arrays_tmrs[Id].push(Expressao);
        }



        for (let cont = 0; cont < timersarray.length; cont++) {
            let Nome = timersarray[cont].name;
            let Id = Nome.replace(/-.*/, "");
        }
    }

} //processDevicesTimers()




//////////////////////////////////
// Actualiza estado dos devices //
//////////////////////////////////
function updateDevState(topic, message) {
    if (topic.includes('zigbee2mqtt/0x')) {

        // Inspeccionar JSON
        let parsedMessage;
        try {
            parsedMessage = JSON.parse(message.toString());
        } catch (error) {
            console.error('Erro ao converter a mensagem para JSON:', error);
            return;
        }

        const msg = parsedMessage;

        if (msg.state != 'online' && !topic.includes('availability') && topic.includes('zigbee2mqtt/0x') && msg != 'TOGGLE' && msg.state != 'TOGGLE' && !topic.includes('bridge') && !topic.includes('Cordinator') && !topic.includes('get')) {

            // Obter nome do device
            const parts = (topic).split('/');
            const ieeeAddress = parts[1];
            const dev_estado = msg;

            const data = {
                Id: ieeeAddress,
                Estado: dev_estado
            };

            // Actualiza estado dos dispositivos
            processDevicesState(data);
        }


    }
}


//////////////////////////////////////////////////////
// Inicializa processo para obtencao da Hora actual //
//////////////////////////////////////////////////////
setInterval(obter_data_hora, 1000);



///////////////////////////////////////////////////////
// Exibir Data/Hora no Dlg de configurações de Timer //
///////////////////////////////////////////////////////
function obter_data_hora() {
    //var strcount
    const weekday = ["Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sabado"];
    var currentdate = new Date();
    var formato = `<font color="white" face="Verdana, Geneva, sans-serif" size="+1">${weekday[currentdate.getDay()]}</font>`;
    formato += '<font color="yellow" face="Verdana, Geneva, sans-serif" size="+1">';

    var hours = currentdate.getHours();
    var minutes = currentdate.getMinutes();
    var seconds = currentdate.getSeconds();

    if (hours < 10) hours = "0" + hours;
    if (minutes < 10) minutes = "0" + minutes;
    if (seconds < 10) seconds = "0" + seconds;

    formato += " " + hours + ":" + minutes + ":" + seconds; formato += "</font>";
    document.getElementById("hora_actual").innerHTML = formato;

} // Fim de obter_data_hora()




////////////////////////
// Classe Dispositivo //
////////////////////////
class Dispositivo {
    constructor(id, nome, img) {
        this.Id = id;
        this.Nome = nome;
        this.Img = img;
    }
}


/////////////////
// Imagens SVG //
/////////////////    
const svg_clock = '<svg class="svg_clk" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><polyline points="12 6 12 12 16 14"></polyline></svg>';
const img_lampada = '<svg id="svg_lamp" viewBox="-80 0 512 512"><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z" /></svg>';

let cards_container = document.getElementById('cards_container');
const historico = '00:00:00, 00/00/0000';





function cronToNextDate(cronExpr) {
    const [sec, min, hour, dayOfMonth, month, dayOfWeek] = cronExpr.split(' ');

    const now = new Date();
    let nextEvent = new Date(now);

    // Definir o mês (0-11 em JavaScript) e o dia do mês (1-31)
    let monthNum = (month === '*') ? now.getMonth() : Number(month) - 1;
    let dayNum = (dayOfMonth === '*') ? now.getDate() : Number(dayOfMonth);

    // Verificar se os dias da semana foram definidos
    const daysOfWeek = dayOfWeek.split(',').map(Number).filter(d => !isNaN(d));

    // Encontrar o próximo dia da semana correspondente
    const currentDay = now.getDay(); // 0 = Domingo, 1 = Segunda, ..., 6 = Sábado
    let nextDayOffset = 0;
    if (daysOfWeek.length > 0) {
        // Encontre o próximo dia da semana a partir do atual
        const nextDay = daysOfWeek.find(day => day >= currentDay) || daysOfWeek[0] + 7;
        nextDayOffset = (nextDay >= currentDay) ? nextDay - currentDay : 7 - (currentDay - nextDay);
    }

    // Configurar a data do próximo evento
    nextEvent.setDate(now.getDate() + nextDayOffset);
    nextEvent.setMonth(monthNum);
    nextEvent.setDate(dayNum); // Ajustar o dia do mês, caso não seja '*'

    // Configurar horas, minutos e segundos
    nextEvent.setHours(Number(hour));
    nextEvent.setMinutes(Number(min));
    nextEvent.setSeconds(Number(sec));

    // Se o próximo evento for no passado, mover para o próximo mês/semana
    if (nextEvent <= now) {
        nextEvent.setDate(nextEvent.getDate() + 7); // Mover uma semana para frente
    }

    return cronExpr;//nextEvent;
}


////////////////////////////////
// Alterar estado dos devices //
////////////////////////////////
function setDevState(devId, devName, devState) {
    // Actualiza o nome do Dispositivo
    document.getElementById(devId + 'devName').innerHTML = devName;
    // Seleciona o elemento pelo ID
    const elemento = document.getElementById(devId + 'devImage');

    // Alterar cor da lampada
    if (devState == "ON") {
        alterar_cor_lamp(devId + 'devImage', '#FFEA20');
        // Define o novo valor para a propriedade box-shadow
        elemento.style.boxShadow = "2px 2px 12px 1px rgba(255, 234, 32, .5)";
    }
    else if (devState == "OFF") {
        alterar_cor_lamp(devId + 'devImage', 'lightgray');
        // Define o novo valor para a propriedade box-shadow
        elemento.style.boxShadow = "2px 2px 12px 1px rgba(140, 140, 140, .5)";
    }
}



///////////////////////////////////
// Funcao para cria dispositivos //
///////////////////////////////////
function criar_device(device) {

    if (device instanceof Dispositivo) {

        const envoltura_card = `                    
                    <div id=${device.Id + 'contextMenu'} class="menu_contextual">
                        <ul>
                            <li id="li_rnm"><a id=${device.Id + 'renameDev'}  href="#"><svg width="24" height="24"
                                        viewBox="0 0 24 24" fill="none" stroke="#FFFFFF" stroke-width="2" stroke-linecap="round"
                                        stroke-linejoin="round">
                                        <polygon points="14 2 18 6 7 17 3 17 3 13 14 2"></polygon>
                                        <line x1="3" y1="22" x2="21" y2="22"></line>
                                    </svg> Renomear</a></li>
                            <li id="li_tmr"><a id=${device.Id + 'devTimer'} href="#"><svg width="24" height="24"
                                        viewBox="0 0 24 24" fill="none" stroke="#FFFFFF" stroke-width="2" stroke-linecap="round"
                                        stroke-linejoin="round">
                                        <circle cx="12" cy="12" r="7"></circle>
                                        <polyline points="12 9 12 12 13.5 13.5"></polyline>
                                        <path
                                            d="M16.51 17.35l-.35 3.83a2 2 0 0 1-2 1.82H9.83a2 2 0 0 1-2-1.82l-.35-3.83m.01-10.7l.35-3.83A2 2 0 0 1 9.83 1h4.35a2 2 0 0 1 2 1.82l.35 3.83">
                                        </path>
                                    </svg> Temporizadores</a></li>
                            <li id="li_cfg"><a id="a_cfg" href="#"><svg width="24" height="24"
                                        viewBox="0 0 24 24" fill="none" stroke="#FFFFFF" stroke-width="2" stroke-linecap="round"
                                        stroke-linejoin="round">
                                        <line x1="4" y1="21" x2="4" y2="14"></line>
                                        <line x1="4" y1="10" x2="4" y2="3"></line>
                                        <line x1="12" y1="21" x2="12" y2="12"></line>
                                        <line x1="12" y1="8" x2="12" y2="3"></line>
                                        <line x1="20" y1="21" x2="20" y2="16"></line>
                                        <line x1="20" y1="12" x2="20" y2="3"></line>
                                        <line x1="1" y1="14" x2="7" y2="14"></line>
                                        <line x1="9" y1="8" x2="15" y2="8"></line>
                                        <line x1="17" y1="16" x2="23" y2="16"></line>
                                    </svg> Cofigurações</a></li>
                        </ul>
                    </div>

                    
                    <div id=${device.Id + 'devCard'} class="card" style="display: block;">                       
                    <div class="text_container">
                        <table id="tbl_hist_clk">
                            <tr>
                                <th rowspan="3">
                                    <div class="div_img" id=${device.Id + 'devImage'} >${device.Img}</div>
                                </th>
                            </tr>
                
                            <tr>
                                <td>
                                    <div class="div_nome" id=${device.Id + 'devName'}>${device.Nome}</div>
                                </td>
                                <td>
                                    <div id=${device.Id + 'devConfig'} class="div_cfg" roll="configurar">
                                        <svg id="svg_configurations" width="22" height="22" viewBox="0 0 22 22" fill="none" stroke="#000000" stroke-width="2"
                                            stroke-linecap="round" stroke-linejoin="round">
                                            <circle cx="12" cy="12" r="1"></circle>
                                            <circle cx="12" cy="5" r="1"></circle>
                                            <circle cx="12" cy="19" r="1"></circle>
                                        </svg>
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
                </div>`;


        ////////////////////////////////                                
        // Cria DIV envoltura da Card //
        ////////////////////////////////
        var div = document.createElement("div");
        div.innerHTML = envoltura_card;
        div.id = device.Id + 'devCardWrapper';
        div.classList.add("grid_container");
        cards_container.appendChild(div);
        //div.style.backgroundColor = '#000000';
        //console.log(device.Id);
        var menu_contexto = document.getElementById(device.Id + 'contextMenu');

        
        /////////////////////////////////////////////////////////////
        // Cria um Array vazio para os Timeres de cada Dispositivo //
        /////////////////////////////////////////////////////////////
        arrays_tmrs[device.Id] = [];



        ////////////////////////////////////////////////////////////
        // Evento Click no elemento <a> para renomear dispositivo //
        ////////////////////////////////////////////////////////////
        document.getElementById(device.Id + 'renameDev').addEventListener('click', function (event) {

            // Alterar propriedades da Card
            const rnm_dev = document.getElementById(device.Id + 'devName');
            const div_card = document.getElementById(device.Id + 'devCard');
            var guarda_html = div_card.innerHTML;
            var guardar_altura = div_card.style.height;
            div_card.style.height = '110px';
            div_card.innerHTML = `<div style="color:yellow;"><u>${rnm_dev.innerHTML}</u></div>`;
            div_card.innerHTML += document.getElementById('div_wrapper_set_name').innerHTML;

            // Evento Click na div Renomear dispositivo
            document.getElementById('div_wrapper_guardar').addEventListener('click', function () {
                const novo_nome = document.getElementById('txt_nome').value;

                // Validar o novo nome
                if (novo_nome == null || novo_nome == '' | novo_nome.lenght <= 0) return;

                // Dados a serem enviados para o fluxo/flow
                var mensagem = {
                    topico: 'zigbee2mqtt/bridge/request/device/rename',
                    carga: `{"from":"${rnm_dev.innerHTML}", "to": "${novo_nome}"}`
                };
                // Enviar os dados ao fluxo/flow
                //scope.send({ payload: JSON.stringify(mensagem) });
            });


            // Evento Click na div Fechar do dialog renomear dispositivo
            document.getElementById('div_wrapper_sair').addEventListener('click', function () {
                div_card.innerHTML = guarda_html;
                div_card.style.height = guardar_altura;

                // Adicionar Evento contexmenu ns div cfg
                add_contextmenu_event();
            });

        });




        ///////////////////////////////////////////////////////
        // Exibir Diago para configurar timer do dispositivo //
        ///////////////////////////////////////////////////////
        document.getElementById(device.Id + 'devTimer').addEventListener('click', function (event) {
            // Ocultar Cards
            document.getElementById('cards_container').style.display = 'none';

            // Exibir Dialogo Configuracoes de Timers
            var cfg_tmr = document.getElementById('div_cfg_tmrs_wrapper');
            cfg_tmr.style.position = 'absolute';
            cfg_tmr.style.zIndex = 999;
            cfg_tmr.style.display = 'block';

            // Atribuir o Nome e imagem, do dispositivo correspondente, ao dialogo Timers
            const nome_dev = document.getElementById(ieeeaddress + 'devName').innerHTML;
            document.getElementById('div_tmr_dev_name').innerHTML = `<b>${nome_dev}</b>`;
            document.getElementById('div_estado').innerHTML = document.getElementById(ieeeaddress + 'devImage').innerHTML;
        });



        ////////////////////////////////////////////////////
        // Fechar Menu contexto, ao clicar sobre a pagina //
        ////////////////////////////////////////////////////
        window.addEventListener('click', function (event) {
            menu_contexto.style.display = 'none';
            menu_contexto.style.zIndex = -999;
        });


        //////////////////////////////////////////////////////////////////////////////
        // Funcao auxiliar para adicionar evento click direito/longpress na div cfg //
        //////////////////////////////////////////////////////////////////////////////
        function add_contextmenu_event() {
            // Evento Listner click direito (long press) na div para configuracoes
            var div_cfg = document.getElementById(device.Id + 'devConfig');
            div_cfg.addEventListener("contextmenu", function (event) {
                // Guarda a base do ID dos dispositivos
                ieeeaddress = device.Id;
                // Evita que o menu de contexto padrão do navegador seja exibido
                event.preventDefault();
                menu_contexto.style.position = 'absolute';
                menu_contexto.style.zIndex = 999;
                menu_contexto.style.display = 'block';
            });
        }


        // Adicionar Evento contexmenu ns div cfg
        add_contextmenu_event();

        /////////////////////////////////////////////////////////
        // Adiciona o event listener de clique a div da imagem //
        /////////////////////////////////////////////////////////
        var divimg = document.getElementById(device.Id + 'devImage');
        divimg.addEventListener("click", function () {

            let texto = device.Id;
            let lastchar = texto[texto.length - 1];
            let ieeaddr = texto.slice(0, -1);
          
            client_mqtt.publish(`zigbee2mqtt/${ieeaddr}/${lastchar}/set`, '{"state":"TOGGLE"}');
        });
       

        // Altera cor da imagem svg
        alterar_cor_lamp(device.Id + 'devImage', 'lightgray');

    }// FIM do if (device instanceof Dispositivo)

    //scope.send({ payload: JSON.stringify(mensagem) });// Enviar os dados ao fluxo/flow

}// FIM de function criar_device(device)





//////////////////////////////////////////////////////////////////////////////
// Evento Click no botao fechar dialogo para configurar timer do dispositivo //
//////////////////////////////////////////////////////////////////////////////
document.getElementById('div_sair_tmrs').addEventListener('click', function (event) {
    // Exibir Cards
    document.getElementById('cards_container').style.display = 'block';

    // Exibir Dialogo Configuracoes de Timers
    var cfg_tmr = document.getElementById('div_cfg_tmrs_wrapper');
    cfg_tmr.style.zIndex = -998;
    cfg_tmr.style.display = 'none';

    // Limpar elementos
    resetElementosTimers();
});



///////////////////////////////////////////////////////////////
// Atribui "Eventlistaner" aos radios buttons "select_timer" //
///////////////////////////////////////////////////////////////  
const radios = document.querySelectorAll('input[name="select_timer"]');
for (const radio of radios) {
    radio.onclick = (e) => {
        // Restituir estado do botão "Guardar"
        document.getElementById("div_guardar_tmrs").disabled = false;
        document.getElementById("div_guardar_tmrs").querySelector('svg').style.stroke = "#39be5a";

        timer_index = e.target.value;

        var mensagem = {
            cmdtimer: 'GET_TIMER',
            command: "status",
            name: `${ieeeaddress}-${timer_index - 1}`
        };

        // Enviar os dados ao fluxo/flow
        //scope.send({ payload: JSON.stringify(mensagem) });

        // Checar estado do Timer (Activado/Desactivado)
        const chk_activar = document.getElementById("chk_activar");
        if (chk_activar.checked == true) {
            //document.getElementById("lbl_activar").style.color = 'green';
        }
        else if (chk_activar.checked == false) {
            //document.getElementById("lbl_activar").style.color = 'red';
        }
    }
}


//////////////////////////////////////
// Evento Click no CheckBox Activar //
//////////////////////////////////////
const chk_activar = document.getElementById("chk_activar");
chk_activar.addEventListener('click', function (event) {
    if (chk_activar.checked == true) {
        //document.getElementById("lbl_activar").style.color = 'green';
    }
    else if (chk_activar.checked == false) {
        //document.getElementById("lbl_activar").style.color = 'red';
    }
});





////////////////////////////////////////////////////////////////////////////////
// Evento Click no botao para guardar configuracoes dos timers do dispositivo //
////////////////////////////////////////////////////////////////////////////////
document.getElementById('div_guardar_tmrs').addEventListener('click', function (event) {

    let txt_nome = "";
    let chk_activar = "";
    //let saida = "";
    let accao = "";
    let hora_minutos = "";
    let dias_semana = "";

    document.getElementById("chk_activar").checked ? chk_activar = 1 : chk_activar = 0;
    var timer = document.querySelector('input[name=select_timer]:checked');
    //let sel_saida = document.getElementById("select_saida");
    let sel_accao = document.getElementById("select_accao");
    //saida = sel_saida.options[sel_saida.selectedIndex].text;
    sel_accao.options[sel_accao.selectedIndex].text == "Ligar" ? accao = "ON" : accao = "OFF";
    hora_minutos = document.getElementById("time_hora").value;
    dias_semana = obtemDiaSemana();

    let TimerX = timer.value - 1;

    // Separa hora e minutos
    let partes = hora_minutos.split(":");
    let hora = partes[0];
    let minutos = partes[1];

    // Conformar dias da semana           
    let dias = [];

    for (let i = 0; i < dias_semana.length; i++) {
        if (dias_semana[i] === "1") {
            dias.push(i);
        }
    }

    // cron => * * * * * * => seg min hor dia-do-mes (1-31) mes (1-12) dia-da-semana (0-6)

    var mensagem = {
        cmdtimer: 'CONFIG_TIMER',
        command: "add",
        name: `${ieeeaddress}-${TimerX}`,
        topic: `zigbee2mqtt/${ieeeaddress}/set`,
        expression: `0 ${minutos} ${hora} * * ${dias.join(",")}`,
        payload: `{"state":"${accao}"}`,
        payloadType: "default",
        persistDynamic: true
    };

    // Enviar os dados ao fluxo/flow
    //scope.send({ payload: JSON.stringify(mensagem) });


    // Envia o estado do Timer
    var timer_state = null;
    (chk_activar == 1) ? timer_state = "resume" : timer_state = "pause";
    var mensaje = {
        cmdtimer: 'TIMER_STATE',
        command: timer_state,
        name: `${ieeeaddress}-${TimerX}`
    };

    // Enviar os dados ao fluxo/flow
    //scope.send({ payload: JSON.stringify(mensaje) });

    // Limpar elementos
    resetElementosTimers();
});


/////////////////////////
// Alterar cor lampada //
/////////////////////////
function alterar_cor_lamp(Id, Cor) {
    const svgElement = document.getElementById(Id);
    const pathElement = svgElement.querySelector('#svg_lamp path');
    if (pathElement) {
        pathElement.style.fill = Cor;
    }
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


/////////////////////////////////////////////////////////////    
// Reset elementos do Dialogo para configuração dos Timers //
/////////////////////////////////////////////////////////////
function resetElementosTimers() {
    // Mudar estado dos elementos
    document.getElementById("div_guardar_tmrs").disabled = true;
    document.getElementById("div_guardar_tmrs").querySelector('svg').style.stroke = "gray";

    var radList = document.getElementsByName('select_timer');
    for (var i = 0; i < radList.length; i++) { radList[i].checked = false; }

    var chckList = document.getElementsByName('sel_week_day');
    for (var i = 0; i < chckList.length; i++) { chckList[i].checked = false; }

    document.getElementById("chk_activar").checked = false;
    //document.getElementById('select_saida').selectedIndex = -1;
    document.getElementById('select_accao').selectedIndex = -1;
    document.getElementById("time_hora").value = "00:00 AM";

} // Fim de resetElementos()

