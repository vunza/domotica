const horaActualElement = document.getElementById('hora_actual');
const diaSemanaElement = document.getElementById('dia_semana_actual');
const timeInput = document.getElementById('time_hora');
const customCheckbox = document.querySelector('.custom-checkbox');
const chkActivar = document.getElementById('chk_activar');
const timerSelectors = document.querySelectorAll('.timer-selector-input');
const weekdayCheckboxes = document.querySelectorAll('.weekday-checkbox');
const weekdaysLabel = document.getElementById('weekdays-label');
const selectAccao = document.getElementById('select_accao');

const diasDaSemana = [
    'Domingo', 'Segunda-feira', 'Terça-feira', 'Quarta-feira',
    'Quinta-feira', 'Sexta-feira', 'Sábado'
];

function atualizarDataHora() {
    const agora = new Date();
    
    // Atualizar hora
    const horas = String(agora.getHours()).padStart(2, '0');
    const minutos = String(agora.getMinutes()).padStart(2, '0');
    const segundos = String(agora.getSeconds()).padStart(2, '0');
    horaActualElement.textContent = `${horas}:${minutos}:${segundos}`;
    
    // Atualizar dia da semana
    const diaSemana = agora.getDay();
    diaSemanaElement.textContent = diasDaSemana[diaSemana];
}

function fecharConfigTimers() {
    console.log('Fechando configurações...');
    alert('Configurações fechadas');
}

function guardarConfigsTimers() {
    const temporizadorAtivo = document.querySelector('.timer-selector-input:checked').value;
    const ativado = chkActivar.checked;
    const acao = selectAccao.value;
    const hora = timeInput.value;
    
    // Validação: ação deve ser selecionada
    if (!acao) {
        alert('Por favor, selecione uma ação!');
        selectAccao.focus();
        return;
    }
    
    // Validação: hora deve ser preenchida
    if (!hora) {
        alert('Por favor, defina uma hora!');
        timeInput.focus();
        return;
    }
    
    const diasSelecionados = [];
    weekdayCheckboxes.forEach((checkbox, index) => {
        if (checkbox.checked) {
            diasSelecionados.push(index);
        }
    });
    
    console.log('Configurações salvas:', {
        temporizador: temporizadorAtivo,
        ativado: ativado,
        acao: acao,
        hora: hora,
        dias: diasSelecionados
    });
    
    alert('Configurações salvas com sucesso!');
}

// Configurar eventos
customCheckbox.addEventListener('click', () => {
    chkActivar.checked = !chkActivar.checked;
    customCheckbox.setAttribute('aria-checked', chkActivar.checked);
});

chkActivar.addEventListener('change', () => {
    customCheckbox.setAttribute('aria-checked', chkActivar.checked);
});

// Removido o código que pergunta para usar hora atual
// O campo agora tem apenas o placeholder "00:00"

// Selecionar todos os dias com duplo clique no label
weekdaysLabel.addEventListener('dblclick', () => {
    const todosMarcados = Array.from(weekdayCheckboxes).every(cb => cb.checked);
    weekdayCheckboxes.forEach(checkbox => {
        checkbox.checked = !todosMarcados;
    });
});

// Limpar a marca d'água quando o select for clicado
selectAccao.addEventListener('focus', function() {
    if (this.value === "") {
        // Opcional: alterar a cor quando focado
        this.style.color = 'white';
    }
});

// Quando uma opção for selecionada, garantir que não é a placeholder
selectAccao.addEventListener('change', function() {
    if (this.value !== "") {
        this.style.color = 'white';
    } else {
        this.style.color = 'rgba(255, 255, 255, 0.5)';
    }
});

// Atribui "Eventlistaner" aos radios buttons "select_timer".    
const radios = document.querySelectorAll('input[name="select_timer"]');
for (const radio of radios) {
    radio.onclick = (e) => {
       
        const label_innerHTML = `Activar Temporizador: <span style="color: #41bdf5; font-size: 1.2em; font-weight: bold;">${e.target.value}</span>`;
        document.getElementById('lbl_activar_temporizador').innerHTML = label_innerHTML;
        

        //obtemConfigTimer(save_dev_id, e.target.value);
    }
}

// Inicializar
atualizarDataHora();
setInterval(atualizarDataHora, 1000);

// Limpar selecção dos temporizadores
if (timerSelectors.length > 0) {
    //timerSelectors[0].checked = true;   
    for (var i = 0; i < timerSelectors.length; i++) { timerSelectors[i].checked = false; }
}

customCheckbox.setAttribute('aria-checked', 'false');

// Inicializar o select com a cor da marca d'água
selectAccao.style.color = 'rgba(255, 255, 255, 0.5)';


window.fecharConfigTimers = function () {
    document.getElementById('cards_main_wrapper').style.display = 'block';
    document.getElementById('config-timers-container').style.display = 'none';   
    window.location.href = 'index.html';   
}


export { fecharConfigTimers, guardarConfigsTimers };