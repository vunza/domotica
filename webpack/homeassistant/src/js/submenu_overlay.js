import {criar_card} from './cria_cards.js';
//import {getZigbeeDevices} from './get_devs_entities_data.js';
import {api, ip_e_porta, getToken, enablePermitJoin, mostrarLogs, getEntitiesDataWithApi} from './vars_funcs_globais.js';

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
    async configureDevices() {

        /**
         * Limpa o container dos cards dos dispositivos.
         */
        let container = document.getElementById('card_wrapper');
        container.innerHTML = ''; 

        const api_token = '/local/json_files/token_api.json';
        const token = await getToken(api_token);

        // // Busca dispositivos Zigbee e cria cards dinamicamente.    
        // getZigbeeDevices(api, token).then(data => {
        //     data.forEach(device => {           

        //         criar_card(device.id, {
        //             nome: device.nome,
        //             historico: device.historico,
        //             tipo: device.tipo,
        //             status: device.status
        //         }); 

        //     });
        // }).catch(error => {
        //     console.error('Erro ao obter a lista de dispositivos:', error);
        //     return [];
        // });
    
    }

    /**
     * Funcao para adicionar dispoziivos zigbee
     */
    async addDevice() {

        let container_entities = document.getElementById('card_wrapper');
        let container_permit_join = document.getElementById('div_container_permit_join');
        container_entities.innerHTML = '';
        container_permit_join.style.display = 'block';
        const logsContainerEl = document.getElementById('logs-container');
        mostrarLogs(logsContainerEl, 'Prima o Botão para procurar.', 'info');

        // Evento click para o botao que activa permit_join
        document.getElementById('connect_btn').addEventListener('click', async ()=>{
            const api_token = '/local/json_files/token_api.json';
            const topico_permit_join = "zigbee2mqtt/bridge/request/permit_join"
            const api_mqtt_publish = '/api/services/mqtt/publish';
            const tempo_permit_join = 6;

            // Obter Token
            const TOKEN = await getToken(api_token);
           
            // Cria lista de dispositivos existentes.  
            // let old_devs_list = [];  
            // getZigbeeDevices(api, TOKEN).then(data => {
            //     data.forEach(device => {           
            //         old_devs_list.push(device.id);
            //     });
            // }).catch(error => {
            //     console.error('Erro ao obter a lista de dispositivos:', error);
            //     return [];
            // });

            
            
            // Inicializa processo de pareamento, apos concluido obtem nova lista de dispositivos e a compara com "old_devs_list"
            enablePermitJoin(ip_e_porta, api_mqtt_publish, TOKEN, topico_permit_join, tempo_permit_join).then((dados) => {
        
                // // Obtem nova lista de dispositivos
                // getZigbeeDevices(api, TOKEN).then(data => {                     
                //     data.forEach(device => {    
                        
                //         // Verifica novo pareamento                    
                //         const existe = old_devs_list.find( d => d === device.id);             

                //         if (!existe) {                                                                   
                //             mostrarLogs(logsContainerEl, device.id, 'info');                        
                //         }
                //     });
                // }).catch(error => {
                //     console.error('Erro ao obter a lista de dispositivos:', error);
                //     return [];
                // });            
            
            });
        });
    }


     /**
     * Funcao para remover dispoziivos zigbee
     */        
    async removeDevice() {
        console.log('🗑️ Abrindo modal para remover dispositivo...');
        // Sua lógica para remover dispositivo
        //alert('Remover Dispositivo - Implemente esta funcionalidade');

        const api_token = '/local/json_files/token_api.json';
        // Obter Token
        const TOKEN = await getToken(api_token);
        getEntitiesDataWithApi(TOKEN, api).then(devs => console.log(devs));
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