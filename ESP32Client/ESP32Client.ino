#include <WiFi.h>

// ============================================================
// Wi-Fi
// ============================================================

const char* WIFI_SSID = "SensorNet";
const char* WIFI_PASSWORD = "Sensor123";

IPAddress SERVER_IP(192, 168, 4, 1);

const uint16_t SERVER_PORT = 5000;

WiFiClient client;

// ============================================================
// Sensor values
// ============================================================

int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;


// ============================================================
// Setup
// ============================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32 Sensor Client");
  Serial.println("=================================");

  // ----------------------------------------------------------
  // Connect to UNO R4 Wi-Fi
  // ----------------------------------------------------------

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");

  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  // ----------------------------------------------------------
  // Connect to UNO R4 TCP server
  // ----------------------------------------------------------

  connectToServer();
}


// ============================================================
// Connect to server
// ============================================================

void connectToServer() {

  Serial.print("Connecting to ");
  Serial.print(SERVER_IP);
  Serial.print(":");
  Serial.println(SERVER_PORT);

  while (!client.connected()) {

    if (client.connect(SERVER_IP, SERVER_PORT)) {

      Serial.println("TCP connection established!");
      Serial.println();

      return;
    }

    Serial.println("Connection failed. Retrying...");
    delay(1000);
  }
}


// ============================================================
// Main loop
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // Reconnect if connection was lost
  // ----------------------------------------------------------

  if (!client.connected()) {

    Serial.println("TCP connection lost.");

    client.stop();

    connectToServer();
  }


  // ----------------------------------------------------------
  // Receive sensor data
  // ----------------------------------------------------------

  if (client.available()) {

    String line = client.readStringUntil('\n');

    line.trim();

    if (line.length() > 0) {

      Serial.print("RX: ");
      Serial.println(line);

      // ------------------------------------------------------
      // Find commas
      // ------------------------------------------------------

      int comma1 = line.indexOf(',');
      int comma2 = line.indexOf(',', comma1 + 1);
      int comma3 = line.indexOf(',', comma2 + 1);

      if (comma1 > 0 &&
          comma2 > comma1 &&
          comma3 > comma2) {

        sensor1 = line.substring(0, comma1).toInt();

        sensor2 = line.substring(
          comma1 + 1,
          comma2
        ).toInt();

        sensor3 = line.substring(
          comma2 + 1,
          comma3
        ).toInt();

        sensor4 = line.substring(
          comma3 + 1
        ).toInt();

        // ----------------------------------------------------
        // Display parsed values
        // ----------------------------------------------------

        Serial.print("Sensor 1: ");
        Serial.println(sensor1);

        Serial.print("Sensor 2: ");
        Serial.println(sensor2);

        Serial.print("Sensor 3: ");
        Serial.println(sensor3);

        Serial.print("Sensor 4: ");
        Serial.println(sensor4);

        Serial.println("-----------------------------");
      }
    }
  }
}