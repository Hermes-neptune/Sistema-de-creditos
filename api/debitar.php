<?php
header('Content-Type: application/json');

require_once 'db.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode(['erro' => 'Método não permitido']);
    exit;
}

if (!isset($_POST['rm']) || empty($_POST['rm'])) {
    http_response_code(400);
    echo json_encode(['erro' => 'RM não fornecido']);
    exit;
}

$rm = $_POST['rm'];

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

if ($aluno['creditos'] <= 0) {
    http_response_code(400);
    echo json_encode(['erro' => 'Aluno não possui créditos suficientes']);
    exit;
}

$resultado = debitarCredito($rm);

if ($resultado) {
    $alunoAtualizado = getAluno($rm);
    
    echo json_encode([
        'sucesso' => true,
        'rm' => $rm,
        'nome' => $alunoAtualizado['nome'],
        'creditos' => $alunoAtualizado['creditos'],
        'mensagem' => 'Crédito debitado com sucesso'
    ]);
} else {
    http_response_code(500);
    echo json_encode(['erro' => 'Erro ao debitar crédito']);
}
?>
