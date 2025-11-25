
async function atualizar(){
    try {
        const r = await fetch('/sensor.json');
        const json = await r.json();

        document.getElementById("tensao").innerText = json.tensao.toFixed(2);
        document.getElementById("corrente").innerText = json.corrente.toFixed(2);
    } catch(e){
        console.log("Erro ao ler sensores", e);
    }
}

setInterval(atualizar, 2000);
atualizar();