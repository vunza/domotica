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
        
        // Suporte a swipe (opcional)
        this.setupSwipeSupport();
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
    
    setupSwipeSupport() {
        let startX = 0;
        let endX = 0;
        
        document.addEventListener('touchstart', (e) => {
            startX = e.changedTouches[0].screenX;
        });
        
        document.addEventListener('touchend', (e) => {
            endX = e.changedTouches[0].screenX;
            this.handleSwipe(startX, endX);
        });
    }
    
    handleSwipe(startX, endX) {
        const swipeThreshold = 50;
        const diff = startX - endX;
        
        if (Math.abs(diff) > swipeThreshold) {
            const pages = ['home-page', 'search-page', 'favorites-page', 'profile-page', 'menu-page'];
            const currentIndex = pages.indexOf(this.currentPage);
            
            if (diff > 0 && currentIndex < pages.length - 1) {
                // Swipe para esquerda - próxima página
                this.navigateTo(pages[currentIndex + 1]);
            } else if (diff < 0 && currentIndex > 0) {
                // Swipe para direita - página anterior
                this.navigateTo(pages[currentIndex - 1]);
            }
        }
    }
    
    // Manipula o botão voltar do navegador
    handleBackButton() {
        window.addEventListener('popstate', (event) => {
            if (event.state && event.state.page) {
                this.navigateTo(event.state.page);
            }
        });
    }
}

// Inicialização quando o DOM estiver carregado
document.addEventListener('DOMContentLoaded', () => {
    // const bottomNav = new BottomNavigation();
    // bottomNav.handleBackButton();
    
    // // Verifica hash da URL inicial
    // const hash = window.location.hash.substring(1);
    // if (hash && document.getElementById(hash)) {
    //     bottomNav.navigateTo(hash);
    // }
    
    // // Log para debug (opcional)
    // document.addEventListener('pageChanged', (e) => {
    //     console.log('Navegou para:', e.detail.pageId);
    // });
});

// Melhoria de performance
//document.addEventListener('touchstart', function() {}, { passive: true });

export { BottomNavigation };