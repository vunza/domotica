///////////////////////
// Variaveis Globais //
///////////////////////
const api_get_dev_name = '/api/get_esp_name'; 
const api_restart_dev = '/api/restart_device'; 
const api_rename_device = '/api/rename_esp';
let device_name_sie = null;
let nome_do_dispositivo = null;


/////////////////////////////////
// Listner da Pagina Carregada //
/////////////////////////////////
document.addEventListener("DOMContentLoaded", async function () {

    // Cotrola o Numero de caracters no input para alterar o nome do ESP
    const input = document.getElementById("device-name-input");
    const counter = document.getElementById("char-counter");

    // Obter Nome do Dispositivo
    const resposta = await callAPI(api_get_dev_name);
    const name_size = resposta.split(",");
    device_name_sie = name_size[1]; // Definido no arquivo constantes.h
    nome_do_dispositivo = name_size[0];

    // Altera propriedades do input
    input.setAttribute( "autocomplete", "off" );
    input.maxLength = device_name_sie;
    input.placeholder = `Máximo ${device_name_sie} Caracteres`;
        
    // Actualizar o nome do dispodsitivo no Header
    document.getElementById('device-name').innerText = nome_do_dispositivo;
   

    function updateCounter() {
        const svg_renomear_esp = document.getElementById("svg_renomear_esp");
        const len = input.value.length;
        counter.textContent = `${len} de ${device_name_sie} Caracteres`;
        counter.classList.remove("warning", "error");

        // Verifica o nome indicado e o numero de caracteres
        if (nome_do_dispositivo === input.value) {            
            svg_renomear_esp.style.pointerEvents = 'none';
            svg_renomear_esp.style.opacity = 0.7;
            svg_renomear_esp.setAttribute('color', 'currentColor');
        }else if (len > 0) {            
            svg_renomear_esp.style.pointerEvents = 'auto';
            svg_renomear_esp.style.cursor = 'pointer';
            svg_renomear_esp.setAttribute('color', 'green'); 
            svg_renomear_esp.style.opacity = 1;
        } else if (len == 0) {            
            svg_renomear_esp.style.pointerEvents = 'none';
            svg_renomear_esp.style.opacity = 0.7;
            svg_renomear_esp.setAttribute('color', 'currentColor');
        }        

        // Altera cor do contador
        if (len >= device_name_sie) {
            counter.classList.add("error");
        } else if (len >= device_name_sie * 0.75) {
            counter.classList.add("warning");
        } 
    }

    // Atualiza contagem ao carregar
    updateCounter();

    // Evento input no input
    input.addEventListener("input", updateCounter);  


    // Evento Click no icon dentro do input, para renomear dispositivo
    svg_renomear_esp.addEventListener("click", async () => {  

        // Renomear o Dispositivo
        const novoNome = document.getElementById("device-name-input").value;     
        const resposta = await renomearDispositivo(api_rename_device, novoNome);

        // Obter Nome do Dispositivo
        //const respoonse = await callAPI(api_restart_dev);

        // Recarrega o Navegador
        window.location.reload();
    });

    
}); // FIM do addEventListener()


///////////////////////////////
// OBTER NOME DO DISPOSITIVO //
///////////////////////////////
async function callAPI(api) {
    try {
        const response = await fetch(api);
        const texto = await response.text();
        return texto;   
    } catch (err) {
        console.error("Erro no fetch:", err);
        return null;
    }
}


////////////////////////////////////////////////////
// TERMINAR OTA POR VIA DO RESTART DO DISPOSITIVO //
////////////////////////////////////////////////////
async function btnRestartDevice(){
    const resposta = await callAPI(api_restart_dev);
}



function renomearDispositivo(api_rename_device, novoNome) {
    
    fetch(api_rename_device, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: novoNome })
    }).then(r => r.text()).then(resp => {
        console.log("Servidor respondeu: " + resp);
    }).catch(err => console.error("Erro:", err));
}




//////////////////////////////////////////////
// Altera elementos da pagina de ElegantOTA //
//////////////////////////////////////////////
document.getElementById("otaFrame").addEventListener("load", function () {
    const iframeDoc = this.contentDocument || this.contentWindow.document;

    if (!iframeDoc) {
        console.error("Não foi possível acessar o conteúdo do object.");
        return;
    }

    // Remover <div>> que comtem <span>
    // Espera um pouco para o AsyncElegantOTA montar o conteúdo via JS
    setTimeout(() => {
        // Opção 1: pegar todos os SPANs e remover o DIV pai de cada um
        const spans = iframeDoc.querySelectorAll('div span');
        let removidos = 0;

        spans.forEach(span => {
            const div = span.closest('div');
            if (div && div.parentNode) {
                div.parentNode.removeChild(div);
                removidos++;
            }
        });

        console.log(`Removidos ${removidos} <div> que continham <span> dentro.`);

    }, 500); // 500ms de atraso – ajuste se precisar
    

    // Criar um <style> dentro da página carregada
    const style = iframeDoc.createElement("style");
    style.textContent = `
        .mt-2 {
            margin-top: 0px !important;            
        }

        .mt-3 {
            margin-top: 0px !important;            
        }

        .pt-2 {
            padding-top: 0 !important;
        }
   
        @media (max-width: 480px) {
            .col-12{
                width: fit-content !important;
            }
        }

    `;

    // Inserir o CSS dentro do HEAD do documento carregado
    iframeDoc.head.appendChild(style);

    console.log("CSS com !important aplicado dentro da página OTA!");
});






