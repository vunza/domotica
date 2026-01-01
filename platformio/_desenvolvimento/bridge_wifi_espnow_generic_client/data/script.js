///////////////////////
// Variaveis Globais //
///////////////////////

/**
 * Endpoint para obter o nome atual do dispositivo ESP.
 * @constant {string}
 */
const api_get_dev_name = '/api/get_esp_name';

/**
 * Endpoint responsável por reiniciar o dispositivo ESP.
 * @constant {string}
 */
const api_restart_dev = '/api/restart_device';

/**
 * Endpoint que renomeia o dispositivo ESP.
 * @constant {string}
 */
const api_rename_device = '/api/rename_esp';

/**
 * Número máximo de caracteres permitidos no nome do dispositivo,
 * informado pelo firmware (constantes.h).
 * @type {number|null}
 */
let device_name_sie = null;

/**
 * Nome atual do dispositivo, conforme retornado pela API.
 * @type {string|null}
 */
let nome_do_dispositivo = null;



/////////////////////////////////
// Listner da Pagina Carregada //
/////////////////////////////////

/**
 * Manipula toda a inicialização da página após o carregamento do DOM.
 * - Carrega nome do dispositivo.
 * - Configura o campo de edição.
 * - Controla contador de caracteres.
 * - Habilita/desabilita ícone de renomear.
 * - Aplica estilos visuais.
 */
document.addEventListener("DOMContentLoaded", async function () {

    // Controla caracteres no input para renomear o ESP
    const input = document.getElementById("device-name-input");
    const counter = document.getElementById("char-counter");

    // Obtém nome do dispositivo e tamanho máximo permitido
    const resposta = await callAPI(api_get_dev_name);
    const name_size = resposta.split(",");
    device_name_sie = name_size[1];
    nome_do_dispositivo = name_size[0];

    // Configura o input
    input.setAttribute("autocomplete", "off");
    input.maxLength = device_name_sie;
    input.placeholder = `Máximo ${device_name_sie} Caracteres`;

    // Atualiza o cabeçalho da página
    document.getElementById('device-name').innerText = nome_do_dispositivo;

    /**
     * Atualiza contador de caracteres, ajusta classes de alerta
     * e modifica o comportamento visual do ícone de renomear.
     *
     * @function updateCounter
     */
    function updateCounter() {
        const svg_renomear_esp = document.getElementById("svg_renomear_esp");
        const len = input.value.length;

        counter.textContent = `${len} de ${device_name_sie} Caracteres`;
        counter.classList.remove("warning", "error");

        // Controle do ícone de renomeação
        if (nome_do_dispositivo === input.value) {
            svg_renomear_esp.style.pointerEvents = 'none';
            svg_renomear_esp.style.opacity = 0.7;
            svg_renomear_esp.setAttribute('color', 'currentColor');

        } else if (len > 0) {
            svg_renomear_esp.style.pointerEvents = 'auto';
            svg_renomear_esp.style.cursor = 'pointer';
            svg_renomear_esp.setAttribute('color', 'green');
            svg_renomear_esp.style.opacity = 1;

        } else { // len == 0
            svg_renomear_esp.style.pointerEvents = 'none';
            svg_renomear_esp.style.opacity = 0.7;
            svg_renomear_esp.setAttribute('color', 'currentColor');
        }

        // Altera cor do contador conforme limite
        if (len >= device_name_sie) {
            counter.classList.add("error");
        } else if (len >= device_name_sie * 0.75) {
            counter.classList.add("warning");
        }
    }

    // Chamada inicial do contador
    updateCounter();

    // Atualiza contador durante digitação
    input.addEventListener("input", updateCounter);


    /**
     * Evento de clique no ícone que envia novo nome para a API
     * e recarrega a página após renomear o ESP.
     */
    svg_renomear_esp.addEventListener("click", async () => {

        const novoNome = document.getElementById("device-name-input").value;
        await renomearDispositivo(api_rename_device, novoNome);

        // Recarrega página após renomear
        window.location.reload();
    });

}); // fim do DOMLoaded



///////////////////////////////
// OBTER NOME DO DISPOSITIVO //
///////////////////////////////

/**
 * Chama uma API e retorna o corpo da resposta como texto.
 *
 * @async
 * @function callAPI
 * @param {string} api - URL da API que será requisitada.
 * @returns {Promise<string|null>} Texto retornado pela API, ou null se ocorrer erro.
 */
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

/**
 * Envia comando de restart ao dispositivo e redireciona
 * o navegador de volta ao painel principal.
 *
 * @async
 * @function btnRestartDevice
 * @returns {Promise<void>}
 */
async function btnRestartDevice() {
    await callAPI(api_restart_dev);
    window.location.href = "/painel";
}



/**
 * Envia requisição POST para renomear o dispositivo ESP.
 *
 * @function renomearDispositivo
 * @param {string} api_rename_device - Endpoint para renomeação.
 * @param {string} novoNome - Novo nome que será aplicado ao ESP.
 * @returns {void}
 */
function renomearDispositivo(api_rename_device, novoNome) {

    fetch(api_rename_device, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: novoNome })
    })
        .then(r => r.text())
        .then(resp => console.log("Servidor respondeu: " + resp))
        .catch(err => console.error("Erro:", err));
}



//////////////////////////////////////////////
// Altera elementos da pagina de ElegantOTA //
//////////////////////////////////////////////

/**
 * Manipula o documento carregado dentro do iframe OTA para:
 * - remover elementos <div> contendo <span> internos,
 * - aplicar CSS customizado dentro do iframe.
 *
 * Esse ajuste é necessário porque o AsyncElegantOTA injeta conteúdo via JS.
 *
 * @event load
 * @listens HTMLIFrameElement#load
 */
document.getElementById("otaFrame").addEventListener("load", function () {

    const iframeDoc = this.contentDocument || this.contentWindow.document;

    if (!iframeDoc) {
        console.error("Não foi possível acessar o conteúdo do object.");
        return;
    }

    // Remover divs que contêm spans (conteúdo gerado pelo OTA)
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

    }, 500); // tempo para o OTA carregar conteúdo via JS


    // Injeta CSS customizado no iframe
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
