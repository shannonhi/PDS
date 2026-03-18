#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"

// -------------------------
// Pin definitions
// -------------------------
const int RAIN_ANALOG_PIN = 1;   // AO from rain sensor module
const int LED_PIN = 2;            // Change if your ESP32 uses a different LED pin

// -------------------------
// Sensor threshold
// Lower value often means wetter on many modules.
// You may need to adjust after testing.
// -------------------------
int rainThreshold = 2500;

// -------------------------
// AWS / MQTT
// -------------------------
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

// -------------------------
// Timing
// -------------------------
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 5000;

// -------------------------
// State
// -------------------------
bool forcedLedMode = false;
bool forcedLedState = false;

// -------------------------
// Function declarations
// -------------------------
void connectWiFi();
void connectAWS();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishRainData();
void updateLedFromSensor(bool wet);
String getMacBasedClientId();
void printWiFiInfo();

// -------------------------
// Setup
// -------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  analogReadResolution(12);  // ESP32 ADC resolution: 0-4095

  connectWiFi();
  printWiFiInfo();

  secureClient.setCACert(AWS_ROOT_CA);
  secureClient.setCertificate(AWS_CERT_CRT);
  secureClient.setPrivateKey(AWS_PRIVATE_KEY);

  mqttClient.setServer(AWS_IOT_ENDPOINT, 8883);
  mqttClient.setCallback(mqttCallback);

  connectAWS();
}

// -------------------------
// Main loop
// -------------------------
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    Serial.println("MQTT disconnected. Reconnecting...");
    connectAWS();
  }

  mqttClient.loop();

  unsigned long now = millis();
  if (now - lastPublishTime >= publishInterval) {
    lastPublishTime = now;
    publishRainData();
  }
}

// -------------------------
// Connect to WiFi
// -------------------------
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
}

// -------------------------
// Print network info
// -------------------------
void printWiFiInfo() {
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

// -------------------------
// Connect to AWS IoT Core
// -------------------------
void connectAWS() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to AWS IoT Core...");

    String clientId = getMacBasedClientId();

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");

      mqttClient.subscribe(AWS_SUBSCRIBE_TOPIC);

      mqttClient.publish(AWS_STATUS_TOPIC, "{\"status\":\"online\"}");

    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 3 seconds...");
      delay(3000);
    }
  }
}

// -------------------------
// Generate unique client ID
// -------------------------
String getMacBasedClientId() {
  uint64_t chipid = ESP.getEfuseMac();
  char clientId[32];
  snprintf(clientId, sizeof(clientId), "esp32-rain-%04X", (uint16_t)(chipid & 0xFFFF));
  return String(clientId);
}

// -------------------------
// MQTT callback
// Expected commands:
// {"led":"on"}
// {"led":"off"}
// {"led":"auto"}
// -------------------------
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Payload: ");
  Serial.println(msg);

  if (msg.indexOf("\"led\":\"on\"") >= 0) {
    forcedLedMode = true;
    forcedLedState = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED forced ON");
  }
  else if (msg.indexOf("\"led\":\"off\"") >= 0) {
    forcedLedMode = true;
    forcedLedState = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED forced OFF");
  }
  else if (msg.indexOf("\"led\":\"auto\"") >= 0) {
    forcedLedMode = false;
    Serial.println("LED returned to AUTO mode");
  }
}

// -------------------------
// LED behavior
// -------------------------
void updateLedFromSensor(bool wet) {
  if (forcedLedMode) {
    digitalWrite(LED_PIN, forcedLedState ? HIGH : LOW);
  } else {
    digitalWrite(LED_PIN, wet ? HIGH : LOW);
  }
}

// -------------------------
// Read sensor and publish JSON
// -------------------------
void publishRainData() {
  int rainValue = analogRead(RAIN_ANALOG_PIN);

  // Many rain modules behave like:
  // lower analog value = wetter
  bool wet = (rainValue < rainThreshold);

  updateLedFromSensor(wet);

  String payload = "{";
  payload += "\"device\":\"esp32-rain-sensor\",";
  payload += "\"analog\":";
  payload += rainValue;
  payload += ",";
  payload += "\"wet\":";
  payload += (wet ? "true" : "false");
  payload += ",";
  payload += "\"threshold\":";
  payload += rainThreshold;
  payload += ",";
  payload += "\"led_mode\":\"";
  payload += (forcedLedMode ? "manual" : "auto");
  payload += "\",";
  payload += "\"led_state\":";
  payload += (digitalRead(LED_PIN) ? "true" : "false");
  payload += "}";

  Serial.print("Publishing: ");
  Serial.println(payload);

  if (mqttClient.publish(AWS_PUBLISH_TOPIC, payload.c_str())) {
    Serial.println("Publish success");
  } else {
    Serial.println("Publish failed");
  }
}