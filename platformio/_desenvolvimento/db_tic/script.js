let globalEquipment = [];
let globalEmails = [];
let globalUsers = [];
let currentUserRole = null;

window.onload = function() {
    checkSession();
};

async function checkSession() {
    try {
        const res = await fetch('api.php?action=check_session');
        const data = await res.json();
        if (data.success) {
            currentUserRole = data.role;
            applyUserPermissions();
            document.getElementById('loginScreen').classList.add('hidden');
            document.getElementById('appContainer').classList.remove('hidden');
            loadData();
        } else {
            showLoginScreen();
        }
    } catch (err) {
        showLoginScreen();
    }
}

function showLoginScreen() {
    document.getElementById('loginUser').value = '';
    document.getElementById('loginPass').value = '';
    document.getElementById('appContainer').classList.add('hidden');
    document.getElementById('loginScreen').classList.remove('hidden');
}

async function handleLogin(e) {
    e.preventDefault();
    const username = document.getElementById('loginUser').value;
    const password = document.getElementById('loginPass').value;

    try {
        const res = await fetch('api.php?action=login', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username, password })
        });
        const data = await res.json();
        if (data.success) {
            currentUserRole = data.role;
            applyUserPermissions();
            document.getElementById('loginScreen').classList.add('hidden');
            document.getElementById('appContainer').classList.remove('hidden');
            loadData();
            showToast('Sessão iniciada com sucesso!');
        } else {
            alert(data.error || 'Credenciais inválidas');
        }
    } catch(err) {
        alert('Erro de ligação ao servidor');
    }
}

async function restartAuthentication() {
    try {
        await fetch('api.php?action=logout');
    } catch(e) {}
    
    currentUserRole = null;
    showLoginScreen();
    showToast('Sessão terminada.');
}

function applyUserPermissions() {
    const btnEq = document.getElementById('btnNewEquipment');
    const btnEm = document.getElementById('btnNewEmail');
    const btnUser = document.getElementById('btnNewUser');
    const tabUsers = document.getElementById('tabUsersBtn');
    const roleDisplay = document.getElementById('userRoleDisplay');

    if (currentUserRole === 'admin') {
        if(btnEq) btnEq.classList.remove('hidden');
        if(btnEm) btnEm.classList.remove('hidden');
        if(btnUser) btnUser.classList.remove('hidden');
        if(tabUsers) tabUsers.classList.remove('hidden'); // Visível para admin
        if(roleDisplay) roleDisplay.innerText = 'Perfil: Administrador';
    } else {
        if(btnEq) btnEq.classList.add('hidden');
        if(btnEm) btnEm.classList.add('hidden');
        if(btnUser) btnUser.classList.add('hidden');
        if(tabUsers) tabUsers.classList.add('hidden');    // Oculto para utilizadores normais
        if(roleDisplay) roleDisplay.innerText = 'Perfil: Utilizador Normal (Consulta)';
        
        // Se um utilizador normal estiver acidentalmente na aba de utilizadores, volta para equipamentos
        const usersSec = document.getElementById('usersSection');
        if(usersSec && !usersSec.classList.contains('hidden')) {
            switchTab('equipment');
        }
    }
}

function toggleDarkMode() {
    const html = document.documentElement;
    html.classList.toggle('dark');
    html.classList.toggle('light');
}

function switchTab(tab) {
    const eqSec = document.getElementById('equipmentSection');
    const emSec = document.getElementById('emailsSection');
    const userSec = document.getElementById('usersSection');
    
    const eqBtn = document.getElementById('tabEquipmentBtn');
    const emBtn = document.getElementById('tabEmailsBtn');
    const userBtn = document.getElementById('tabUsersBtn');

    // Esconder todas as secções
    if(eqSec) eqSec.classList.add('hidden');
    if(emSec) emSec.classList.add('hidden');
    if(userSec) userSec.classList.add('hidden');

    // Resetar estilos de todas as abas
    if(eqBtn) eqBtn.className = "pb-3 text-sm font-semibold border-b-2 border-transparent text-slate-500 hover:text-slate-400 flex items-center space-x-2 transition-all whitespace-nowrap";
    if(emBtn) emBtn.className = "pb-3 text-sm font-semibold border-b-2 border-transparent text-slate-500 hover:text-slate-400 flex items-center space-x-2 transition-all whitespace-nowrap";
    if(userBtn) userBtn.className = "pb-3 text-sm font-semibold border-b-2 border-transparent text-slate-500 hover:text-slate-400 flex items-center space-x-2 transition-all whitespace-nowrap";

    // Ativar a aba selecionada
    if (tab === 'equipment') {
        if(eqSec) eqSec.classList.remove('hidden');
        if(eqBtn) eqBtn.className = "pb-3 text-sm font-semibold border-b-2 border-blue-600 text-blue-600 flex items-center space-x-2 transition-all whitespace-nowrap";
    } else if (tab === 'emails') {
        if(emSec) emSec.classList.remove('hidden');
        if(emBtn) emBtn.className = "pb-3 text-sm font-semibold border-b-2 border-blue-600 text-blue-600 flex items-center space-x-2 transition-all whitespace-nowrap";
    } else if (tab === 'users') {
        // Proteção extra: Se não for admin, impede abrir a aba de utilizadores
        if (currentUserRole !== 'admin') {
            switchTab('equipment');
            return;
        }
        if(userSec) userSec.classList.remove('hidden');
        if(userBtn) userBtn.className = "pb-3 text-sm font-semibold border-b-2 border-blue-600 text-blue-600 flex items-center space-x-2 transition-all whitespace-nowrap";
    }

    // Garante que a visibilidade da aba de utilizadores respeita sempre o perfil do utilizador
    if (userBtn) {
        if (currentUserRole === 'admin') {
            userBtn.classList.remove('hidden');
        } else {
            userBtn.classList.add('hidden');
        }
    }
}

async function loadData() {
    try {
        const res = await fetch('api.php?action=list');
        const data = await res.json();
        if (data.success) {
            globalEquipment = data.equipment;
            globalEmails = data.emails;
            globalUsers = data.users || [];
            renderEquipmentTable();
            renderEmailTable();
            renderUserTable();
        } else {
            restartAuthentication();
        }
    } catch (err) {
        showToast('Erro de comunicação com o servidor', true);
    }
}

// Funções de Gestão de Utilizadores
function renderUserTable() {
    const tbody = document.getElementById('userTableBody');
    if (!tbody) return;

    if (globalUsers.length === 0) {
        tbody.innerHTML = `<tr><td colspan="4" class="py-8 text-center text-slate-400 text-xs">Nenhum utilizador encontrado.</td></tr>`;
        return;
    }

    let html = '';
    globalUsers.forEach(u => {
        let deleteBtn = `<span class="inline-flex items-center space-x-1 text-xs text-slate-400 bg-slate-100 dark:bg-slate-800 px-2.5 py-1 rounded-lg">
                            <svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M12 1L3 5v6c0 5.55 3.84 10.74 9 12 5.16-1.26 9-6.45 9-12V5l-9-4zm-2 16l-4-4 1.41-1.41L10 14.17l6.59-6.59L18 9l-8 8z"/></svg>
                            <span>Principal</span>
                         </span>`;
                         
        if (u.username !== 'admin') {
            deleteBtn = `<button onclick="deleteUser(${u.id})" class="w-8 h-8 rounded-lg bg-rose-50 text-rose-600 hover:bg-rose-100 dark:bg-rose-900/30 dark:text-rose-400 transition-colors inline-flex items-center justify-center" title="Eliminar Utilizador">
                            <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></svg>
                         </button>`;
        }

        // Ícones específicos por perfil
        const roleBadge = u.role === 'admin' 
            ? `<span class="inline-flex items-center space-x-1.5 px-2.5 py-1 rounded-md text-xs font-semibold bg-purple-50 text-purple-700 dark:bg-purple-900/30 dark:text-purple-300 border border-purple-200 dark:border-purple-800">
                <svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M12 1L3 5v6c0 5.55 3.84 10.74 9 12 5.16-1.26 9-6.45 9-12V5l-9-4zm-1 6h2v2h-2V7zm0 4h2v6h-2v-6z"/></svg>
                <span>Administrador</span>
               </span>`
            : `<span class="inline-flex items-center space-x-1.5 px-2.5 py-1 rounded-md text-xs font-semibold bg-blue-50 text-blue-700 dark:bg-blue-900/30 dark:text-blue-300 border border-blue-200 dark:border-blue-800">
                <svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M12 12c2.21 0 4-1.79 4-4s-1.79-4-4-4-4 1.79-4 4 1.79 4 4 4zm0 2c-2.67 0-8 1.34-8 4v2h16v-2c0-2.66-5.33-4-8-4z"/></svg>
                <span>Consulta</span>
               </span>`;

        html += `
            <tr class="hover:bg-slate-50/80 dark:hover:bg-slate-700/50 transition-colors">
                <td class="py-4 px-6 font-medium text-slate-900 dark:text-white flex items-center space-x-3">
                    <div class="w-8 h-8 rounded-lg bg-slate-100 text-slate-600 dark:bg-slate-700 dark:text-slate-300 flex items-center justify-center flex-shrink-0">
                        <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 3c1.66 0 3 1.34 3 3s-1.34 3-3 3-3-1.34-3-3 1.34-3 3-3zm0 14.2c-2.5 0-4.71-1.28-6-3.22.03-1.99 4-3.08 6-3.08 1.99 0 5.97 1.09 6 3.08-1.29 1.94-3.5 3.22-6 3.22z"/></svg>
                    </div>
                    <span>${escapeHtml(u.username)}</span>
                </td>
                <td class="py-4 px-6">${roleBadge}</td>
                <td class="py-4 px-6 text-slate-500 text-xs">
                    <div class="flex items-center space-x-1.5">
                        <svg class="w-3.5 h-3.5 fill-current text-slate-400" viewBox="0 0 24 24"><path d="M19 3h-1V1h-2v2H8V1H6v2H5c-1.11 0-1.99.9-1.99 2L3 19c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zm0 16H5V8h14v11z"/></svg>
                        <span>${escapeHtml(u.created_at)}</span>
                    </div>
                </td>
                <td class="py-4 px-6 text-right">${deleteBtn}</td>
            </tr>
        `;
    });
    tbody.innerHTML = html;
}



function openUserModal() {
    document.getElementById('userForm').reset();
    document.getElementById('userModal').classList.remove('hidden');
    document.getElementById('userModal').classList.add('flex');
}
function closeUserModal() {
    document.getElementById('userModal').classList.remove('flex');
    document.getElementById('userModal').classList.add('hidden');
}

function openChangePasswordModal() {
    document.getElementById('changePasswordForm').reset();
    document.getElementById('changePasswordModal').classList.remove('hidden');
    document.getElementById('changePasswordModal').classList.add('flex');
}
function closeChangePasswordModal() {
    document.getElementById('changePasswordModal').classList.remove('flex');
    document.getElementById('changePasswordModal').classList.add('hidden');
}

async function saveUser(e) {
    e.preventDefault();
    const payload = {
        username: document.getElementById('newUsername').value,
        password: document.getElementById('newPasswordUser').value,
        role: document.getElementById('newRole').value
    };

    const res = await fetch('api.php?action=save_user', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(payload) });
    const data = await res.json();
    if (data.success) {
        closeUserModal();
        loadData();
        showToast('Utilizador registado com sucesso!');
    } else {
        alert(data.error);
    }
}

async function deleteUser(id) {
    if (confirm('Tem a certeza que pretende eliminar este utilizador?')) {
        const res = await fetch('api.php?action=delete_user', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ id }) });
        const data = await res.json();
        if (data.success) {
            loadData();
            showToast('Utilizador eliminado com sucesso!');
        } else {
            alert(data.error);
        }
    }
}

async function changePassword(e) {
    e.preventDefault();
    const payload = {
        old_password: document.getElementById('oldPassword').value,
        new_password: document.getElementById('newPassword').value
    };

    const res = await fetch('api.php?action=change_password', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(payload) });
    const data = await res.json();
    if (data.success) {
        closeChangePasswordModal();
        showToast('Senha alterada com sucesso!');
    } else {
        alert(data.error);
    }
}

// Gestão de Equipamentos e Modais
function handleEquipmentTypeChange() {
    const type = document.getElementById('eqType').value;
    const networkFields = document.getElementById('networkFields');
    const switchFields = document.getElementById('switchFields');

    if (['Computador', 'Telefone VoIP', 'Switch'].includes(type)) {
        networkFields.classList.remove('hidden');
    } else {
        networkFields.classList.add('hidden');
    }

    if (['Computador', 'Telefone VoIP'].includes(type)) {
        switchFields.classList.remove('hidden');
    } else {
        switchFields.classList.add('hidden');
    }
}

function openDetailsModal(item) {
    const modal = document.getElementById('detailsModal');
    const content = document.getElementById('detailsContent');
    let html = `<div class="space-y-3"><div><span class="text-xs font-semibold text-slate-400 uppercase">Nome</span><p class="font-medium">${escapeHtml(item.name)}</p></div><div><span class="text-xs font-semibold text-slate-400 uppercase">Tipo</span><p>${escapeHtml(item.type)}</p></div><div><span class="text-xs font-semibold text-slate-400 uppercase">Localização</span><p>${escapeHtml(item.location || '-')}</p></div>`;
    if (['Computador', 'Telefone VoIP', 'Switch'].includes(item.type)) {
        html += `<div><span class="text-xs font-semibold text-slate-400 uppercase">IP</span><p class="font-mono">${escapeHtml(item.ip || '-')}</p></div><div><span class="text-xs font-semibold text-slate-400 uppercase">MAC</span><p class="font-mono">${escapeHtml(item.mac || '-')}</p></div>`;
    }
    if (['Computador', 'Telefone VoIP'].includes(item.type)) {
        html += `<div class="border-t pt-2 grid grid-cols-2 gap-2"><div><span class="text-xs font-semibold text-slate-400 uppercase">Switch</span><p>${escapeHtml(item.switch || '-')}</p></div><div><span class="text-xs font-semibold text-slate-400 uppercase">Porta</span><p>${escapeHtml(item.port || '-')}</p></div></div>`;
    }
    html += `</div>`;
    content.innerHTML = html;
    modal.classList.remove('hidden'); modal.classList.add('flex');
}

function closeDetailsModal() { 
    document.getElementById('detailsModal').classList.remove('flex'); 
    document.getElementById('detailsModal').classList.add('hidden'); 
}

function openEquipmentModal(item = null) {
    if (currentUserRole !== 'admin') return;
    document.getElementById('equipmentForm').reset();
    if (item) {
        document.getElementById('equipmentModalTitle').innerText = 'Editar Equipamento';
        document.getElementById('eqId').value = item.id;
        document.getElementById('eqName').value = item.name;
        document.getElementById('eqType').value = item.type;
        document.getElementById('eqMac').value = item.mac || '';
        document.getElementById('eqIp').value = item.ip || '';
        document.getElementById('eqSerial').value = item.serial_number || '';
        document.getElementById('eqLocation').value = item.location || '';
        document.getElementById('eqStatus').value = item.status;
        document.getElementById('eqSwitch').value = item.switch || '';
        document.getElementById('eqPort').value = item.port || '';
    } else {
        document.getElementById('equipmentModalTitle').innerText = 'Novo Equipamento';
        document.getElementById('eqId').value = '';
    }
    handleEquipmentTypeChange();
    document.getElementById('equipmentModal').classList.remove('hidden');
    document.getElementById('equipmentModal').classList.add('flex');
}

function closeEquipmentModal() { 
    document.getElementById('equipmentModal').classList.remove('flex'); 
    document.getElementById('equipmentModal').classList.add('hidden'); 
}

function openEmailModal(item = null) {
    if (currentUserRole !== 'admin') return;
    document.getElementById('emailForm').reset();
    if (item) {
        document.getElementById('emailModalTitle').innerText = 'Editar Conta de E-mail';
        document.getElementById('emId').value = item.id;
        document.getElementById('emAccount').value = item.account;
        document.getElementById('emOwner').value = item.owner;
        document.getElementById('emStatus').value = item.status;
    } else {
        document.getElementById('emailModalTitle').innerText = 'Nova Conta de E-mail';
        document.getElementById('emId').value = '';
    }
    document.getElementById('emailModal').classList.remove('hidden');
    document.getElementById('emailModal').classList.add('flex');
}

function closeEmailModal() { 
    document.getElementById('emailModal').classList.remove('flex'); 
    document.getElementById('emailModal').classList.add('hidden'); 
}

async function saveEquipment(e) {
    e.preventDefault();
    const payload = {
        id: document.getElementById('eqId').value || null,
        name: document.getElementById('eqName').value,
        type: document.getElementById('eqType').value,
        mac: document.getElementById('eqMac').value,
        ip: document.getElementById('eqIp').value,
        serial_number: document.getElementById('eqSerial').value,
        location: document.getElementById('eqLocation').value,
        status: document.getElementById('eqStatus').value,
        switch: document.getElementById('eqSwitch').value,
        port: document.getElementById('eqPort').value
    };
    const res = await fetch('api.php?action=save_equipment', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(payload) });
    const data = await res.json();
    if (data.success) { closeEquipmentModal(); loadData(); showToast('Guardado com sucesso!'); } else { alert(data.error); }
}

async function deleteEquipment(id) {
    if (confirm('Tem a certeza que pretende eliminar?')) {
        const res = await fetch('api.php?action=delete_equipment', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ id }) });
        const data = await res.json();
        if (data.success) { loadData(); showToast('Eliminado com sucesso!'); } else { alert(data.error); }
    }
}

async function saveEmail(e) {
    e.preventDefault();
    const payload = {
        id: document.getElementById('emId').value || null,
        account: document.getElementById('emAccount').value,
        owner: document.getElementById('emOwner').value,
        status: document.getElementById('emStatus').value
    };
    const res = await fetch('api.php?action=save_email', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(payload) });
    const data = await res.json();
    if (data.success) { closeEmailModal(); loadData(); showToast('Conta guardada!'); } else { alert(data.error); }
}

async function deleteEmail(id) {
    if (confirm('Tem a certeza que pretende eliminar?')) {
        const res = await fetch('api.php?action=delete_email', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ id }) });
        const data = await res.json();
        if (data.success) { loadData(); showToast('Conta eliminada!'); } else { alert(data.error); }
    }
}

// Funções de Renderização e Utilitários Visuais
function getEquipmentBadge(type) {
    let colorClass = 'bg-blue-50 text-blue-700 border-blue-200 dark:bg-blue-900/30 dark:text-blue-300 dark:border-blue-800';
    let iconSvg = '';

    switch(type) {
        case 'Computador':
            colorClass = 'bg-blue-50 text-blue-700 border-blue-200 dark:bg-blue-900/30 dark:text-blue-300 dark:border-blue-800';
            iconSvg = `<svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M4 6h16v10H4V6m16 12h2v2H2v-2h2V4h16v14z"/></svg>`;
            break;
        case 'Telefone VoIP':
            colorClass = 'bg-indigo-50 text-indigo-700 border-indigo-200 dark:bg-indigo-900/30 dark:text-indigo-300 dark:border-indigo-800';
            iconSvg = `<svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M6.62 10.79c1.44 2.83 3.76 5.14 6.59 6.59l2.2-2.2c.27-.27.67-.36 1.02-.24 1.12.37 2.33.57 3.57.57.55 0 1 .45 1 1V20c0 .55-.45 1-1 1-9.39 0-17-7.61-17-17 0-.55.45-1 1-1h3.5c.55 0 1 .45 1 1 0 1.25.2 2.45.57 3.57.11.35.03.74-.25 1.02l-2.2 2.2z"/></svg>`;
            break;
        case 'Monitor':
            colorClass = 'bg-emerald-50 text-emerald-700 border-emerald-200 dark:bg-emerald-900/30 dark:text-emerald-300 dark:border-emerald-800';
            iconSvg = `<svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M21 2H3c-1.1 0-2 .9-2 2v12c0 1.1.9 2 2 2h7v2H8v2h8v-2h-2v-2h7c1.1 0 2-.9 2-2V4c0-1.1-.9-2-2-2zm0 14H3V4h18v12z"/></svg>`;
            break;
        case 'Projetor':
            colorClass = 'bg-purple-50 text-purple-700 border-purple-200 dark:bg-purple-900/30 dark:text-purple-300 dark:border-purple-800';
            iconSvg = `<svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M23 7l-5 3v4l5 3V7zM1 9h15v6H1V9z"/></svg>`;
            break;
        case 'Switch':
            colorClass = 'bg-amber-50 text-amber-700 border-amber-200 dark:bg-amber-900/30 dark:text-amber-300 dark:border-amber-800';
            iconSvg = `<svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M2 13h20v-2H2v2m0-6h20V5H2v2m0 12h20v-2H2v2z"/></svg>`;
            break;
        default:
            colorClass = 'bg-slate-100 text-slate-700 border-slate-200 dark:bg-slate-800 dark:text-slate-300 dark:border-slate-700';
            iconSvg = `<svg class="w-3.5 h-3.5 fill-current" viewBox="0 0 24 24"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/></svg>`;
    }

    return `<span class="inline-flex items-center space-x-1.5 px-2.5 py-1 rounded-lg text-xs font-semibold border ${colorClass}">${iconSvg}<span>${escapeHtml(type)}</span></span>`;
}

function getStatusBadge(status) {
    if (status === 'Ativo') {
        return `<span class="inline-flex items-center px-2.5 py-1 rounded-full text-xs font-semibold bg-emerald-50 text-emerald-700 dark:bg-emerald-900/30 dark:text-emerald-400"><span class="w-1.5 h-1.5 rounded-full bg-emerald-500 mr-1.5"></span>Ativo</span>`;
    } else if (status === 'Manutenção') {
        return `<span class="inline-flex items-center px-2.5 py-1 rounded-full text-xs font-semibold bg-amber-50 text-amber-700 dark:bg-amber-900/30 dark:text-amber-400"><span class="w-1.5 h-1.5 rounded-full bg-amber-500 mr-1.5"></span>Manutenção</span>`;
    } else {
        return `<span class="inline-flex items-center px-2.5 py-1 rounded-full text-xs font-semibold bg-rose-50 text-rose-700 dark:bg-rose-900/30 dark:text-rose-400"><span class="w-1.5 h-1.5 rounded-full bg-rose-500 mr-1.5"></span>Inativo</span>`;
    }
}

function renderEquipmentTable() {
    const query = document.getElementById('searchEquipment').value.toLowerCase();
    const filterType = document.getElementById('filterType').value;
    const tbody = document.getElementById('equipmentTableBody');
    if (!tbody) return;

    const filtered = globalEquipment.filter(item => {
        const matchSearch = item.name.toLowerCase().includes(query) ||
                            (item.mac && item.mac.toLowerCase().includes(query)) ||
                            (item.ip && item.ip.toLowerCase().includes(query)) ||
                            (item.serial_number && item.serial_number.toLowerCase().includes(query)) ||
                            (item.location && item.location.toLowerCase().includes(query)) ||
                            (item.switch && item.switch.toLowerCase().includes(query));
        const matchType = filterType === '' || item.type === filterType;
        return matchSearch && matchType;
    });

    if (filtered.length === 0) {
        tbody.innerHTML = `<tr><td colspan="4" class="py-8 text-center text-slate-400 text-xs">Nenhum equipamento encontrado.</td></tr>`;
        return;
    }

    let html = '';
    filtered.forEach(item => {
        let actionsHtml = `
            <button onclick='openDetailsModal(${JSON.stringify(item)})' class="w-8 h-8 rounded-lg bg-emerald-50 text-emerald-600 hover:bg-emerald-100 dark:bg-emerald-900/30 dark:text-emerald-400 transition-colors inline-flex items-center justify-center" title="Ver Detalhes">
                <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/></svg>
            </button>`;
        
        if (currentUserRole === 'admin') {
            actionsHtml += `
                <button onclick='openEquipmentModal(${JSON.stringify(item)})' class="w-8 h-8 rounded-lg bg-blue-50 text-blue-600 hover:bg-blue-100 dark:bg-blue-900/30 dark:text-blue-400 transition-colors inline-flex items-center justify-center ml-1" title="Editar">
                    <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M3 17.25V21h3.75L17.81 9.94l-3.75-3.75L3 17.25zM20.71 7.04c.39-.39.39-1.02 0-1.41l-2.34-2.34c-.39-.39-1.02-.39-1.41 0l-1.83 1.83 3.75 3.75 1.83-1.83z"/></svg>
                </button>
                <button onclick="deleteEquipment(${item.id})" class="w-8 h-8 rounded-lg bg-rose-50 text-rose-600 hover:bg-rose-100 dark:bg-rose-900/30 dark:text-rose-400 transition-colors inline-flex items-center justify-center ml-1" title="Eliminar">
                    <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></svg>
                </button>
            `;
        }

        html += `
            <tr class="hover:bg-slate-50/80 dark:hover:bg-slate-700/50 transition-colors">
                <td class="py-4 px-6"><div class="font-medium text-slate-900 dark:text-white">${escapeHtml(item.name)}</div><div class="mt-1">${getEquipmentBadge(item.type)}</div></td>
                <td class="py-4 px-6 text-slate-600 dark:text-slate-300">${escapeHtml(item.location || '-')}</td>
                <td class="py-4 px-6">${getStatusBadge(item.status)}</td>
                <td class="py-4 px-6 text-right">${actionsHtml}</td>
            </tr>
        `;
    });
    tbody.innerHTML = html;
}

function renderEmailTable() {
    const query = document.getElementById('searchEmail').value.toLowerCase();
    const tbody = document.getElementById('emailTableBody');
    if (!tbody) return;

    const filtered = globalEmails.filter(item => {
        return item.account.toLowerCase().includes(query) || item.owner.toLowerCase().includes(query);
    });

    if (filtered.length === 0) {
        tbody.innerHTML = `<tr><td colspan="4" class="py-8 text-center text-slate-400 text-xs">Nenhuma conta encontrada.</td></tr>`;
        return;
    }

    let html = '';
    filtered.forEach(item => {
        let actionsHtml = '';
        if (currentUserRole === 'admin') {
            actionsHtml = `
                <button onclick='openEmailModal(${JSON.stringify(item)})' class="w-8 h-8 rounded-lg bg-blue-50 text-blue-600 hover:bg-blue-100 dark:bg-blue-900/30 dark:text-blue-400 transition-colors inline-flex items-center justify-center" title="Editar">
                    <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M3 17.25V21h3.75L17.81 9.94l-3.75-3.75L3 17.25zM20.71 7.04c.39-.39.39-1.02 0-1.41l-2.34-2.34c-.39-.39-1.02-.39-1.41 0l-1.83 1.83 3.75 3.75 1.83-1.83z"/></svg>
                </button>
                <button onclick="deleteEmail(${item.id})" class="w-8 h-8 rounded-lg bg-rose-50 text-rose-600 hover:bg-rose-100 dark:bg-rose-900/30 dark:text-rose-400 transition-colors inline-flex items-center justify-center ml-1" title="Eliminar">
                    <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></svg>
                </button>
            `;
        } else {
            actionsHtml = `<span class="text-xs text-slate-400">Apenas leitura</span>`;
        }

        html += `
            <tr class="hover:bg-slate-50/80 dark:hover:bg-slate-700/50 transition-colors">
                <td class="py-4 px-6 font-medium text-slate-900 dark:text-white flex items-center space-x-3">
                    <div class="w-8 h-8 rounded-lg bg-indigo-50 text-indigo-600 dark:bg-indigo-900/30 dark:text-indigo-400 flex items-center justify-center flex-shrink-0">
                        <svg class="w-4 h-4 fill-current" viewBox="0 0 24 24"><path d="M20 4H4c-1.1 0-1.99.9-1.99 2L2 18c0 1.1.9 2 2 2h16c1.1 0 2-.9 2-2V6c0-1.1-.9-2-2-2zm0 4l-8 5-8-5V6l8 5 8-5v2z"/></svg>
                    </div>
                    <span>${escapeHtml(item.account)}</span>
                </td>
                <td class="py-4 px-6 text-slate-600 dark:text-slate-300">${escapeHtml(item.owner)}</td>
                <td class="py-4 px-6">${getStatusBadge(item.status)}</td>
                <td class="py-4 px-6 text-right">${actionsHtml}</td>
            </tr>
        `;
    });
    tbody.innerHTML = html;
}

function showToast(msg) {
    const t = document.getElementById('toast');
    if (!t) return;
    t.innerText = msg;
    t.classList.remove('translate-y-20', 'opacity-0');
    setTimeout(() => t.classList.add('translate-y-20', 'opacity-0'), 3000);
}

function escapeHtml(str) { 
    return !str ? '' : str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;"); 
}