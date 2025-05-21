/*
 * Sistema de Créditos para Alunos
 * 
 * Este código implementa um sistema para consulta e débito de créditos de alunos
 * utilizando uma placa Wemos D1, um teclado 3x4 e um display LCD 16x2.
 * 
 * Funcionalidades:
 * - Entrada do RM (5 dígitos) via teclado 3x4
 * - Consulta de informações do aluno via API PHP
 * - Exibição do nome e créditos no display LCD
 * - Débito de créditos ao pressionar "#"
 * - Encerramento do ciclo ao pressionar "*"
 * 
 * Conexões:
 * - Teclado 3x4: Pinos D1, D2, D3, D4, D5, D6, D7
 * - LCD 16x2 (I2C): Pinos D2 (SDA), D1 (SCL)
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ArduinoJson.h>

// Configurações do WiFi
const char* ssid = "SUA_REDE_WIFI";
const char* password = "SUA_SENHA_WIFI";

// Configurações da API
const char* serverName = "http://seu_servidor/sistema_creditos/api/";
String consultaEndpoint = "consulta.php?rm=";
String debitarEndpoint = "debitar.php";

// Configurações do teclado 3x4
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {D1, D2, D3, D4}; // Conectar a R1, R2, R3, R4
byte colPins[COLS] = {D5, D6, D7};     // Conectar a C1, C2, C3
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Configurações do LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C 0x27, 16 colunas, 2 linhas

// Variáveis globais
String rm = "";
String nomeAluno = "";
int creditos = 0;
bool alunoConsultado = false;

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);
  
  // Inicializa o LCD
  Wire.begin(D2, D1); // SDA, SCL
  lcd.begin();
  lcd.backlight();
  
  // Conecta ao WiFi
  conectarWiFi();
  
  // Exibe mensagem inicial
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Creditos");
  lcd.setCursor(0, 1);
  lcd.print("Digite o RM:");
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    // Se o aluno já foi consultado e está no modo de operação
    if (alunoConsultado) {
      if (key == '#') {
        // Debita um crédito
        debitarCredito();
      } else if (key == '*') {
        // Volta para o modo de entrada de RM
        resetarSistema();
      }
    } else {
      // Modo de entrada de RM
      if (key >= '0' && key <= '9') {
        // Adiciona dígito ao RM se ainda não tiver 5 dígitos
        if (rm.length() < 5) {
          rm += key;
          atualizarDisplayRM();
        }
      } else if (key == '#' && rm.length() == 5) {
        // Consulta o aluno quando o RM estiver completo
        consultarAluno();
      } else if (key == '*') {
        // Limpa o RM atual
        rm = "";
        atualizarDisplayRM();
      }
    }
  }
}

void conectarWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");
  
  WiFi.begin(ssid, password);
  
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    lcd.setCursor(tentativas % 16, 1);
    lcd.print(".");
    tentativas++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Conectado!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Falha na conexao");
    lcd.setCursor(0, 1);
    lcd.print("Reiniciando...");
    delay(3000);
    ESP.restart();
  }
}

void atualizarDisplayRM() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite o RM:");
  lcd.setCursor(0, 1);
  lcd.print(rm);
}

void consultarAluno() {
  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro de conexao");
    lcd.setCursor(0, 1);
    lcd.print("Tente novamente");
    delay(2000);
    resetarSistema();
    return;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Consultando...");
  
  WiFiClient client;
  HTTPClient http;
  
  String url = String(serverName) + consultaEndpoint + rm;
  http.begin(client, url);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String payload = http.getString();
    
    // Aloca memória para o documento JSON
    DynamicJsonDocument doc(1024);
    
    // Analisa o JSON
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      bool sucesso = doc["sucesso"];
      
      if (sucesso) {
        nomeAluno = doc["nome"].as<String>();
        creditos = doc["creditos"];
        alunoConsultado = true;
        
        // Exibe as informações do aluno
        exibirInfoAluno();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Erro na consulta");
        lcd.setCursor(0, 1);
        lcd.print("Tente novamente");
        delay(2000);
        resetarSistema();
      }
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro no formato");
      lcd.setCursor(0, 1);
      lcd.print("Tente novamente");
      delay(2000);
      resetarSistema();
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro HTTP: ");
    lcd.print(httpResponseCode);
    delay(2000);
    resetarSistema();
  }
  
  http.end();
}

void exibirInfoAluno() {
  lcd.clear();
  
  // Exibe o nome do aluno (truncado se necessário)
  lcd.setCursor(0, 0);
  if (nomeAluno.length() > 16) {
    lcd.print(nomeAluno.substring(0, 16));
  } else {
    lcd.print(nomeAluno);
  }
  
  // Exibe os créditos
  lcd.setCursor(0, 1);
  lcd.print("Creditos: ");
  lcd.print(creditos);
  
  // Exibe instruções no Serial para debug
  Serial.println("Aluno consultado:");
  Serial.println("RM: " + rm);
  Serial.println("Nome: " + nomeAluno);
  Serial.println("Créditos: " + String(creditos));
  Serial.println("Pressione # para debitar ou * para sair");
}

void debitarCredito() {
  if (creditos <= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sem creditos");
    lcd.setCursor(0, 1);
    lcd.print("Pressione *");
    delay(2000);
    exibirInfoAluno();
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro de conexao");
    lcd.setCursor(0, 1);
    lcd.print("Tente novamente");
    delay(2000);
    exibirInfoAluno();
    return;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Debitando...");
  
  WiFiClient client;
  HTTPClient http;
  
  String url = String(serverName) + debitarEndpoint;
  http.begin(client, url);
  
  // Especifica o tipo de conteúdo
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Envia a requisição POST com o RM
  String httpRequestData = "rm=" + rm;
  int httpResponseCode = http.POST(httpRequestData);
  
  if (httpResponseCode == 200) {
    String payload = http.getString();
    
    // Aloca memória para o documento JSON
    DynamicJsonDocument doc(1024);
    
    // Analisa o JSON
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      bool sucesso = doc["sucesso"];
      
      if (sucesso) {
        creditos = doc["creditos"];
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Credito debitado");
        lcd.setCursor(0, 1);
        lcd.print("Restantes: ");
        lcd.print(creditos);
        delay(2000);
        exibirInfoAluno();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Erro ao debitar");
        lcd.setCursor(0, 1);
        lcd.print("Tente novamente");
        delay(2000);
        exibirInfoAluno();
      }
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro no formato");
      lcd.setCursor(0, 1);
      lcd.print("Tente novamente");
      delay(2000);
      exibirInfoAluno();
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro HTTP: ");
    lcd.print(httpResponseCode);
    delay(2000);
    exibirInfoAluno();
  }
  
  http.end();
}

void resetarSistema() {
  rm = "";
  nomeAluno = "";
  creditos = 0;
  alunoConsultado = false;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Creditos");
  lcd.setCursor(0, 1);
  lcd.print("Digite o RM:");
}
