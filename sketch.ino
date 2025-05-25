#include <WiFi.h>
#include <PubSubClient.h>

// Configurações WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurações MQTT
const char* mqttBroker = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttTopicMotos = "mottu/patio/motos";
const char* mqttTopicComandos = "mottu/patio/comandos";

// Pinos
#define PIN_RED 18
#define PIN_GREEN 19
#define PIN_BLUE 21
#define BUZZER_PIN 22
#define BUTTON_PIN 5

// Estados da moto
enum MotoStatus {
  PRONTA_PARA_ALUGAR = 0,
  PENDENTE_REGULARIZACAO = 1,
  EM_MANUTENCAO = 2
};

struct Moto {
  String id;
  MotoStatus status;
  String placa;
  String modelo;
  float latitude;
  float longitude;
};

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Moto moto;
unsigned long lastUpdate = 0;
bool buttonPressed = false;

void setColor(int red, int green, int blue) {
  digitalWrite(PIN_RED, red > 0 ? LOW : HIGH);
  digitalWrite(PIN_GREEN, green > 0 ? LOW : HIGH);
  digitalWrite(PIN_BLUE, blue > 0 ? LOW : HIGH);
}

void atualizarLedStatus() {
  switch (moto.status) {
    case PRONTA_PARA_ALUGAR:
      setColor(0, 255, 0); // Verde
      break;
    case PENDENTE_REGULARIZACAO:
      setColor(255, 150, 0); // Amarelo vibrante
      break;
    case EM_MANUTENCAO:
      setColor(255, 0, 0); // Vermelho
      break;
  }
}

void playStatusSound(MotoStatus status) {
  switch (status) {
    case PRONTA_PARA_ALUGAR:
      tone(BUZZER_PIN, 1000, 200);
      break;
    case PENDENTE_REGULARIZACAO:
      tone(BUZZER_PIN, 800, 400);
      break;
    case EM_MANUTENCAO:
      tone(BUZZER_PIN, 500, 800);
      break;
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (mqttClient.connect("ESP32-Moto")) {
      Serial.println("Conectado!");
      mqttClient.subscribe(mqttTopicComandos);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == mqttTopicComandos) {
    if (message == "ALERTA") {
      tone(BUZZER_PIN, 1000, 1000);
      setColor(255, 255, 0); // Amarelo piscante para alerta
      delay(500);
      atualizarLedStatus();
    }
    else if (message == "PRONTA_PARA_ALUGAR") {
      moto.status = PRONTA_PARA_ALUGAR;
      atualizarLedStatus();
      playStatusSound(moto.status);
      enviarDadosMQTT();
    }
    else if (message == "PENDENTE_REGULARIZACAO") {
      moto.status = PENDENTE_REGULARIZACAO;
      atualizarLedStatus();
      playStatusSound(moto.status);
      enviarDadosMQTT();
    }
    else if (message == "EM_MANUTENCAO") {
      moto.status = EM_MANUTENCAO;
      atualizarLedStatus();
      playStatusSound(moto.status);
      enviarDadosMQTT();
    }
  }
}

void atualizarPosicao() {
  moto.latitude = -23.5 + (random(0, 1000) / 10000.0);
  moto.longitude = -46.6 + (random(0, 1000) / 10000.0);
}

void enviarDadosMQTT() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  String payload = 
    "{\"id\":\"" + moto.id + 
    "\",\"status\":" + moto.status + 
    ",\"lat\":" + String(moto.latitude, 6) + 
    ",\"lon\":" + String(moto.longitude, 6) + 
    ",\"placa\":\"" + moto.placa + 
    "\",\"modelo\":\"" + moto.modelo + "\"}";

  mqttClient.publish(mqttTopicMotos, payload.c_str());
  Serial.println("Dados enviados via MQTT: " + payload);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  setColor(0, 0, 0);
  digitalWrite(BUZZER_PIN, LOW);
  
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
  
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(mqttCallback);
  
  moto.id = "moto-" + String(ESP.getEfuseMac(), HEX);
  moto.status = PRONTA_PARA_ALUGAR;
  moto.placa = "XYZ" + String(random(1000, 9999));
  moto.modelo = "Honda CG 160";
  atualizarPosicao();
  
  atualizarLedStatus();
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  if (digitalRead(BUTTON_PIN) == LOW && !buttonPressed) {
    buttonPressed = true;
    delay(50);
    
    moto.status = (MotoStatus)((moto.status + 1) % 3);
    atualizarLedStatus();
    enviarDadosMQTT();
    playStatusSound(moto.status);
  }
  
  if (digitalRead(BUTTON_PIN) == HIGH && buttonPressed) {
    buttonPressed = false;
  }
  
  if (millis() - lastUpdate > 10000) {
    atualizarPosicao();
    enviarDadosMQTT();
    lastUpdate = millis();
  }
}