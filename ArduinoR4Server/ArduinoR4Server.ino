#include <WiFiS3.h>

// ============================================================
// Wi-Fi Access Point
// ============================================================

const char* AP_SSID = "SensorNet";
const char* AP_PASSWORD = "Sensor123";

IPAddress AP_IP(192, 168, 4, 1);

const uint16_t TCP_PORT = 5000;

WiFiServer server(TCP_PORT);

// Maximum number of simultaneous TCP clients
const int MAX_CLIENTS = 5;

WiFiClient clients[MAX_CLIENTS];

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
Serial.println("MULTI-CLIENT SENSOR DATA");
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

Serial.print("Maximum TCP clients: ");
Serial.println(MAX_CLIENTS);

Serial.println("Waiting for ESP32 clients...");
Serial.println();
}

// ============================================================
// Main loop
// ============================================================

void loop() {

// ==========================================================
// Check existing clients
// ==========================================================

for (int i = 0; i < MAX_CLIENTS; i++) {

if (clients[i] && !clients[i].connected()) {

  Serial.println();
  Serial.print("Client ");
  Serial.print(i + 1);
  Serial.println(" disconnected.");

  clients[i].stop();
}

}

// ==========================================================
// Accept new TCP connections
// ==========================================================

WiFiClient newClient = server.accept();

if (newClient) {

bool clientAdded = false;

for (int i = 0; i < MAX_CLIENTS; i++) {

  if (!clients[i] || !clients[i].connected()) {

    clients[i] = newClient;

    clientAdded = true;

    Serial.println();
    Serial.println("=================================");
    Serial.println("ESP32 CONNECTED!");
    Serial.println("=================================");

    Serial.print("Client number: ");
    Serial.println(i + 1);

    Serial.print("Client IP: ");
    Serial.println(clients[i].remoteIP());

    Serial.print("Client port: ");
    Serial.println(clients[i].remotePort());

    Serial.println();

    break;
  }
}

// --------------------------------------------------------
// No free client slot
// --------------------------------------------------------

if (!clientAdded) {

  Serial.println();
  Serial.println("Maximum number of clients reached.");

  newClient.stop();
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

// --------------------------------------------------------
// Send the same sensor values to every connected client
// --------------------------------------------------------

bool dataSent = false;

for (int i = 0; i < MAX_CLIENTS; i++) {

  if (clients[i] && clients[i].connected()) {

    clients[i].print(sensor1);
    clients[i].print(",");
    clients[i].print(sensor2);
    clients[i].print(",");
    clients[i].print(sensor3);
    clients[i].print(",");
    clients[i].println(sensor4);

    dataSent = true;
  }
}

// --------------------------------------------------------
// Show transmitted data on Serial Monitor
// --------------------------------------------------------

if (dataSent) {

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