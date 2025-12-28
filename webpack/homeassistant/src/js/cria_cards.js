
// Inicializar a interface
function initDashboard() {
    const container = document.getElementById('cards-container');
    container.innerHTML = '';
}



// Criar elemento de card
function createCardElement(card) {
    const cardElement = document.createElement('div');
    cardElement.className = 'card';
    cardElement.dataset.id = card.id;
    
    // Mapa de SVGs para cada tipo 
    const iconMap = {
        thermometer: `<svg fill="lightgray" viewBox="0 0 100 100" enable-background="new 0 0 100 100" id="Layer_1" version="1.1" xml:space="preserve" stroke="#000000"><g id="SVGRepo_bgCarrier" stroke-width="0"></g><g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g><g id="SVGRepo_iconCarrier"> <g> <path d="M50,82.3c7.6,0,13.8-6.2,13.8-13.8c0-4.4-2-8.4-5.5-11V26c0-2.2-0.9-4.3-2.4-5.9c-1.6-1.6-3.7-2.4-5.9-2.4 c-4.6,0-8.3,3.7-8.3,8.3v31.5c-3.4,2.6-5.5,6.6-5.5,11C36.2,76.1,42.4,82.3,50,82.3z M44.8,60.3l0.9-0.6V48.2H50v-4h-4.3v-1.8H50 v-4h-4.3v-1.8H50v-4h-4.3v-1.8H50v-4h-4.3V26c0-2.4,1.9-4.3,4.3-4.3c1.1,0,2.2,0.4,3,1.3c0.8,0.8,1.3,1.9,1.3,3v33.6l0.9,0.6 c2.8,1.8,4.5,4.9,4.5,8.2c0,5.4-4.4,9.8-9.8,9.8s-9.8-4.4-9.8-9.8C40.2,65.1,41.9,62.1,44.8,60.3z"></path> <path d="M50,76.6c4.5,0,8.1-3.6,8.1-8.1c0-3.8-2.6-6.9-6.1-7.8v-8.1h-4v8.1c-3.5,0.9-6.1,4.1-6.1,7.8C41.9,73,45.5,76.6,50,76.6z M50,64.4c2.3,0,4.1,1.8,4.1,4.1s-1.8,4.1-4.1,4.1s-4.1-1.8-4.1-4.1S47.7,64.4,50,64.4z"></path> </g> </g></svg>`,
        energy: `<svg fill="lightgray" viewBox="0 0 24 24"><path d="M13,9h6L8,22l3-10H5L10,2h7Z"/></svg>`,
        security: `<svg fill="lightgray" viewBox="0 0 24 24"><path d="M12 1L3 5V11C3 16.55 6.84 21.74 12 23C17.16 21.74 21 16.55 21 11V5L12 1ZM12 11H19C18.86 14.73 16.64 17.96 12 19V11Z"/></svg>`,
        camera: `<svg viewBox="0 0 24 24" fill="lightgray" ><g id="SVGRepo_bgCarrier" stroke-width="0"></g><g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g><g id="SVGRepo_iconCarrier"> <path d="M16 10L18.5768 8.45392C19.3699 7.97803 19.7665 7.74009 20.0928 7.77051C20.3773 7.79703 20.6369 7.944 20.806 8.17433C21 8.43848 21 8.90095 21 9.8259V14.1741C21 15.099 21 15.5615 20.806 15.8257C20.6369 16.056 20.3773 16.203 20.0928 16.2295C19.7665 16.2599 19.3699 16.022 18.5768 15.5461L16 14M6.2 18H12.8C13.9201 18 14.4802 18 14.908 17.782C15.2843 17.5903 15.5903 17.2843 15.782 16.908C16 16.4802 16 15.9201 16 14.8V9.2C16 8.0799 16 7.51984 15.782 7.09202C15.5903 6.71569 15.2843 6.40973 14.908 6.21799C14.4802 6 13.9201 6 12.8 6H6.2C5.0799 6 4.51984 6 4.09202 6.21799C3.71569 6.40973 3.40973 6.71569 3.21799 7.09202C3 7.51984 3 8.07989 3 9.2V14.8C3 15.9201 3 16.4802 3.21799 16.908C3.40973 17.2843 3.71569 17.5903 4.09202 17.782C4.51984 18 5.07989 18 6.2 18Z" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"></path> </g></svg>`,
        climate: `<svg viewBox="0 0 45 45" fill="lightgray" ><g id="SVGRepo_bgCarrier" stroke-width="0"></g><g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g><g id="SVGRepo_iconCarrier"> <path d="M23.0261 7.548V11.578L27.0521 9.253L28.0521 10.986L23.0261 13.887V20.815L29.0261 17.351V11.548H31.0261V16.196L34.5171 14.182L35.5171 15.914L32.0261 17.929L36.0521 20.253L35.0521 21.986L30.0261 19.083L24.0261 22.547L30.0271 26.012L35.0521 23.11L36.0521 24.842L32.0261 27.166L35.5171 29.182L34.5171 30.914L31.0261 28.899V33.548H29.0261V27.744L23.0261 24.279V31.208L28.0521 34.11L27.0521 35.842L23.0261 33.517V37.548H21.0261V33.517L17.0001 35.842L16.0001 34.11L21.0261 31.208V24.279L15.0261 27.743V33.548H13.0261V28.898L9.53606 30.914L8.53606 29.182L12.0251 27.166L8.00006 24.842L9.00006 23.11L14.0251 26.011L20.0251 22.547L14.0261 19.083L9.00006 21.986L8.00006 20.253L12.0261 17.929L8.53606 15.914L9.53606 14.182L13.0261 16.196V11.548H15.0261V17.351L21.0261 20.815V13.887L16.0001 10.986L17.0001 9.253L21.0261 11.578V7.548H23.0261Z" fill="#3C3C3C"></path> </g></svg>`,
        picture: `<svg fill="lightgray" viewBox="0 0 24 24" transform="" id="injected-svg"><path d="M21 14V5c0-1.1-.9-2-2-2H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h9v-2H5v-1.59l3-3 1.29 1.29c.39.39 1.02.39 1.41 0l5.29-5.29 3 3V14h2Zm-4.29-5.71a.996.996 0 0 0-1.41 0l-5.29 5.29-1.29-1.29a.996.996 0 0 0-1.41 0l-2.29 2.29V5h14v5.59L16.73 8.3Z"/><path d="M8.5 7a1.5 1.5 0 1 0 0 3 1.5 1.5 0 1 0 0-3M21 16h-2v3h-3v2h3v3h2v-3h3v-2h-3z"/></svg>`,
        switch: `<svg fill="lightgray" viewBox="0 0 24 24" transform="" id="injected-svg"><path d="M16 9a3 3 0 1 0 0 6 3 3 0 1 0 0-6"/><path d="M16 6H8c-3.31 0-6 2.69-6 6s2.69 6 6 6h8c3.31 0 6-2.69 6-6s-2.69-6-6-6m0 10H8c-2.21 0-4-1.79-4-4s1.79-4 4-4h8c2.21 0 4 1.79 4 4s-1.79 4-4 4"/></svg>`,
        plug: `<svg fill="lightgray" version="1.1" id="Layer_1"  viewBox="0 0 512.001 512.001" xml:space="preserve"><g id="SVGRepo_bgCarrier" stroke-width="0"></g><g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g><g id="SVGRepo_iconCarrier"> <g> <g> <path d="M511.68,370.753c-1.216-4.928-10.581-29.419-74.347-29.419c-52.736,0-80.128,43.691-104.32,82.219 c-21.632,34.517-42.091,67.115-77.013,67.115c-66.816,0-89.429-40.149-94.656-64h19.989c17.643,0,32-14.357,32-32v-32 c0-30.059,14.357-58.752,38.4-76.8c29.397-22.037,46.933-57.131,46.933-93.867v-53.333c0-17.643-14.357-32-32-32h-32v-96 c0-5.888-4.779-10.667-10.667-10.667s-10.667,4.779-10.667,10.667v96h-128v-96c0-5.888-4.779-10.667-10.667-10.667 S64,4.779,64,10.667v96H32c-17.643,0-32,14.357-32,32v53.333c0,36.736,17.557,71.829,46.933,93.867 c24.043,18.048,38.4,46.741,38.4,76.8v32c0,17.643,14.357,32,32,32h22.251c5.44,36.736,38.72,85.333,116.416,85.333 c46.72,0,71.317-39.189,95.104-77.099c23.296-37.141,45.312-72.235,86.229-72.235c45.76,0,53.483,13.291,53.653,13.291v-0.021 c1.429,5.696,7.232,9.195,12.928,7.744C509.632,382.251,513.109,376.47,511.68,370.753z M117.333,405.334 c-5.888,0-10.667-4.779-10.667-10.667v-32c0-36.736-17.557-71.829-46.933-93.867c-24.043-18.048-38.4-46.741-38.4-76.8v-53.333 c0-5.888,4.779-10.667,10.667-10.667h234.667c5.888,0,10.667,4.779,10.667,10.667v53.333c0,30.059-14.357,58.752-38.4,76.8 C209.536,290.838,192,325.931,192,362.667v32c0,5.888-4.779,10.667-10.667,10.667H117.333z"></path> </g> </g> <g> <g> <path d="M149.333,192.001c-23.531,0-42.667,19.136-42.667,42.667c0,23.531,19.136,42.667,42.667,42.667 c23.531,0,42.667-19.136,42.667-42.667C192,211.137,172.864,192.001,149.333,192.001z M149.333,256.001 c-11.755,0-21.333-9.579-21.333-21.333c0-11.755,9.579-21.333,21.333-21.333c11.755,0,21.333,9.579,21.333,21.333 C170.667,246.422,161.088,256.001,149.333,256.001z"></path> </g> </g> </g></svg>`,
        hub_zb: `<svg viewBox="0 0 24 24" fill="lightgray" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="2"></circle><path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path></svg>`,
        light: `<svg viewBox="-80 0 512 512" fill="lightgray"><path d="M96.06 454.35c.01 6.29 1.87 12.45 5.36 17.69l17.09 25.69a31.99 31.99 0 0 0 26.64 14.28h61.71a31.99 31.99 0 0 0 26.64-14.28l17.09-25.69a31.989 31.989 0 0 0 5.36-17.69l.04-38.35H96.01l.05 38.35zM0 176c0 44.37 16.45 84.85 43.56 115.78 16.52 18.85 42.36 58.23 52.21 91.45.04.26.07.52.11.78h160.24c.04-.26.07-.51.11-.78 9.85-33.22 35.69-72.6 52.21-91.45C335.55 260.85 352 220.37 352 176 352 78.61 272.91-.3 175.45 0 73.44.31 0 82.97 0 176zm176-80c-44.11 0-80 35.89-80 80 0 8.84-7.16 16-16 16s-16-7.16-16-16c0-61.76 50.24-112 112-112 8.84 0 16 7.16 16 16s-7.16 16-16 16z"/></svg>`,
        pump: `<svg viewBox="0 0 1024 1024" fill="lightgray"><path d="M544.7 703.8v-320c-35.3 0-64 26-64 58.2v203.7c0 32.1 28.6 58.1 64 58.1zM576.7 383.8h288v44.5h-288zM576.7 556.3h288v64h-288zM576.7 460.3h288v64h-288zM960.7 645.6V442c0-32.1-28.7-58.2-64-58.2v320c35.4 0 64-26 64-58.2zM640.7 351.8h160c17.7 0 32-14.3 32-32v-64h-224v64c0 17.7 14.3 32 32 32zM288.7 223.8h64v-64h-64v-64h-64v64h-64v64h64z"/><path d="M832.7 799.8v-64h32v-83.5h-288v83.5h32v64h-320v-160h64v-64h32v32h64v-128h-64v32h-32v-64h-64v-128h64v-64h-192v64h64v128h-32v192h32v96h-128c-35.3 0-64 28.7-64 64v128h960.1v-128H832.7z m-64 0h-96v-64h96v64z"/><path d="M160.7 639.8v-192c-35.3 0-64 28.7-64 64h-64v64h64c0 35.4 28.6 64 64 64z"/></svg>`,
        clock: `<svg viewBox="0 0 24 24" fill="lightgray" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"></circle><polyline points="12 6 12 12 16 14"></polyline></svg>`,
        config: `<svg viewBox="0 0 24 24" fill="lightgray" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>`
        //pencil: `<svg fill="lightgray" viewBox="0 0 24 24" transform="" id="injected-svg"><path d="m19.41,3c-.78-.78-2.05-.78-2.83,0L4.29,15.29c-.13.13-.22.29-.26.46l-1,4c-.08.34.01.7.26.95.19.19.45.29.71.29.08,0,.16,0,.24-.03l4-1c.18-.04.34-.13.46-.26l12.29-12.29c.78-.78.78-2.05,0-2.83l-1.59-1.59Zm-11.93,15.1l-2.11.53.53-2.11L15,7.41l1.59,1.59-9.1,9.1Zm10.51-10.51l-1.59-1.59,1.59-1.59,1.59,1.58-1.59,1.59Z"/></svg>`

    };
    //console.log('MAC Address do card selecionado:', card.mac_address);
    const iconHTML = iconMap[card.type] || iconMap.picture;
    cardElement.id = card.id;
    cardElement.tipo = card.type;
    cardElement.innerHTML =         
        `<div class="card-header" data-device-mac-address="${card.mac_address || 'null'}">
            <div class="card-title">
                <span class="card-icon" id="${card.id}.img" onclick="click_on_image_card(this.id);">
                    ${iconHTML}
                </span>
                <span class="card-title-content">${card.title}</span>
            </div>
            <button class="menu-trigger" data-id="${card.id}">
                <svg width="24" height="24" fill="currentColor" viewBox="0 0 24 24">
                    <path d="M12 8c1.1 0 2-.9 2-2s-.9-2-2-2-2 .9-2 2 .9 2 2 2zm0 2c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm0 6c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2z"/>
                </svg>
            </button>
        </div>
        <div class="card-content">
            ${card.content}
        </div>`;
 
    
    
    // Adiciona a Card criada, se não existe no DOM
    if( !document.getElementById(`${card.id}.img`) ){
        document.getElementById('cards-container').appendChild(cardElement);
    }   
    
}


// Mostrar indicador de ação
function showActionIndicator(message) {
    const indicator = document.getElementById('action-indicator');
    const actionText = document.getElementById('action-text');
    
    actionText.textContent = message;
    indicator.classList.add('show');
    
    setTimeout(() => {
        indicator.classList.remove('show');
    }, 3000);
}


// After HTML is parsed and the DOM is ready (does not wait for images/stylesheets)
document.addEventListener('DOMContentLoaded', (event) => {
    initDashboard();
});


// Exportar 
export { createCardElement };