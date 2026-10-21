CREATE DATABASE IF NOT EXISTS it_inventory CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE it_inventory;

-- Tabela de Utilizadores
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(100) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    role VARCHAR(50) NOT NULL DEFAULT 'normal', -- 'admin' ou 'normal'
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Inserir utilizadores padrão (Admin: admin123 / Normal: user123)
-- As senhas abaixo já estão hasheadas com password_hash para segurança
INSERT IGNORE INTO users (id, username, password, role) VALUES
(1, 'admin', '$2y$10$TKh8H1.PfQx37YgCzwiKb.KjNyWgaHb9cbcoQgdIVFlYg7B77UdFm', 'admin'),
(2, 'user', '$2y$10$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2.uheWG/igi', 'normal');

-- Restante das tabelas existentes (equipment, email_accounts)...
CREATE TABLE IF NOT EXISTS equipment (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    type VARCHAR(50) NOT NULL,
    mac VARCHAR(50) DEFAULT NULL,
    ip VARCHAR(50) DEFAULT NULL,
    serial_number VARCHAR(100) DEFAULT NULL,
    location VARCHAR(100) DEFAULT NULL,
    status VARCHAR(50) NOT NULL DEFAULT 'Ativo',
    `switch` VARCHAR(100) DEFAULT NULL,
    port VARCHAR(50) DEFAULT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS email_accounts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    account VARCHAR(150) NOT NULL,
    owner VARCHAR(150) NOT NULL,
    status VARCHAR(50) NOT NULL DEFAULT 'Ativo',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
