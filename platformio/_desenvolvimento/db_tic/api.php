<?php
session_start();
header('Content-Type: application/json; charset=utf-8');

$host = getenv('DB_HOST') ?: 'db';$dbname = getenv('DB_NAME') ?: 'it_inventory';
$username = getenv('DB_USER') ?: 'it_user';$password = getenv('DB_PASSWORD') ?: 'it_password';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8mb4", $username,$password, [
        PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
        PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC
    ]);
} catch (PDOException $e) {
    echo json_encode(['success' => false, 'error' => 'Database connection failed: ' . $e->getMessage()]);
    exit;
}

$action =$_GET['action'] ?? '';

function getJsonInput() {
    return json_decode(file_get_contents('php://input'), true);
}

try {
   if ($action === 'login') {
        $data = getJsonInput();
        $user = trim($data['username'] ?? '');
        $pass = trim($data['password'] ?? '');

        $stmt = $pdo->prepare("SELECT * FROM users WHERE username = ?");
        $stmt->execute([$user]);
        $dbUser = $stmt->fetch();

        if ($dbUser && password_verify($pass, $dbUser['password'])) {
            $_SESSION['user_id'] = $dbUser['id'];
            $_SESSION['role'] = $dbUser['role'];
            $_SESSION['username'] = $dbUser['username'];
            echo json_encode(['success' => true, 'role' => $dbUser['role'], 'username' => $dbUser['username']]);
            exit;
        } else {
            echo json_encode(['success' => false, 'error' => 'Utilizador ou palavra-passe incorretos.']);
            exit;
        }
    }

    if ($action === 'check_session') {
        if (isset($_SESSION['user_id'])) {
            echo json_encode(['success' => true, 'role' => $_SESSION['role'], 'username' =>$_SESSION['username']]);
        } else {
            echo json_encode(['success' => false]);
        }
        exit;
    }

    if ($action === 'logout') {
        session_destroy();
        echo json_encode(['success' => true]);
        exit;
    }

    if (!isset($_SESSION['user_id'])) {
        echo json_encode(['success' => false, 'error' => 'Não autorizado. Faça login primeiro.']);
        exit;
    }

    $role =$_SESSION['role'];
    $current_user_id =$_SESSION['user_id'];

    // Listar dados gerais + utilizadores (se for admin)
    if ($action === 'list') {
        $equipment =$pdo->query("SELECT * FROM equipment ORDER BY id DESC")->fetchAll();
        $emails =$pdo->query("SELECT * FROM email_accounts ORDER BY id DESC")->fetchAll();
        
        $users = [];
        if ($role === 'admin') {
            $users =$pdo->query("SELECT id, username, role, created_at FROM users ORDER BY id DESC")->fetchAll();
        }

        echo json_encode(['success' => true, 'equipment' => $equipment, 'emails' =>$emails, 'users' => $users, 'role' =>$role]);
        exit;
    }

    // Alterar Senha (Disponível para qualquer utilizador logado)
    if ($action === 'change_password') {$data = getJsonInput();
        $old_pass =$data['old_password'] ?? '';
        $new_pass =$data['new_password'] ?? '';

        
        if (empty($old_pass) || empty($new_pass)) {
            echo json_encode(['success' => false, 'error' => 'Preencha todos os campos de senha.']);
            exit;
        }

        $stmt =$pdo->prepare("SELECT password FROM users WHERE id = ?");
        $stmt->execute([$current_user_id]);
        $userData =$stmt->fetch();

        if (!password_verify($old_pass,$userData['password'])) {
            echo json_encode(['success' => false, 'error' => 'A senha atual está incorreta.']);
            exit;
        }

        $hashed_new_pass = password_hash($new_pass, PASSWORD_DEFAULT);
        $update =$pdo->prepare("UPDATE users SET password = ? WHERE id = ?");
        $update->execute([$hashed_new_pass,$current_user_id]);

        echo json_encode(['success' => true]);
        exit;
    }


   // Ações exclusivas de Admin (Criar e Eliminar Utilizadores)
    if ($action === 'save_user' || $action === 'delete_user') {
        if ($role !== 'admin') {
            echo json_encode(['success' => false, 'error' => 'Acesso negado: Apenas administradores.']);
            exit;
        }
    }

    if ($action === 'save_user') {$data = getJsonInput();
        $new_user = trim($data['username'] ?? '');
        $new_pass = trim($data['password'] ?? '');
        $new_role =$data['role'] ?? 'normal';

        if (empty($new_user) || empty($new_pass)) {
            echo json_encode(['success' => false, 'error' => 'Nome de utilizador e senha são obrigatórios.']);
            exit;
        }

        $hashed = password_hash($new_pass, PASSWORD_DEFAULT);
        try {
            $stmt =$pdo->prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
            $stmt->execute([$new_user, $hashed,$new_role]);
            echo json_encode(['success' => true]);
        } catch (PDOException $e) {
            echo json_encode(['success' => false, 'error' => 'O nome de utilizador já existe.']);
        }
        exit;
    }

    if ($action === 'delete_user') {$data = getJsonInput();
        $id =$data['id'] ?? null;

        if ($id ==$current_user_id) {
            echo json_encode(['success' => false, 'error' => 'Não pode eliminar a sua própria conta de administrador ativa.']);
            exit;
        }

        if ($id) {
            $stmt =$pdo->prepare("DELETE FROM users WHERE id = ?");
            $stmt->execute([$id]);
            echo json_encode(['success' => true]);
            exit;
        }
        echo json_encode(['success' => false, 'error' => 'ID inválido.']);
        exit;
    }

    // Restantes ações de equipamentos e emails (mantidas iguais para admins)...
    if ($action === 'save_equipment' || $action === 'delete_equipment' || $action === 'save_email' || $action === 'delete_email') {
        if ($role !== 'admin') {
            echo json_encode(['success' => false, 'error' => 'Acesso negado.']);
            exit;
        }
    }

    if ($action === 'save_equipment') {$data = getJsonInput();
        $id =$data['id'] ?? null;
        $name = $data['name'] ?? '';
        $type = $data['type'] ?? 'Computador';
        $serial_number = $data['serial_number'] ?? '';
        $location = $data['location'] ?? '';
        $status = $data['status'] ?? 'Ativo';
        $isNetworkDevice = ($type === 'Computador' || $type === 'Telefone VoIP' || $type === 'Switch');
        $mac = $isNetworkDevice ? ($data['mac'] ?? '') : null;
        $ip = $isNetworkDevice ? ($data['ip'] ?? '') : null;
        
        $isComputerOrVoip = ($type === 'Computador' || $type === 'Telefone VoIP');$switch = $isComputerOrVoip ? ($data['switch'] ?? '') : null;
        $port = $isComputerOrVoip ? ($data['port'] ?? '') : null;

        if (empty($name)) {
            echo json_encode(['success' => false, 'error' => 'O nome é obrigatório.']);
            exit;
        }

        if ($id) {
            $stmt =$pdo->prepare("UPDATE equipment SET name = ?, type = ?, mac = ?, ip = ?, serial_number = ?, location = ?, status = ?, `switch` = ?, port = ? WHERE id = ?");
            $stmt->execute([$name,$type, $mac,$ip, $serial_number,$location, $status,$switch, $port,$id]);
        } else {
            $stmt =$pdo->prepare("INSERT INTO equipment (name, type, mac, ip, serial_number, location, status, `switch`, port) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
            $stmt->execute([$name, $type,$mac, $ip,$serial_number, $location,$status, $switch,$port]);
        }
        echo json_encode(['success' => true]);
        exit;
    }

    if ($action === 'delete_equipment') {$data = getJsonInput();
        $id =$data['id'] ?? null;
        if ($id) {
            $stmt =$pdo->prepare("DELETE FROM equipment WHERE id = ?");
            $stmt->execute([$id]);
            echo json_encode(['success' => true]);
            exit;
        }
    }

    if ($action === 'save_email') {$data = getJsonInput();
        $id =$data['id'] ?? null;
        $account =$data['account'] ?? '';
        $owner =$data['owner'] ?? '';
        $status =$data['status'] ?? 'Ativo';

        if ($id) {
            $stmt =$pdo->prepare("UPDATE email_accounts SET account = ?, owner = ?, status = ? WHERE id = ?");
            $stmt->execute([$account,$owner, $status,$id]);
        } else {
            $stmt =$pdo->prepare("INSERT INTO email_accounts (account, owner, status) VALUES (?, ?, ?)");
            $stmt->execute([$account, $owner,$status]);
        }
        echo json_encode(['success' => true]);
        exit;
    }

    if ($action === 'delete_email') {$data = getJsonInput();
        $id =$data['id'] ?? null;
        if ($id) {
            $stmt =$pdo->prepare("DELETE FROM email_accounts WHERE id = ?");
            $stmt->execute([$id]);
            echo json_encode(['success' => true]);
            exit;
        }
    }

    echo json_encode(['success' => false, 'error' => 'Ação desconhecida.']);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>