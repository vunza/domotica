///////////////////////
// Variaveis Globais //
///////////////////////

/**
 * Endpoint da API responsável por retornar o nome do dispositivo ESP.
 * @constant {string}
 */
const api_get_dev_name = '/api/get_esp_name';

/**
 * Endpoint da API que reinicia o dispositivo ESP.
 * @constant {string}
 */
const api_restart_dev = '/api/restart_device';

/**
 * Endpoint da API que renomeia o dispositivo ESP.
 * @constant {string}
 */
const api_rename_device = '/api/rename_esp';

/**
 * Quantidade máxima de caracteres permitidos no nome do dispositivo.
 * Definido no firmware (constantes.h).
 * @type {number|null}
 */
let device_name_sie = null;

/**
 * Nome atual do dispositivo, obtido via API.
 * @type {string|null}
 */
let nome_do_dispositivo = null;



/////////////////////////////////
// Listener da Página Carregada //
/////////////////////////////////

/**
 * Evento disparado quando o DOM está totalmente carregado.
 * Configura:
 *  - input de alteração do nome do ESP,
 *  - contador de caracteres,
 *  - ícone de renomear dispositivo,
 *  - atualização do header,
 *  - integração com renomeação via API.
 */
document.addEventListener("DOMContentLoaded", async function () {

    // Controle de caracteres do input
    const input = document.getElementById("device-name-input");
    const counter = document.getElementById("char-counter");

    // Obter nome e limite de caracteres do dispositivo
    const resposta = await callAPI(api_get_dev_name);
    const name_size = resposta.split(",");

    device_name_sie = name_size[1];
    nome_do_dispositivo = name_size[0];

    // Configura input
    input.setAttribute("autocomplete", "off");
    input.maxLength = device_name_sie;
    input.placeholder = `Máximo ${device_name_sie} Caracteres`;

    // Atualiza nome no cabeçalho
    document.getElementById('device-name').innerText = nome_do_dispositivo;

    /**
     * Atualiza a contagem de caracteres no input e o estado visual do
     * ícone de renomeação do dispositivo.
     * @function updateCounter
     * @returns {void}
     */
    function updateCounter() {
        const svg_renomear_esp = document.getElementById("svg_renomear_esp");
        const len = input.value.length;

        counter.textContent = `${len} de ${device_name_sie} Caracteres`;
        counter.classList.remove("warning", "error");

        // Verifica nome e habilita/desabilita botão de renomeação
        if (nome_do_dispositivo === input.value) {
            svg_renomear_esp.style.pointerEvents = 'none';
            svg_renomear_esp.style.opacity = 0.7;
            svg_renomear_esp.setAttribute('color', 'currentColor');

        } else if (len > 0) {
            svg_renomear_esp.style.pointerEvents = 'auto';
            svg_renomear_esp.style.cursor = 'pointer';
            svg_renomear_esp.setAttribute('color', 'green');
            svg_renomear_esp.style.opacity = 1;

        } else {
            svg_renomear_esp.style.pointerEvents = 'none';
            svg_renomear_esp.style.opacity = 0.7;
            svg_renomear_esp.setAttribute('color', 'currentColor');
        }

        // Controle de cor do contador
        if (len >= device_name_sie) {
            counter.classList.add("error");
        } else if (len >= device_name_sie * 0.75) {
            counter.classList.add("warning");
        }
    }

    // Atualiza contador inicialmente
    updateCounter();

    // Evento de digitação
    input.addEventListener("input", updateCounter);

    // Evento de clique no ícone de renomear
    svg_renomear_esp.addEventListener("click", async () => {
        const novoNome = document.getElementById("device-name-input").value;

        // Envia solicitação de renomeação à API
        await renomearDispositivo(api_rename_device, novoNome);

        // Recarrega a página após renomear
        window.location.reload();
    });

}); // Fim do DOMContentLoaded



///////////////////////////////
// OBTER NOME DO DISPOSITIVO //
///////////////////////////////

/**
 * Executa um fetch simples e retorna a resposta como texto.
 * Usado para APIs do ESP que retornam texto puro.
 *
 * @async
 * @function callAPI
 * @param {string} api - URL da API.
 * @returns {Promise<string|null>} Texto retornado pela API ou `null` em caso de erro.
 */
async function callAPI(api) {
    try {
        const response = await fetch(api);
        return await response.text();

    } catch (err) {
        console.error("Erro no fetch:", err);
        return null;
    }
}



////////////////////////////////////////////////////
// TERMINAR OTA POR VIA DO RESTART DO DISPOSITIVO //
////////////////////////////////////////////////////

/**
 * Envia comando para reiniciar o dispositivo e redireciona
 * o navegador para a página principal.
 *
 * @async
 * @function btnRestartDevice
 * @returns {Promise<void>}
 */
async function btnRestartDevice() {
    await callAPI(api_restart_dev);
    window.location.href = "/";
}



/**
 * Envia novo nome para o ESP via método POST.
 *
 * @function renomearDispositivo
 * @param {string} api_rename_device - Endpoint da API de renomeação.
 * @param {string} novoNome - Novo nome desejado para o dispositivo.
 * @returns {void}
 */
function renomearDispositivo(api_rename_device, novoNome) {

    fetch(api_rename_device, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: novoNome })
    })
    .then(r => r.text())
    .then(resp => {
        console.log("Servidor respondeu: " + resp);
    })
    .catch(err => console.error("Erro:", err));
}





//////////////////////////////////////////////
// Altera elementos da página de ElegantOTA //
//////////////////////////////////////////////

/**
 * Manipula o conteúdo carregado dentro do iframe OTA para remover
 * elementos indesejados gerados pelo AsyncElegantOTA e aplicar CSS
 * personalizado para melhorar a interface.
 *
 * @event load
 * @listens iframe#otaFrame:load
 */
document.getElementById("otaFrame").addEventListener("load", function () {

    const iframeDoc = this.contentDocument || this.contentWindow.document;

    if (!iframeDoc) {
        console.error("Não foi possível acessar o conteúdo do iframe.");
        return;
    }

    // Remover elementos após o OTA injetar o HTML
    setTimeout(() => {

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
    }, 500);

    // Injetar CSS customizado
    const style = iframeDoc.createElement("style");
    style.textContent = `
        .mt-2 { margin-top: 0px !important; }
        .mt-3 { margin-top: 0px !important; }
        .pt-2 { padding-top: 0 !important; }

        @media (max-width: 480px) {
            .col-12 { width: fit-content !important; }
        }
    `;

    iframeDoc.head.appendChild(style);

    console.log("CSS com !important aplicado dentro da página OTA!");
});
