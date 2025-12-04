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
        
    `;

    // Inserir o CSS dentro do HEAD do documento carregado
    iframeDoc.head.appendChild(style);

    console.log("CSS com !important aplicado dentro da página OTA!");
});






