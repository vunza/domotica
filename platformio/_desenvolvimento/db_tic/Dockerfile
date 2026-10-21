FROM php:8.2-apache

# Instalar extensões necessárias para ligar ao MariaDB/MySQL
RUN docker-php-ext-install mysqli pdo pdo_mysql

# Ativar o módulo Apache mod_rewrite
RUN a2enmod rewrite

# Copiar os códigos fonte da aplicação web para a pasta pública do Apache
COPY index.html /var/www/html/index.html
COPY script.js /var/www/html/script.js
COPY api.php /var/www/html/api.php

# Garantir permissões corretas
RUN chown -R www-data:www-data /var/www/html
