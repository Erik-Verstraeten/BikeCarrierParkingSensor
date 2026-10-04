#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
// OLED
// ============================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 21
#define OLED_SCL 22

#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);


// ============================================================
// Wi-Fi
// ============================================================

const char* WIFI_SSID = "SensorNet";
const char* WIFI_PASSWORD = "Sensor123";

// UNO R4 WiFi Access Point
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
// Display sensor values
// ============================================================

void updateDisplay() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // ----------------------------------------------------------
  // Title
  // ----------------------------------------------------------

  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("Sensor Data");

  display.drawLine(
    0, 10,
    127, 10,
    SSD1306_WHITE
  );

  // ----------------------------------------------------------
  // Sensor 1
  // ----------------------------------------------------------

  display.setTextSize(1);

  display.setCursor(0, 16);
  display.print("Sensor 1:");

  display.setCursor(72, 16);
  display.setTextSize(1);
  display.print(sensor1);

  // ----------------------------------------------------------
  // Sensor 2
  // ----------------------------------------------------------

  display.setTextSize(1);

  display.setCursor(0, 29);
  display.print("Sensor 2:");

  display.setCursor(72, 29);
  display.setTextSize(1);
  display.print(sensor2);

  // ----------------------------------------------------------
  // Sensor 3
  // ----------------------------------------------------------

  display.setTextSize(1);

  display.setCursor(0, 42);
  display.print("Sensor 3:");

  display.setCursor(72, 42);
  display.setTextSize(1);
  display.print(sensor3);

  // ----------------------------------------------------------
  // Sensor 4
  // ----------------------------------------------------------

  display.setTextSize(1);

  display.setCursor(0, 55);
  display.print("Sensor 4:");

  display.setCursor(72, 55);
  display.setTextSize(1);
  display.print(sensor4);

  display.display();
}


// ============================================================
// Display connection status
// ============================================================

void showConnectionStatus(const char* message) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("ESP32 Sensor Client");

  display.drawLine(
    0, 10,
    127, 10,
    SSD1306_WHITE
  );

  display.setCursor(0, 22);
  display.println(message);

  display.display();
}


// ============================================================
// Connect to UNO R4 TCP server
// ============================================================

void connectToServer() {

  showConnectionStatus("Connecting...");

  Serial.print("Connecting to ");
  Serial.print(SERVER_IP);
  Serial.print(":");
  Serial.println(SERVER_PORT);

  while (!client.connected()) {

    if (client.connect(SERVER_IP, SERVER_PORT)) {

      Serial.println("TCP connection established!");

      showConnectionStatus("TCP connected");

      delay(500);

      return;
    }

    Serial.println("Connection failed. Retrying...");

    showConnectionStatus("Connection failed");

    delay(1000);
  }
}


// ============================================================
// Setup
// ============================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-WROOM-32 Sensor Client");
  Serial.println("=================================");


  // ==========================================================
  // Initialize I2C OLED
  // ==========================================================

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
      )) {

    Serial.println("ERROR: OLED not found!");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("OLED initialized.");

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("ESP32 Sensor Client");

  display.setCursor(0, 20);
  display.println("Starting...");

  display.display();

  delay(1000);


  // ==========================================================
  // Connect to UNO R4 Wi-Fi
  // ==========================================================

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  showConnectionStatus("Connecting WiFi...");

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("Wi-Fi connected.");

  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());


  // ==========================================================
  // Connect to UNO R4 TCP server
  // ==========================================================

  connectToServer();
}


// ============================================================
// Main loop
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // Check TCP connection
  // ----------------------------------------------------------

  if (!client.connected()) {

    Serial.println("TCP connection lost.");

    client.stop();

    showConnectionStatus("TCP disconnected");

    delay(500);

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


      // ------------------------------------------------------
      // Check that packet is valid
      // ------------------------------------------------------

      if (comma1 > 0 &&
          comma2 > comma1 &&
          comma3 > comma2) {

        sensor1 = line.substring(
          0,
          comma1
        ).toInt();

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
        // Serial output
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


        // ----------------------------------------------------
        // Update OLED
        // ----------------------------------------------------

        updateDisplay();
      }
    }
  }
}