export const contextMenuStyles = `
  .context-menu {
    position: absolute;
    right: 0;
    background-color: #2d3748;
    border-radius: 10px;
    box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
    z-index: 1000;
    width: 240px;
    opacity: 0;
    visibility: hidden;
    transition: opacity 0.2s ease, transform 0.2s ease, visibility 0.2s ease;
    border: 1px solid #4a5568;
    max-height: 350px;
    overflow-y: auto;
  }

  .context-menu.context-menu--down {
    top: 100%;
    transform: translateY(-15px);
  }

  .context-menu.context-menu--up {
    bottom: 100%;
    transform: translateY(15px);
  }

  .context-menu.active {
    opacity: 1;
    visibility: visible;
    transform: translateY(0);
  }

  .menu-item {
    padding: 12px 16px;
    cursor: pointer;
    display: flex;
    align-items: center;
    gap: 14px;
    transition: background-color 0.2s ease;
    font-size: 1rem;
    border-bottom: 1px solid #4a5568;
    color: #e2e8f0;
  }

  .menu-item:last-child {
    border-bottom: none;
  }

  .menu-item:hover {
    background-color: #4a5568;
  }

  .menu-item svg {
    width: 30px;
    height: 30px;
    flex-shrink: 0;   
  }

  #svg_upload_sketch {
    width: 40px;
    height: 40px;
  }

  #span_programar {
    margin-left: -10px;
  }

  .menu-item[data-action="upload_sketch"] svg { color: #4299e1; }
  .menu-item[data-action="rename"] svg        { color: #ed8936; }
  .menu-item[data-action="timer"] svg         { color: #9f7aea; }
  .menu-item[data-action="delete"] svg        { color: #f56565; }
  .menu-item[data-action="close"] svg         { color: #a0aec0; }

  .context-menu::-webkit-scrollbar {
    width: 6px;
  }

  .context-menu::-webkit-scrollbar-track {
    background: transparent;
    border-radius: 3px;
  }

  .context-menu::-webkit-scrollbar-thumb {
    background: #4a5568;
    border-radius: 3px;
  }

  .context-menu::-webkit-scrollbar-thumb:hover {
    background: #5a6578;
  }
`;