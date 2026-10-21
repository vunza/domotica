FROM php:8.2-apache

# Install mysqli extension for database connectivity
RUN docker-php-ext-install mysqli pdo pdo_mysql

# Enable Apache rewrite module
RUN a2enmod rewrite

# Copy project files into container's web root
COPY . /var/www/html/

# Set correct permissions
RUN chown -R www-data:www-data /var/www/html

docker-compose down -- Normal
docker-compose down -v -- Apaga a base de dados
docker-compose up --build -d


# Para redefinir senha...
docker exec -it it_inventory_web php -r '$pdo = new PDO("mysql:host=db;dbname=it_inventory;charset=utf8mb4", "it_user", "it_password"); $hash = password_hash("admin123", PASSWORD_DEFAULT); $stmt = $pdo->prepare("UPDATE users SET password = ? WHERE username = ?"); $stmt->execute([$hash, "admin"]); echo "Password redefinida com sucesso!\n";'