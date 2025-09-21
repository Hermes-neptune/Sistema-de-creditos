/*
 * Sistema de Controle de Créditos
 * Wemos D1 + Teclado 3x4 + LCD 16x2
 * 
 * Conexões:
 * LCD I2C:
 *   SDA -> D6 (GPIO12)
 *   SCL -> D5 (GPIO14)
 *   VCC -> 3.3V
 *   GND -> GND
 * 
 * Teclado 3x4:
 *   R1-R4 -> D0, D2, D4, D8 (linhas)
 *   C1-C3 -> D10, D12, D13, D15 (colunas)
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Wire.h>

// Configurações WiFi
const char* ssid = "nome";
const char* password = "senha";

// URL da API
const char* apiBaseUrl = "Endereço da API";

// Configuração do LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C pode ser 0x27 ou 0x3F

// Configuração do teclado
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3','A'},
  {'4', '5', '6','B'},
  {'7', '8', '9','C'},
  {'*', '0', '#','D'}
};

byte rowPins[ROWS] = {D0, D2, D4, D8}; 
byte colPins[COLS] = {D10, D12, D13, D15};    

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String rmInput = "";
String nomeAluno = "";
int creditosAluno = 0;
bool dadosCarregados = false;
unsigned long lastKeyPress = 0;
const unsigned long TIMEOUT_MS = 30000; // 30 segundos

// Estados do sistema
enum SystemState {
  STATE_INIT,
  STATE_INPUT_RM,
  STATE_LOADING,
  STATE_SHOW_INFO,
  STATE_ERROR
};

SystemState currentState = STATE_INIT;
String errorMessage = "";

void setup() {
  Serial.begin(115200);
  
  // Configurar pinos I2C para LCD
  Wire.begin(D5, D6); // SDA = D6 (GPIO12), SCL = D5 (GPIO14)
  
  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Conectar WiFi
  connectWiFi();
  
  // Tela inicial
  showInitScreen();
  currentState = STATE_INPUT_RM;
}

void loop() {
  char key = customKeypad.getKey();
  
  if (key) {
    lastKeyPress = millis();
    handleKeyPress(key);
  }
  
  // Timeout - voltar ao início
  if (millis() - lastKeyPress > TIMEOUT_MS && currentState != STATE_INIT) {
    resetSystem();
  }
  
  delay(100);
}

void connectWiFi() {
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Aguarde...");
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi conectado!");
    Serial.println("IP: " + WiFi.localIP().toString());
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro WiFi!");
    lcd.setCursor(0, 1);
    lcd.print("Verifique config");
    delay(3000);
  }
}

void showInitScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Creditos");
  lcd.setCursor(0, 1);
  lcd.print("Digite RM:");
  rmInput = "";
  dadosCarregados = false;
}

void handleKeyPress(char key) {
  Serial.println("Tecla pressionada: " + String(key));
  lcd.setCursor(11, 1);
  lcd.print(String(key));  
  switch (currentState) {
    case STATE_INPUT_RM:
      handleInputRM(key);
      break;
      
    case STATE_SHOW_INFO:
      handleShowInfo(key);
      break;
      
    default:
      break;
  }
}

void handleInputRM(char key) {
  if (key >= '0' && key <= '9') {
    if (rmInput.length() < 5) {
      rmInput += key;
      updateRMDisplay();
    }
  }
  else if (key == '#') {
    if (rmInput.length() == 5) {
      consultarAluno();
    } else {
      showError("RM deve ter 5 dig");
    }
  }
  else if (key == '*') {
    if (rmInput.length() > 0) {
      rmInput.remove(rmInput.length() - 1);
      updateRMDisplay();
    } else {
      resetSystem();
    }
  }
}

void handleShowInfo(char key) {
  if (key == '#') {
    debitarCredito();
  }
  else if (key == '*') {
    resetSystem();
  }
}

void updateRMDisplay() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("RM: " + rmInput);
  // Limpar resto da linha
  for (int i = rmInput.length() + 4; i < 16; i++) {
    lcd.print(" ");
    
  }
}

void consultarAluno() {
  currentState = STATE_LOADING;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Consultando...");
  lcd.setCursor(0, 1);
  lcd.print("Aguarde...");
  
  if (WiFi.status() != WL_CONNECTED) {
    showError("Sem conexao WiFi");
    return;
  }
  
  WiFiClient client;
  HTTPClient http;
  
  String url = String(apiBaseUrl) + "/aluno/" + rmInput;
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String payload = http.getString();
    Serial.println("Resposta API: " + payload);
    
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    
    if (doc["success"]) {
      nomeAluno = doc["data"]["nome"].as<String>();
      creditosAluno = doc["data"]["creditos"];
      dadosCarregados = true;
      showAlunoInfo();
      currentState = STATE_SHOW_INFO;
    } else {
      showError("Aluno nao encontr");
    }
  }
  else if (httpResponseCode == 404) {
    showError("RM nao encontrado");
  }
  else {
    showError("Erro servidor");
    Serial.println("Código erro: " + String(httpResponseCode));
  }
  
  http.end();
}

void showAlunoInfo() {
  lcd.clear();
  
  // Mostrar nome (máximo 16 caracteres)
  String nomeDisplay = nomeAluno;
  if (nomeDisplay.length() > 16) {
    nomeDisplay = nomeDisplay.substring(0, 16);
  }
  
  lcd.setCursor(0, 0);
  lcd.print(nomeDisplay);
  
  // Mostrar créditos
  lcd.setCursor(0, 1);
  lcd.print("Creditos: " + String(creditosAluno));
  
  Serial.println("Mostrando info: " + nomeAluno + " - Créditos: " + String(creditosAluno));
}

void debitarCredito() {
  if (!dadosCarregados) {
    showError("Dados nao carregad");
    return;
  }
  
  if (creditosAluno <= 0) {
    showError("Sem creditos!");
    delay(2000);
    showAlunoInfo();
    return;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Debitando...");
  lcd.setCursor(0, 1);
  lcd.print("Aguarde...");
  
  if (WiFi.status() != WL_CONNECTED) {
    showError("Sem conexao WiFi");
    return;
  }
  
  WiFiClient client;
  HTTPClient http;
  
  String url = String(apiBaseUrl) + "/debitar";
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  
  // Preparar JSON
  DynamicJsonDocument doc(256);
  doc["rm"] = rmInput.toInt();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode == 200) {
    String payload = http.getString();
    Serial.println("Resposta débito: " + payload);
    
    DynamicJsonDocument responseDoc(1024);
    deserializeJson(responseDoc, payload);
    
    if (responseDoc["success"]) {
      creditosAluno = responseDoc["data"]["creditos_atual"];
      
      // Mostrar confirmação
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Debito OK!");
      lcd.setCursor(0, 1);
      lcd.print("Novo saldo: " + String(creditosAluno));
      
      delay(2000);
      showAlunoInfo();
    } else {
      String erro = responseDoc["error"];
      showError("Erro: " + erro);
    }
  }
  else {
    showError("Erro debito");
    Serial.println("Código erro débito: " + String(httpResponseCode));
  }
  
  http.end();
}

void showError(String message) {
  currentState = STATE_ERROR;
  errorMessage = message;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERRO:");
  lcd.setCursor(0, 1);
  lcd.print(errorMessage);
  
  Serial.println("Erro: " + errorMessage);
  
  // Voltar automaticamente após 3 segundos
  delay(3000);
  resetSystem();
}

void resetSystem() {
  Serial.println("Resetando sistema");
  rmInput = "";
  nomeAluno = "";
  creditosAluno = 0;
  dadosCarregados = false;
  currentState = STATE_INPUT_RM;
  showInitScreen();
  lastKeyPress = millis();
}
