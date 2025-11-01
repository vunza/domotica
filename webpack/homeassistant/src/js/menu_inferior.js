
class BottomNavigation {
    constructor() {
        this.navItems = document.querySelectorAll('.nav-item');
        this.pages = document.querySelectorAll('.page');
        this.currentPage = 'home-page';
        
        this.init();
    }
    
    init() {
        // Adiciona event listeners para cada item do menu
        this.navItems.forEach(item => {
            item.addEventListener('click', (e) => {
                e.preventDefault();
                const pageId = item.getAttribute('data-page');
                this.navigateTo(pageId);                
            });
            
            // Suporte para teclado
            item.addEventListener('keydown', (e) => {
                if (e.key === 'Enter' || e.key === ' ') {
                    e.preventDefault();
                    const pageId = item.getAttribute('data-page');
                    this.navigateTo(pageId);
                }
            });
        });
        
        // Navegação inicial
        this.navigateTo('home-page');     
    }

    /**
     * Navega para a página especificada, em funcao do ID correspondente ao Item do SubMneu overlay
     * @param {String} pageId ID da página para navegar
     */
    navigateTo(pageId) {
        if (this.currentPage === pageId) return;
        
        // Atualiza a página atual
        this.currentPage = pageId;
        
        // Esconde todas as páginas com transição
        this.pages.forEach(page => {
            page.style.display = 'none';
        });
        
        // Mostra a página correspondente a opcao submenu overlay
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
    

    /**
     * Atualiza o estado ativo do menu inferior
     * @param {String} activePageId Variavel correspondente ao Item clicado no Menu Inferior
     */
    updateActiveState(activePageId) {        
        this.navItems.forEach(item => {
            const itemPage = item.getAttribute('data-page');
            
            if (itemPage === activePageId) {
                item.classList.add('active');
                item.setAttribute('aria-current', 'page');
            } else {
                item.classList.remove('active');
                item.removeAttribute('aria-current');
            }
        });
    }
    

    /**
     * Dispara um evento customizado ao clicar em um item do menu inferior
     * @param {String} pageId ID da página navegada
    */
    dispatchNavigationEvent(pageId) {
        const event = new CustomEvent('pageChanged', {
            detail: { pageId, pageTitle: this.getPageTitle(pageId) }
        });
        document.dispatchEvent(event);        
    }
    
    getPageTitle(pageId) {
        const titles = {
            'home-page': 'Página Inicial',
            'search-page': 'Pesquisa',
            'favorites-page': 'Favoritos',
            'profile-page': 'Perfil',
            'menu-page': 'Menu'
        };
        return titles[pageId] || 'Página';
    }
}



/**
 * @type {HTMLElement}  navBarInicial Elemento da barra de navegação inicial
 * @type {HTMLElement}  navBar Elemento da barra de navegação inferior
 */
const navBarInicial = document.querySelector('.navbar_inicial')
const navBar = document.querySelector('.bottom-nav')



/**
 * Impedir o menu de contexto padrão em todo o documento.
 */
window.addEventListener('contextmenu', event => {
    event.preventDefault();
}, false);



/**
 * Evento de clique com o botão direito/long press na barra de navegação inferior inicial.
 * Ao clicar, simula click na opcao "Mais" da barra de navegação inferior e abre o SubMenu Overlay.
 */
navBarInicial.addEventListener('contextmenu', event => {
    event.preventDefault();
    //navBarInicial.style.display = 'none';
	//navBar.style.display = 'flex'; 
    const targetPage = document.getElementById('more-menu');
    targetPage.click(); 
});
        
        

export { BottomNavigation };