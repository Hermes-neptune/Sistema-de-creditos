<?php

function readDatabase() {
    $dbFile = __DIR__ . '/database.json';
    
    if (!file_exists($dbFile)) {
        $initialData = [
            'alunos' => [
                '12345' => ['nome' => 'João Silva', 'creditos' => 10],
                '23456' => ['nome' => 'Maria Oliveira', 'creditos' => 5],
                '34567' => ['nome' => 'Pedro Santos', 'creditos' => 8],
                '45678' => ['nome' => 'Ana Souza', 'creditos' => 3],
                '56789' => ['nome' => 'Carlos Ferreira', 'creditos' => 15]
            ]
        ];
        file_put_contents($dbFile, json_encode($initialData, JSON_PRETTY_PRINT));
    }
    
    $jsonData = file_get_contents($dbFile);
    return json_decode($jsonData, true);
}

function saveDatabase($data) {
    $dbFile = __DIR__ . '/database.json';
    file_put_contents($dbFile, json_encode($data, JSON_PRETTY_PRINT));
}

function getAluno($rm) {
    $database = readDatabase();
    
    if (isset($database['alunos'][$rm])) {
        return $database['alunos'][$rm];
    }
    
    return null;
}

function debitarCredito($rm) {
    $database = readDatabase();
    
    if (isset($database['alunos'][$rm])) {
        if ($database['alunos'][$rm]['creditos'] > 0) {
            $database['alunos'][$rm]['creditos']--;
            saveDatabase($database);
            return true;
        }
    }
    
    return false;
}
?>
