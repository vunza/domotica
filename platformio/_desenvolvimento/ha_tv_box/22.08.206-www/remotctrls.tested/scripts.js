
const tokenInput = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJjMDUwYTVkNTMzZDU0ZjBmODJlY2NjZmQyZDgwZDRlYyIsImlhdCI6MTczNjcxNzU1NSwiZXhwIjoyMDUyMDc3NTU1fQ.oLIiyFN8c4lPCAgIPL_sFEWI12P3IcsLsid4KL8A2D0';
const http_prot = window.location.protocol; // Guarda http: OU https:

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
});


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


////////////////////////////////////////////////
// Assinar tópico MQTT (via WebSocket ou API) //
////////////////////////////////////////////////
async function obterCodigoIR(ip_porta) {
    const url = `${ip_porta}/api/states/sensor.0xf082c0fffeedf8c4_learned_ir_code`;    
    fetch(url, {
        headers: {
            "Authorization": `Bearer ${tokenInput}`,
            "Content-Type": "application/json"
        }
    })
    .then(res => res.json())
    .then(data => console.log("Código IR: ", data.state))
    .catch(err => console.error("Erro:", err));  
}



//////////////////////////////////////////////////////////////
// Exibir a página após o carregamento completo dos estilos //
//////////////////////////////////////////////////////////////
async function showPage() {
    document.getElementById("div_ctrl_r_wrappper").style.display = "block";  
    document.getElementById("tbl_key_pad_fan").style.display = "block";   
}



///////////////////////////////////////////////////
// Envia comando IR para o dispositivo, via mqtt //
///////////////////////////////////////////////////
async function send_ir_cmd(btnId){      
    
    obterDadosIR(`${http_prot}//${location.host}`)
    .then(dados => {   
        // Envia o comando IR para o dispositivo   
        const getobj = dados.find(item => item.bth_id === btnId);  // Busca o objeto correspondente ao botão clicado
        publishMQTT(`${http_prot}//${location.host}`, `zigbee2mqtt/${getobj.hub_id}/set`, `{"ir_code_to_send": "${getobj.ir_code}"}`);
    })
    .catch(error => {
        console.error("Erro:", error);
    });


    // TODO: (*J*) No arquivo .json deve ssubstituir as ' por " e remover os espaços em branco
    const hubs_ir = [
        { bth_id: "td_timer_fan", hub_id: "0xf082c0fffeedf8c4", ir_code: "AAA/OGFSHNMSKKW==" },
        { bth_id: "btn_lamp_fan", hub_id: "0xf082c0fffeedf9d5", ir_code: "A00/OGFSHNMSKKW==" }      
    ];     
    //guardaCodigosIR(`${http_prot}//${location.host}`, hubs_ir);
}




////////////////////////
// Guardar Codigos IR //
////////////////////////
async function guardaCodigosIR(ip_porta, codigo_ir) {

    const token = tokenInput;     
    const url = `${ip_porta}/api/services/shell_command/guardar_codigos_ir`;

    fetch(url, {
    method: "POST",
    headers: {
        "Authorization": `Bearer ${token}`,
        "Content-Type": "application/json"
    },
    body: JSON.stringify({"data": codigo_ir})
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




//////////////////////
// Obter Codigos IR //
//////////////////////    
async function obterDadosIR(ip_porta) {

    // const timestamp = new Date().getTime(); // Gera um número único
    // const url = `${ip_porta}/local/json_files/codigos_ir.json?t=${timestamp}`;    
    // fetch(url)
    // .then(response => {
    //     if (!response.ok) {
    //     throw new Error('Erro ao carregar o arquivo JSON');
    //     }        
    //     return response.json();
    // })
    // .then(data => {
    //     console.log('Dados carregados:', data);
    // })
    // .catch(error => {
    //     console.error('Erro:', error);
    // });


    const timestamp = new Date().getTime();
    const url = `${ip_porta}/local/json_files/codigos_ir.json?t=${timestamp}`;

    const response = await fetch(url);
    if (!response.ok) {
        throw new Error('Erro ao carregar o arquivo JSON');
    }
    const data = await response.json();
    return data; // Retorna diretamente o conteúdo do JSON
   
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
function abrir_menu_ctrl_r() {
    document.getElementById("header_menu_ctrl_r").style.display = "none";
    document.getElementById("div_ctrl_remoto_fan").style.display = "none";
    document.getElementById("nav_menu_ctrl_r").style.display = "block";

    document.getElementById("div_ctrl_r_wrappper").style.border = "1px solid #4e94ab";
    
    // Colapsa todos os itens, do menu ctrl remoto, que têm submenu
    document.querySelectorAll(".has-submenu.active").forEach((item) => {
      item.classList.remove("active");
    });
}
   

///////////////////////////////////////////
// Abrir controlo remoto para execucacao //
///////////////////////////////////////////
function executar_ctrl_remoto(){
    document.getElementById("header_menu_ctrl_r").style.display = "block";
    document.getElementById("div_ctrl_remoto_fan").style.display = "flex";
    document.getElementsByClassName("vertical-menu")[0].style.display = "none";
    document.getElementById("div_ctrl_r_wrappper").style.border = "1px solid #39be5a";
    document.getElementById("div_ctrl_r_wrappper").style.width = "auto";
}


////////////////////////////////////////////
// Abrir controlo remoto para programacao //
////////////////////////////////////////////
function programar_ctrl_remoto(){
    document.getElementById("header_menu_ctrl_r").style.display = "block";
    document.getElementById("div_ctrl_remoto_fan").style.display = "flex";
    document.getElementsByClassName("vertical-menu")[0].style.display = "none";
    document.getElementById("div_ctrl_r_wrappper").style.border = "1px solid #B03A2E";
    document.getElementById("div_ctrl_r_wrappper").style.width = "auto";    
}