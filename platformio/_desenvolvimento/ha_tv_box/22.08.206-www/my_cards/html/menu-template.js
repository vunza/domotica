export const menuTemplate = `
  <div class="menu-item" data-action="upload_sketch">
    <svg id="svg_upload_sketch" viewBox="0 0 64 64" fill="none" stroke="currentColor" stroke-width="2"> 
      <path d="M20 44h24a8 8 0 0 0 0-16 12 12 0 0 0-24 2 6 6 0 0 0 0 14z"/> 
      <line x1="32" y1="36" x2="32" y2="20"/>
      <polyline points="26,26 32,20 38,26"/>
    </svg>
    <span id="span_programar">Programar</span>
  </div>
  <div class="menu-item" data-action="rename">
    <svg viewBox="0 0 24 24" fill="currentColor">
      <path d="M3 17.25V21h3.75L17.81 9.94l-3.75-3.75L3 17.25zM20.71 7.04c.39-.39.39-1.02 0-1.41l-2.34-2.34c-.39-.39-1.02-.39-1.41 0l-1.83 1.83 3.75 3.75 1.83-1.83z"/>
    </svg>
    <span>Renomear</span>
  </div>
  <div class="menu-item" data-action="timer">
    <svg viewBox="0 0 24 24" fill="currentColor">
      <path d="M15 1H9v2h6V1zm-4 13h2V8h-2v6zm8.03-6.61l1.42-1.42c-.43-.51-.9-.99-1.41-1.41l-1.42 1.42C16.07 4.74 14.12 4 12 4c-4.97 0-9 4.03-9 9s4.02 9 9 9 9-4.03 9-9c0-2.12-.74-4.07-1.97-5.61zM12 20c-3.87 0-7-3.13-7-7s3.13-7 7-7 7 3.13 7 7-3.13 7-7 7z"/>
    </svg>
    <span>Temporizar</span>
  </div>
  <div class="menu-item" data-action="delete">
    <svg viewBox="0 0 24 24" fill="currentColor">
      <path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/>
    </svg>
    <span>Remover</span>
  </div>
  <div class="menu-item" data-action="close">
    <svg viewBox="0 0 24 24" fill="currentColor">
      <path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/>
    </svg>
    <span>Fechar</span>
  </div>
`;