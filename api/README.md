# Documentação da API de Sistema de Créditos

Esta documentação descreve os endpoints disponíveis na API do Sistema de Créditos para alunos.

## Base URL

Para testes locais, a API estará disponível em:
```
http://localhost/sistema_creditos/api/
```

## Endpoints

### 1. Consultar Aluno

Retorna informações de um aluno pelo seu Registro de Matrícula (RM).

- **URL**: `/consulta.php`
- **Método**: `GET`
- **Parâmetros**:
  - `rm` (obrigatório): Registro de Matrícula do aluno (5 dígitos numéricos)

#### Exemplo de Requisição:
```
GET /sistema_creditos/api/consulta.php?rm=12345
```

#### Resposta de Sucesso:
```json
{
  "sucesso": true,
  "rm": "12345",
  "nome": "João Silva",
  "creditos": 10
}
```

#### Respostas de Erro:
- **400 Bad Request** - RM não fornecido ou inválido
```json
{
  "erro": "RM não fornecido"
}
```
ou
```json
{
  "erro": "RM inválido. Deve conter 5 dígitos numéricos"
}
```

- **404 Not Found** - Aluno não encontrado
```json
{
  "erro": "Aluno não encontrado"
}
```

### 2. Debitar Crédito

Debita um crédito do aluno pelo seu Registro de Matrícula (RM).

- **URL**: `/debitar.php`
- **Método**: `POST`
- **Parâmetros**:
  - `rm` (obrigatório): Registro de Matrícula do aluno (5 dígitos numéricos)

#### Exemplo de Requisição:
```
POST /sistema_creditos/api/debitar.php
Content-Type: application/x-www-form-urlencoded

rm=12345
```

#### Resposta de Sucesso:
```json
{
  "sucesso": true,
  "rm": "12345",
  "nome": "João Silva",
  "creditos": 9,
  "mensagem": "Crédito debitado com sucesso"
}
```

#### Respostas de Erro:
- **400 Bad Request** - RM não fornecido, inválido ou sem créditos suficientes
```json
{
  "erro": "RM não fornecido"
}
```
ou
```json
{
  "erro": "RM inválido. Deve conter 5 dígitos numéricos"
}
```
ou
```json
{
  "erro": "Aluno não possui créditos suficientes"
}
```

- **404 Not Found** - Aluno não encontrado
```json
{
  "erro": "Aluno não encontrado"
}
```

- **500 Internal Server Error** - Erro ao debitar crédito
```json
{
  "erro": "Erro ao debitar crédito"
}
```

## Banco de Dados de Teste

O sistema utiliza um arquivo JSON como banco de dados simples. Os seguintes registros estão disponíveis para teste:

| RM    | Nome            | Créditos |
|-------|-----------------|----------|
| 12345 | João Silva      | 10       |
| 23456 | Maria Oliveira  | 5        |
| 34567 | Pedro Santos    | 8        |
| 45678 | Ana Souza       | 3        |
| 56789 | Carlos Ferreira | 15       |
