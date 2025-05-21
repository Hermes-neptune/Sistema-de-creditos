<?php
header('Content-Type: application/json');

require_once 'db.php';

if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
    http_response_code(405);
    echo json_encode(['erro' => 'Método não permitido']);
    exit;
}

if (!isset($_GET['rm']) || empty($_GET['rm'])) {
    http_response_code(400);
    echo json_encode(['erro' => 'RM não fornecido']);
    exit;
}

$rm = $_GET['rm'];

if (!preg_match('/^\d{5}$/', $rm)) {
    http_response_code(400);
    echo json_encode(['erro' => 'RM inválido. Deve conter 5 dígitos numéricos']);
    exit;
}

$aluno = getAluno($rm);

if ($aluno === null) {
    http_response_code(404);
    echo json_encode(['erro' => 'Aluno não encontrado']);
    exit;
}

echo json_encode([
    'sucesso' => true,
    'rm' => $rm,
    'nome' => $aluno['nome'],
    'creditos' => $aluno['creditos']
]);
?>
