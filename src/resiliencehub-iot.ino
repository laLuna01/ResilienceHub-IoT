#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

// Definições dos pinos
#define LED_TERREMOTO 21    // LED1 - Alerta de terremoto (vibração)
#define LED_CALOR 23        // LED2 - Alerta de calor extremo
#define LED_ENCHENTE 22     // LED3 - Alerta de enchente
#define BUZZER 2
#define DHT 13              // Sensor de temperatura/umidade
#define HC_TRIG 32          // Sensor de distância (nível água)
#define HC_ECHO 33
#define POT 34              // Potenciômetro (simula sensor sísmico)

// Configurações
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dhtSensor;

// Variáveis para controle de alertas
bool alerta_terremoto = false;
bool alerta_calor = false;
bool alerta_enchente = false;
String ultimo_alerta = "Nenhum alerta ativo";

unsigned long ultimoAlarme = 0;
int frequenciaAtual = 800;
bool crescendo = true;
bool alarmeAtivo = false;

// Limiares para os alertas
const float TEMP_ALERTA = 35.0;       // Temperatura crítica (°C)
const int VIBRACAO_ALERTA = 3000;     // Nível de vibração crítico
const float DISTANCIA_ALERTA = 20.0;  // Distância crítica para enchente (cm)

void setup_wifi() {
  delay(10);
  Serial.println("\nConectando a " + String(ssid));
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi conectado\nIP: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    String clientId = "ESP32-AlertaDesastres-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      // Publica status inicial
      client.publish("esp32/alertas/status", "Sistema iniciado");
    } else {
      Serial.println("falha, rc=" + String(client.state()) + " tentando em 5s...");
      delay(5000);
    }
  }
}

void verificarAlertas(float temperatura, int vibracao, float distancia) {
  // Reinicia estados de alerta
  alerta_terremoto = false;
  alerta_calor = false;
  alerta_enchente = false;
  
  // Verifica cada tipo de alerta
  if (vibracao > VIBRACAO_ALERTA) {
    alerta_terremoto = true;
    ultimo_alerta = "ALERTA: Terremoto detectado! Vibração: " + String(vibracao);
  }
  
  if (temperatura > TEMP_ALERTA) {
    alerta_calor = true;
    ultimo_alerta = "ALERTA: Calor extremo! Temperatura: " + String(temperatura, 1) + "°C";
  }
  
  if (distancia < DISTANCIA_ALERTA) {
    alerta_enchente = true;
    ultimo_alerta = "ALERTA: Risco de enchente! Nível água: " + String(DISTANCIA_ALERTA - distancia, 1) + "cm";
  }
  
  // Ativa LEDs e buzzer conforme alertas
  digitalWrite(LED_TERREMOTO, alerta_terremoto ? HIGH : LOW);
  digitalWrite(LED_CALOR, alerta_calor ? HIGH : LOW);
  digitalWrite(LED_ENCHENTE, alerta_enchente ? HIGH : LOW);
  
  alarmeAtivo = (alerta_terremoto || alerta_calor || alerta_enchente);

  // Ativa buzzer se qualquer alerta estiver ativo
  if (!alarmeAtivo) {
    noTone(BUZZER);
    ultimo_alerta = "Monitoramento normal";
  }
}

void atualizarAlarme() {
  if (!alarmeAtivo) return;

  unsigned long agora = millis();
  
  // Atualiza a frequência a cada 50ms para criar efeito de sirene
  if (agora - ultimoAlarme > 50) {
    ultimoAlarme = agora;
    
    // Varia a frequência entre 800-1200Hz
    if (crescendo) {
      frequenciaAtual += 10;
      if (frequenciaAtual >= 1200) crescendo = false;
    } else {
      frequenciaAtual -= 10;
      if (frequenciaAtual <= 800) crescendo = true;
    }
    
    tone(BUZZER, frequenciaAtual);
  }
  
  // Padrão intermitente (1s ligado, 1s desligado)
  static bool buzzerLigado = true;
  static unsigned long ultimoToggle = 0;
  
  if (agora - ultimoToggle > 1000) {
    ultimoToggle = agora;
    buzzerLigado = !buzzerLigado;
    
    if (buzzerLigado) {
      tone(BUZZER, frequenciaAtual);
    } else {
      noTone(BUZZER);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configura pinos
  pinMode(LED_TERREMOTO, OUTPUT);
  pinMode(LED_CALOR, OUTPUT);
  pinMode(LED_ENCHENTE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(POT, INPUT);
  pinMode(HC_TRIG, OUTPUT);
  pinMode(HC_ECHO, INPUT);

  // Inicializa sensor DHT
  dhtSensor.setup(DHT, DHTesp::DHT22);
  
  // Conecta WiFi e MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) { // Atualiza a cada 2 segundos
    lastUpdate = millis();
    
    // Lê sensores
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    int vibracao = analogRead(POT);
    
    // Mede distância (nível de água simulado)
    digitalWrite(HC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(HC_TRIG, LOW);
    float distancia_cm = 0.017 * pulseIn(HC_ECHO, HIGH);
    
    // Verifica alertas
    verificarAlertas(data.temperature, vibracao, distancia_cm);
    
    // Publica dados e alertas
    client.publish("esp32/alertas/temperatura", String(data.temperature, 1).c_str());
    client.publish("esp32/alertas/vibracao", String(vibracao).c_str());
    client.publish("esp32/alertas/nivel_agua", String(distancia_cm, 1).c_str());
    client.publish("esp32/alertas/ultimo_alerta", ultimo_alerta.c_str());
    
    // Debug no Serial Monitor
    Serial.println("=== Dados dos Sensores ===");
    Serial.println("Temperatura: " + String(data.temperature, 1) + "°C");
    Serial.println("Vibração: " + String(vibracao));
    Serial.println("Distância: " + String(distancia_cm, 1) + " cm");
    Serial.println("Último Alerta: " + ultimo_alerta);
    Serial.println("=========================");

    atualizarAlarme();
  }

  atualizarAlarme();
}