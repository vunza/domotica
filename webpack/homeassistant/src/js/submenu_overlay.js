import {criar_card} from './cria_cards.js';
import {getZigbeeDevices} from './get_devs_entities_data.js';
import {api} from './vars_globais.js';

/**
 * Gerencia o submenu overlay
 * Permite abrir e fechar o submenu, além de lidar com ações dos itens do submenu.
 * 
 * @class SubmenuManager
 * @author Zambo
 * @date 2024-12-15 
 */
class SubmenuManager {
    constructor() {
        this.submenuOverlay = document.getElementById('submenuOverlay');
        this.moreMenuButton = document.getElementById('more-menu');
        this.closeButton = document.getElementById('closeSubmenu');
        this.submenuItems = document.querySelectorAll('.submenu-item');
        
        this.init();
    }
    
    init() {
        // Abrir submenu
        this.moreMenuButton.addEventListener('click', (e) => {
            e.preventDefault();
            this.openSubmenu();
        });
        
        // Fechar submenu
        this.closeButton.addEventListener('click', () => {
            this.closeSubmenu();
        });
        
        // Fechar ao clicar no overlay (fora do menu)
        this.submenuOverlay.addEventListener('click', (e) => {
            if (e.target === this.submenuOverlay) {
                this.closeSubmenu();
            }
        });
        
        // Fechar com tecla ESC
        document.addEventListener('keydown', (e) => {
            if (e.key === 'Escape' && this.submenuOverlay.style.display !== 'none') {
                this.closeSubmenu();
            }
        });
        
        // Ações dos itens do submenu
        this.submenuItems.forEach(item => {
            item.addEventListener('click', () => {
                const action = item.getAttribute('data-action');
                this.handleSubmenuAction(action);
            });
        });
    }


    
    openSubmenu() {
        this.submenuOverlay.style.display = 'flex';
        document.body.style.overflow = 'hidden'; // Previne scroll do body
        
        // Foco no botão de fechar para acessibilidade
        setTimeout(() => {
            this.closeButton.focus();
        }, 100);
    }
    
    closeSubmenu() {
        this.submenuOverlay.style.display = 'none';
        document.body.style.overflow = ''; // Restaura scroll
    }
    
    handleSubmenuAction(action) {
        console.log('Ação do submenu:', action);
        
        // Fecha o submenu primeiro
        this.closeSubmenu();
        
        // Executa a ação baseada no data-action
        switch(action) {
            case 'config-devices':
                this.configureDevices();
                break;

            case 'add-device':
                this.addDevice();
                break;
                
            case 'remove-device':
                this.removeDevice();
                break;
                
            case 'settings':
                this.openSettings();
                break;
                
            case 'backup':
                this.createBackup();
                break;
                
            case 'about':
                this.showAbout();
                break;
                
            default:
                console.warn('Ação não reconhecida:', action);
        }
    }
    
    /**
     * Visualiza os dispositivos em modo de configuração, através do submenu overlay
     */
    configureDevices() {

        /**
         * Limpa o container dos cards dos dispositivos.
         */
        let container = document.getElementById('card_wrapper');
        container.innerHTML = ''; 

        // Busca dispositivos Zigbee e cria cards dinamicamente.    
        getZigbeeDevices(api).then(data => {
            data.forEach(device => {           

                criar_card(device.id, {
                    nome: device.nome,
                    historico: device.historico,
                    tipo: device.tipo,
                    status: device.status
                }); 

            });
        }).catch(error => {
            console.error('Erro ao obter a lista de dispositivos:', error);
            return [];
        });
    
    }

    /**
     * Funcao para adicionar dispoziivos zigbee
     */
    async addDevice() {
        console.log('🔄 Abrindo modal para adicionar dispositivo...');
        // Sua lógica para adicionar dispositivo
        alert('Adicionar Dispositivo - Implemente esta funcionalidade');

        try {
            // Buscar o token do arquivo JSON local
            const response = await fetch('/local/json_files/token_api.json');
            const { token } = await response.json();       
            
            
        } catch (error) {
            console.error('Erro:', error);
        }       
    }


            
    removeDevice() {
        console.log('🗑️ Abrindo modal para remover dispositivo...');
        // Sua lógica para remover dispositivo
        alert('Remover Dispositivo - Implemente esta funcionalidade');
    }
    
    openSettings() {
        console.log('⚙️ Abrindo configurações...');
        // Sua lógica para abrir configurações
        alert('Configurações - Implemente esta funcionalidade');
    }
    
    createBackup() {
        console.log('💾 Criando backup...');
        // Sua lógica para criar backup
        alert('Fazer Backup - Implemente esta funcionalidade');
    }
    
    showAbout() {
        console.log('ℹ️ Mostrando informações...');
        // Sua lógica para mostrar sobre
        alert('Sobre - Implemente esta funcionalidade');
    }
}

// Integração com a navegação existente
class SubmenuOverlay {
    constructor() {
        this.navItems = document.querySelectorAll('.nav-item');
        this.pages = document.querySelectorAll('.page');
        this.currentPage = 'home-page';
        this.submenuManager = new SubmenuManager();
        
        this.init();
    }
    
    init() {
        this.navItems.forEach(item => {
            item.addEventListener('click', (e) => {
                e.preventDefault();
                const pageId = item.getAttribute('data-page');
                
                // Verifica se é o menu "Mais" (submenu)
                if (pageId === 'menu-page') {
                    // O submenuManager já cuida disso
                    return;
                }
                // Verifica se é refresh
                else if (pageId === 'refresh-page') {
                    this.handleRefresh();
                } else {
                    this.navigateTo(pageId);
                }
            });
        });
        
        this.navigateTo('home-page');
    }

    navigateTo(pageId) {
        if (this.currentPage === pageId) return;
        
        // Atualiza a página atual
        this.currentPage = pageId;
        
        // Esconde todas as páginas com transição
        this.pages.forEach(page => {
            page.style.display = 'none';
        });
        
        // Mostra a página selecionada
        const targetPage = document.getElementById(pageId);
        if (targetPage) {
            targetPage.style.display = 'block';
        }
        
        // Atualiza o estado ativo do menu
        this.updateActiveState(pageId);
        
        // Adiciona ao histórico do navegador
        history.pushState({ page: pageId }, '', `#${pageId}`);
        
        // Dispara evento customizado
        this.dispatchNavigationEvent(pageId);
    }
    
    // ... resto do código existente da navegação
}

// Inicialização
// document.addEventListener('DOMContentLoaded', () => {
//     const submenuOverlay = new SubmenuOverlay();
// });

export { SubmenuOverlay };