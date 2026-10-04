#include <WiFiS3.h>

// ============================================================
// Wi-Fi Access Point
// ============================================================

const char* AP_SSID = "SensorNet";
const char* AP_PASSWORD = "Sensor123";

IPAddress AP_IP(192, 168, 4, 1);

const uint16_t TCP_PORT = 5000;

WiFiServer server(TCP_PORT);
WiFiClient client;

// ============================================================
// Simulated sensor data
// ============================================================

const unsigned long SENSOR_INTERVAL = 500;
unsigned long lastSensorUpdate = 0;


// ============================================================
// Setup
// ============================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=================================");
  Serial.println("UNO R4 WiFi Sensor Server");
  Serial.println("SIMULATED SENSOR DATA");
  Serial.println("=================================");

  // Random seed
  randomSeed(analogRead(A0));

  // ----------------------------------------------------------
  // Start Access Point
  // ----------------------------------------------------------

  Serial.print("Starting Access Point: ");
  Serial.println(AP_SSID);

  WiFi.config(AP_IP);

  int status = WiFi.beginAP(AP_SSID, AP_PASSWORD);

  if (status != WL_AP_LISTENING) {

    Serial.print("ERROR starting AP. Status = ");
    Serial.println(status);

    while (true) {
      delay(1000);
    }
  }

  delay(1000);

  Serial.println();
  Serial.println("Access Point started.");

  Serial.print("SSID: ");
  Serial.println(AP_SSID);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ----------------------------------------------------------
  // Start TCP server
  // ----------------------------------------------------------

  server.begin();

  Serial.print("TCP server listening on port ");
  Serial.println(TCP_PORT);

  Serial.println("Waiting for ESP32...");
  Serial.println();
}


// ============================================================
// Main loop
// ============================================================

void loop() {

  // ==========================================================
  // Accept incoming TCP connection
  // ==========================================================

  if (!client || !client.connected()) {

    WiFiClient newClient = server.accept();

    if (newClient) {

      client = newClient;

      Serial.println();
      Serial.println("=================================");
      Serial.println("ESP32 CONNECTED!");
      Serial.println("=================================");

      Serial.print("Client IP: ");
      Serial.println(client.remoteIP());

      Serial.print("Client port: ");
      Serial.println(client.remotePort());

      Serial.println();
    }
  }


  // ==========================================================
  // Generate and send simulated sensor data
  // ==========================================================

  unsigned long now = millis();

  if (now - lastSensorUpdate >= SENSOR_INTERVAL) {

    lastSensorUpdate = now;

    int sensor1 = random(0, 1001);
    int sensor2 = random(0, 1001);
    int sensor3 = random(0, 1001);
    int sensor4 = random(0, 1001);

    if (client && client.connected()) {

      client.print(sensor1);
      client.print(",");
      client.print(sensor2);
      client.print(",");
      client.print(sensor3);
      client.print(",");
      client.println(sensor4);

      Serial.print("TX: ");
      Serial.print(sensor1);
      Serial.print(",");
      Serial.print(sensor2);
      Serial.print(",");
      Serial.print(sensor3);
      Serial.print(",");
      Serial.println(sensor4);
    }
  }
}