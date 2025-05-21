# Guia de Teste e Integração do Sistema de Créditos

Este documento fornece instruções para testar e integrar o Sistema de Créditos desenvolvido para a placa Wemos D1, teclado 3x4 e display LCD 16x2.

## 1. Configuração do Ambiente de Teste

### 1.1 Requisitos de Hardware
- Placa Wemos D1 (ESP8266)
- Teclado matricial 3x4
- Display LCD 16x2 com módulo I2C
- Cabos de conexão
- Fonte de alimentação para o Wemos D1

### 1.2 Requisitos de Software
- Arduino IDE (versão 1.8.x ou superior)
- Bibliotecas necessárias:
  - ESP8266WiFi
  - ESP8266HTTPClient
  - WiFiClient
  - Wire
  - LiquidCrystal_I2C
  - Keypad
  - ArduinoJson (versão 6.x)

### 1.3 Configuração do Servidor PHP
- Servidor web com suporte a PHP (Apache, Nginx, etc.)
- PHP 7.0 ou superior
- Permissões de escrita para o diretório da API (para o arquivo database.json)

## 2. Instalação

### 2.1 Configuração do Servidor
1. Copie a pasta `api` para o diretório raiz do seu servidor web
2. Verifique se o diretório tem permissões de leitura e escrita para o arquivo database.json
3. Teste os endpoints acessando:
   - `http://seu_servidor/sistema_creditos/api/consulta.php?rm=12345`
   - Deve retornar informações do aluno em formato JSON

### 2.2 Configuração do Arduino IDE
1. Instale o suporte para ESP8266 no Arduino IDE:
   - Adicione `http://arduino.esp8266.com/stable/package_esp8266com_index.json` em Preferências > URLs Adicionais de Gerenciadores de Placas
   - Instale a placa ESP8266 via Gerenciador de Placas
2. Instale as bibliotecas necessárias via Gerenciador de Bibliotecas:
   - ESP8266WiFi (incluída no pacote ESP8266)
   - ESP8266HTTPClient (incluída no pacote ESP8266)
   - LiquidCrystal_I2C
   - Keypad
   - ArduinoJson

### 2.3 Configuração do Código
1. Abra o arquivo `wemos_sistema_creditos.ino` no Arduino IDE
2. Modifique as seguintes variáveis:
   - `ssid`: Nome da sua rede WiFi
   - `password`: Senha da sua rede WiFi
   - `serverName`: URL do seu servidor (ex: "http://192.168.1.100/sistema_creditos/api/")
3. Verifique as conexões dos pinos conforme comentários no código:
   - Teclado 3x4: Pinos D1, D2, D3, D4, D5, D6, D7
   - LCD 16x2 (I2C): Pinos D2 (SDA), D1 (SCL)

## 3. Conexões de Hardware

### 3.1 Conexão do Teclado 3x4
- Linhas (R1, R2, R3, R4) -> D1, D2, D3, D4
- Colunas (C1, C2, C3) -> D5, D6, D7

### 3.2 Conexão do LCD 16x2 com I2C
- SDA -> D2
- SCL -> D1
- VCC -> 5V
- GND -> GND

## 4. Testes

### 4.1 Teste da API
1. Acesse `http://seu_servidor/sistema_creditos/api/consulta.php?rm=12345` em um navegador
2. Verifique se retorna um JSON com os dados do aluno
3. Teste o débito de crédito usando uma ferramenta como Postman ou curl:
   ```
   curl -X POST -d "rm=12345" http://seu_servidor/sistema_creditos/api/debitar.php
   ```
4. Verifique se o crédito foi debitado consultando novamente o aluno

### 4.2 Teste do Sistema Completo
1. Carregue o código no Wemos D1
2. Observe o display LCD:
   - Deve exibir "Sistema Creditos" e "Digite o RM:"
3. Digite um RM válido (ex: 12345) usando o teclado
4. Pressione "#" para consultar
   - O display deve mostrar o nome e créditos do aluno
5. Pressione "#" novamente para debitar um crédito
   - O display deve mostrar a confirmação e atualizar os créditos
6. Pressione "*" para voltar à tela inicial

## 5. Solução de Problemas

### 5.1 Problemas de Conexão WiFi
- Verifique se as credenciais WiFi estão corretas
- Certifique-se de que o sinal WiFi é forte o suficiente
- Verifique se o roteador está configurado para permitir novos dispositivos

### 5.2 Problemas com a API
- Verifique se o servidor está acessível na rede
- Confirme se os arquivos PHP têm permissões corretas
- Verifique se o arquivo database.json tem permissões de leitura e escrita
- Teste os endpoints diretamente no navegador ou com ferramentas como Postman

### 5.3 Problemas com o Display LCD
- Verifique o endereço I2C do display (padrão é 0x27, mas pode variar)
- Confirme as conexões SDA e SCL
- Teste o display com um código simples de exemplo

### 5.4 Problemas com o Teclado
- Verifique as conexões das linhas e colunas
- Teste o teclado com um código simples de exemplo
- Confirme se os pinos definidos no código correspondem às conexões físicas

## 6. Melhorias Futuras

- Adicionar autenticação à API para maior segurança
- Implementar cache de dados para reduzir requisições à API
- Adicionar suporte para múltiplos usuários simultâneos
