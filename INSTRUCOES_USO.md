# Instruções de Uso do Sistema de Créditos

Este documento fornece instruções detalhadas para a instalação, configuração e uso do Sistema de Créditos para alunos utilizando uma placa Wemos D1, teclado 3x4 e display LCD 16x2.

## Visão Geral do Sistema

O sistema permite:
- Digitar o RM (Registro de Matrícula) de 5 dígitos no teclado
- Consultar informações do aluno (nome e créditos) via API
- Exibir as informações no display LCD
- Debitar créditos ao pressionar "#"
- Encerrar o ciclo atual ao pressionar "*"

## Componentes do Sistema

1. **API PHP**: Fornece endpoints para consulta e débito de créditos
2. **Banco de Dados**: Arquivo JSON com registros de alunos para teste
3. **Firmware Wemos D1**: Código Arduino para o dispositivo físico

## Fluxo de Funcionamento

1. O usuário digita o RM de 5 dígitos no teclado
2. Ao pressionar "#", o sistema consulta a API para obter os dados do aluno
3. O display LCD mostra o nome e a quantidade de créditos do aluno
4. Se o usuário pressionar "#" novamente, um crédito é debitado
5. Se o usuário pressionar "*", o sistema volta à tela inicial

## Requisitos Técnicos

### Hardware
- Placa Wemos D1 (ESP8266)
- Teclado matricial 3x4
- Display LCD 16x2 com módulo I2C
- Cabos de conexão
- Fonte de alimentação para o Wemos D1

### Software
- Servidor web com PHP (para hospedar a API)
- Arduino IDE (para carregar o firmware no Wemos D1)
- Bibliotecas Arduino necessárias (listadas no código)

## Arquivos Incluídos

1. **API PHP**:
   - `api/consulta.php`: Endpoint para consultar informações do aluno
   - `api/debitar.php`: Endpoint para debitar créditos
   - `api/db.php`: Funções de banco de dados
   - `api/README.md`: Documentação da API

2. **Firmware**:
   - `wemos_sistema_creditos.ino`: Código Arduino para o Wemos D1

3. **Documentação**:
   - `GUIA_TESTE_INTEGRACAO.md`: Guia detalhado para teste e integração
   - `INSTRUCOES_USO.md`: Este arquivo com instruções de uso

## Próximos Passos

1. Configure o servidor web e instale os arquivos da API
2. Configure e carregue o firmware no Wemos D1
3. Conecte o hardware conforme as instruções
4. Teste o sistema completo

Para instruções detalhadas de instalação, configuração e testes, consulte o arquivo `GUIA_TESTE_INTEGRACAO.md`.
