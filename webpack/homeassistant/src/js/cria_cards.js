
/**
 * Cria um card para um dispositivo
 * @param {String} Id - O ID do dispositivo
 * @param {Object} deviceData - Os dados do dispositivo
 * @returns {HTMLElement} O elemento do card criado
 */
const criar_card = (Id, deviceData = {}) => {
    const {
        nome = '[Nome do Dispositivo]',
        historico = '00:00:00, 00/00/0000',
        tipo = 'lampada',
        status = 'online'
    } = deviceData;

    

    // SVGs melhorados com classes CSS
    const svgs = {
        picture: `<svg width="28" height="28" fill="currentColor" viewBox="0 0 24 24" transform="" id="injected-svg"><path d="M21 14V5c0-1.1-.9-2-2-2H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h9v-2H5v-1.59l3-3 1.29 1.29c.39.39 1.02.39 1.41 0l5.29-5.29 3 3V14h2Zm-4.29-5.71a.996.996 0 0 0-1.41 0l-5.29 5.29-1.29-1.29a.996.996 0 0 0-1.41 0l-2.29 2.29V5h14v5.59L16.73 8.3Z"/><path d="M8.5 7a1.5 1.5 0 1 0 0 3 1.5 1.5 0 1 0 0-3M21 16h-2v3h-3v2h3v3h2v-3h3v-2h-3z"/></svg>`,
        interruptor: `<svg width="24" height="24" fill="currentColor" viewBox="0 0 24 24" transform="" id="injected-svg"><path d="M16 9a3 3 0 1 0 0 6 3 3 0 1 0 0-6"/><path d="M16 6H8c-3.31 0-6 2.69-6 6s2.69 6 6 6h8c3.31 0 6-2.69 6-6s-2.69-6-6-6m0 10H8c-2.21 0-4-1.79-4-4s1.79-4 4-4h8c2.21 0 4 1.79 4 4s-1.79 4-4 4"/></svg>`,
        plug: `<svg width="24" height="24" fill="currentColor" viewBox="0 0 24 24" transform="" id="injected-svg"><path d="M18.01 14.06c.26 0 .51-.1.71-.29l2.48-2.47c.47-.47.73-1.1.73-1.77s-.26-1.3-.73-1.77l-1.77-1.77 2.12-2.12-1.41-1.41-2.12 2.12-1.77-1.77c-.98-.97-2.56-.97-3.54 0l-2.47 2.47a.996.996 0 0 0 0 1.41l7.07 7.07c.2.2.45.29.71.29Zm-3.89-9.84c.13-.13.28-.15.35-.15s.23.02.35.15l4.95 4.95c.13.13.15.28.15.35s-.02.23-.15.35L18 11.64l-5.66-5.66 1.77-1.77ZM13.06 12.35l-2.12 2.12-1.41-1.41 2.12-2.12-1.41-1.41-2.12 2.12-1.41-1.41a.996.996 0 0 0-1.41 0l-2.48 2.47c-.47.47-.73 1.1-.73 1.77s.26 1.3.73 1.77l1.77 1.77-2.12 2.12 1.41 1.41L6 19.43l1.77 1.77c.49.49 1.13.73 1.77.73s1.28-.24 1.77-.73l2.47-2.47a.996.996 0 0 0 0-1.41l-1.41-1.41 2.12-2.12-1.41-1.41Zm-3.18 7.42c-.13.13-.28.15-.35.15s-.23-.02-.35-.15l-4.95-4.95c-.13-.13-.15-.28-.15-.35s.02-.23.15-.35L6 12.35l3.54 3.54 1.41 1.41.71.71-1.77 1.77Z"/></svg>`,
        hub_zb: `<svg class="svg-icon hub-icon" width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="2"></circle><path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path></svg>`,
        lampada: `<svg class="svg-icon lamp-icon" viewBox="-80 0 512 512" fill="currentColor"><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>`,
        bomba: `<svg class="svg-icon pump-icon" viewBox="0 0 1024 1024" fill="currentColor"><path d="M544.7 703.8v-320c-35.3 0-64 26-64 58.2v203.7c0 32.1 28.6 58.1 64 58.1zM576.7 383.8h288v44.5h-288zM576.7 556.3h288v64h-288zM576.7 460.3h288v64h-288zM960.7 645.6V442c0-32.1-28.7-58.2-64-58.2v320c35.4 0 64-26 64-58.2zM640.7 351.8h160c17.7 0 32-14.3 32-32v-64h-224v64c0 17.7 14.3 32 32 32zM288.7 223.8h64v-64h-64v-64h-64v64h-64v64h64z"/><path d="M832.7 799.8v-64h32v-83.5h-288v83.5h32v64h-320v-160h64v-64h32v32h64v-128h-64v32h-32v-64h-64v-128h64v-64h-192v64h64v128h-32v192h32v96h-128c-35.3 0-64 28.7-64 64v128h960.1v-128H832.7z m-64 0h-96v-64h96v64z"/><path d="M160.7 639.8v-192c-35.3 0-64 28.7-64 64h-64v64h64c0 35.4 28.6 64 64 64z"/></svg>`,
        clock: `<svg class="svg-clk" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"></circle><polyline points="12 6 12 12 16 14"></polyline></svg>`,
        config: `<svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>`,
        //pencil: `<svg width="24" height="24" fill="currentColor" viewBox="0 0 24 24" transform="" id="injected-svg"><path d="m19.41,3c-.78-.78-2.05-.78-2.83,0L4.29,15.29c-.13.13-.22.29-.26.46l-1,4c-.08.34.01.7.26.95.19.19.45.29.71.29.08,0,.16,0,.24-.03l4-1c.18-.04.34-.13.46-.26l12.29-12.29c.78-.78.78-2.05,0-2.83l-1.59-1.59Zm-11.93,15.1l-2.11.53.53-2.11L15,7.41l1.59,1.59-9.1,9.1Zm10.51-10.51l-1.59-1.59,1.59-1.59,1.59,1.58-1.59,1.59Z"/></svg>`
    };

    const getDeviceIcon = (type) => {
        const icons = {
            'hub_zb': svgs.hub_zb,
            'lampada': svgs.lampada,
            'bomba': svgs.bomba,
            'switch': svgs.plug,
            'interruptor': svgs.interruptor
        };
        return icons[type] || svgs.picture;
    };

    let container = document.getElementById('card_wrapper');
    
    // Checar se o elemento já existe
    if(document.getElementById(Id)) return;

    const cardHTML = `
        <div id="${Id}" class="card" data-status="${status}" data-type="${tipo}">
            <div class="srvr_indicator ${status !== 'online' ? 'offline' : ''}"></div>
            
            <div class="card-content">
                <div class="div_img ${tipo}-icon" id="${Id}.img" onclick="click_on_image_card(this.id);">
                    ${getDeviceIcon(tipo)}
                </div>
                
                <div class="card-info">
                    <div class="card-header">
                        <div class="div_nome">${nome}</div>
                        <div style="display:none" class="config-button" id="${Id}" oncontextmenu="on_click_cfg(this.id);">
                            ${svgs.config}
                        </div>
                    </div>
                    
                    <div class="history-section">
                        <div class="div_img_clk">
                            ${svgs.clock}
                        </div>
                        <div class="div_historico">${historico}</div>
                    </div>
                </div>
            </div>
        </div>
    `;  

    container.innerHTML += cardHTML;  
   
}


export { criar_card};

