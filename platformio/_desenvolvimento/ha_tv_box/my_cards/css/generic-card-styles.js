// Estilos gerais do card (fora do menu)
export const cardStyles = `
  .card {
    background: var(--card-background-color, white);
    color: var(--primary-text-color);
    border-radius: 12px;
    box-shadow: var(--ha-card-box-shadow, 0 2px 2px 0 rgba(0,0,0,0.14), 0 1px 5px 0 rgba(0,0,0,0.12));
    font-family: var(--primary-font-family, Roboto);
    border: 1px solid var(--primary-color);  
  }
  .card-header {
    display: flex;
    justify-content: space-between;
    align-items: center;   
    padding: 8px 5px 0px 5px;
  }
  .card-title {
    display: flex;
    align-items: center;
    gap: 8px;
  }
  .card-icon {
    display: flex;
    align-items: center;
    transition: color 0.2s ease;
    cursor: pointer;
  }
  .card-title-content {
    font-size: 1.2rem;
    font-weight: 500;
  }
  .menu-container {
    position: relative;
    display: inline-block;
  }
  .menu-trigger {
    background: none;
    border: none;
    padding: 4px;
    cursor: pointer;
    color: var(--secondary-text-color);
    border-radius: 4px;
  }
  .menu-trigger:hover {
    background: var(--divider-color, #e0e0e0);
  }
  .card-content {
    display: flex;
    align-items: center;
    gap: 16px;
    padding: 5px;  
    border-top: 1px solid var(--divider-color, #e0e0e0);  
  }
  .div_img_clock {
    display: flex;
    align-items: center;
    color: var(--secondary-text-color);   
  }
  .div_content {
    flex: 1;
    font-size: 0.9rem;
    color: var(--secondary-text-color);   
  }

  /*ENTUDADES EXTRAS*/
  .extra-entities {
    margin-top: 1px;
    padding-top: 8px;    
  }

  /*.extra-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(140px, 1fr));
    gap: 8px;
  }*/

  .extra-grid {
    display: grid;
    grid-template-columns: 1fr;   /* uma única coluna ocupando toda largura */
    gap: 8px;
    border-top: 1px solid var(--divider-color, #e0e0e0);
  }

  .extra-item {
    display: flex;
    justify-content: space-between;
    align-items: baseline;
    font-size: 0.85rem;
    padding: 4px 8px;
    background: var(--card-background-color, white);
    border-radius: 8px;
  }

  .extra-name {
    color: var(--secondary-text-color, #5f6368);
    font-weight: 500;
  }

  .extra-value {
    font-weight: bold;
    color: var(--primary-text-color, #202124);
  }
`;