// Seleção de view
document.querySelectorAll('.view-option').forEach(option => {
    option.addEventListener('click', function() {
        document.querySelectorAll('.view-option').forEach(opt => {
            opt.classList.remove('selected');
        });
        this.classList.add('selected');
    });
});