
/**
 * Endpoint que retorna o nome atual do dispositivo ESP.
 * @constant {string}
 */
const api_get_dev_name = '/api/get_esp_name';

/**
 * Endpoint responsável por reiniciar o dispositivo ESP.
 * @constant {string}
 */
const api_restart_dev = '/api/restart_device';

/**
 * Endpoint utilizado para renomear o dispositivo ESP.
 * @constant {string}
 */
const api_rename_device = '/api/rename_esp';

/**
 * Tamanho máximo permitido para o nome do dispositivo,
 * obtido a partir da API (definido no firmware).
 * @type {number|null}
 */
let device_name_sie = null;

/**
 * Nome atual do dispositivo retornado pela API.
 * @type {string|null}
 */
let nome_do_dispositivo = null;




/**
 * Evento disparado quando o DOM foi completamente carregado.
 * Aqui são configurados:
 *  - Entrada de texto para renomear o ESP.
 *  - Contador de caracteres.
 *  - Comportamentos visuais do ícone de renomear.
 *  - Manipulações do iframe do ElegantOTA.
 */
document.addEventListener("DOMContentLoaded", async function () {

    // Captura elementos da interface
    const input = document.getElementById("device-name-input");
    const counter = document.getElementById("char-counter");

    // Obtém nome e limite de caracteres do dispositivo
    const resposta = await callAPI(api_get_dev_name);
    const name_size = resposta.split(",");
    device_name_sie = Number(name_size[1]);
    nome_do_dispositivo = name_size[0];

    // Configura propriedades do input
    input.setAttribute("autocomplete", "off");
    input.maxLength = device_name_sie;
    input.placeholder = `Máximo ${device_name_sie} Caracteres`;

    // Atualiza o header com o nome do dispositivo
    document.getElementById('device-name').innerText = nome_do_dispositivo;

    /**
     * Atualiza o contador de caracteres, estilos do ícone de renomear
     * e aplica regras visuais conforme a validade do nome.
     * @function updateCounter
     */
    function updateCounter() {
        const svg_renomear_esp = document.getElementById("svg_renomear_esp");
        const len = input.value.length;

        counter.textContent = `${len} de ${device_name_sie} Caracteres`;
        counter.classList.remove("warning", "error");

        // Controle do ícone de renomear
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

        // Cores do contador baseado na proximidade do limite
        if (len >= device_name_sie) {
            counter.classList.add("error");
        } else if (len >= device_name_sie * 0.75) {
            counter.classList.add("warning");
        }
    }

    // Inicializa contador ao carregar
    updateCounter();

    // Atualiza contador a cada digitação
    input.addEventListener("input", updateCounter);

    // Evento para renomear dispositivo ao clicar no ícone
    svg_renomear_esp.addEventListener("click", async () => {

        const novoNome = document.getElementById("device-name-input").value;

        // Envia nome para API
        await renomearDispositivo(api_rename_device, novoNome);

        // Recarrega página
        window.location.reload();
    });

}); // FIM do DOMContentLoaded




/**
 * Chama uma API e retorna a resposta como texto.
 * @async
 * @function callAPI
 * @param {string} api - URL do endpoint a ser chamado.
 * @returns {Promise<string|null>} Texto retornado pela API ou null em caso de erro.
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




/**
 * Reinicia o dispositivo via API e redireciona para /painel.
 * @async
 * @function btnRestartDevice
 * @returns {Promise<void>}
 */
async function btnRestartDevice() {
    await callAPI(api_restart_dev);
    window.location.href = "/painel";
}




/**
 * Envia o novo nome do dispositivo para a API de renomeação.
 * @function renomearDispositivo
 * @param {string} api_rename_device - Endpoint para renomear o ESP.
 * @param {string} novoNome - Novo nome desejado.
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




/**
 * Manipula elementos internos do iframe da ElegantOTA:
 *  - Remove divs contendo spans,
 *  - Aplica CSS customizado,
 *  - Ajusta layout responsivo.
 *
 * O código espera 500ms pois o conteúdo é montado via JS pelo ElegantOTA.
 */
document.getElementById("otaFrame").addEventListener("load", function () {
    const iframeDoc = this.contentDocument || this.contentWindow.document;

    if (!iframeDoc) {
        console.error("Não foi possível acessar o conteúdo do object.");
        return;
    }

    // Remoção de divs contendo spans
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

    // Criação e injeção de CSS dentro do iframe
    const style = iframeDoc.createElement("style");
    style.textContent = `
        .mt-2 { margin-top: 0px !important; }
        .mt-3 { margin-top: 0px !important; }
        .pt-2 { padding-top: 0 !important; }

        @media (max-width: 480px) {
            .col-12 {
                width: fit-content !important;
            }
        }
    `;

    iframeDoc.head.appendChild(style);

    console.log("CSS com !important aplicado dentro da página OTA!");
});
