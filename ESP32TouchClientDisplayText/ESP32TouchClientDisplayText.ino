#include <Arduino_GFX_Library.h>
#include "TCA9554.h"
#include <Wire.h>
#include <WiFi.h>

// ============================================================
// Waveshare ESP32-S3-Touch-LCD-3.5
// ============================================================

#define GFX_BL       6

#define SPI_MISO     2
#define SPI_MOSI     1
#define SPI_SCLK     5

#define LCD_CS      -1
#define LCD_DC       3
#define LCD_RST     -1

#define LCD_HOR_RES 320
#define LCD_VER_RES 480

#define I2C_SDA      8
#define I2C_SCL      7

// ============================================================
// Wi-Fi / TCP
// ============================================================

// Arduino UNO R4 WiFi access point
const char* WIFI_SSID = "SensorNet";
const char* WIFI_PASSWORD = "Sensor123";

// UNO R4 WiFi AP address
IPAddress SERVER_IP(192, 168, 4, 1);

const uint16_t SERVER_PORT = 5000;

WiFiClient client;

// ============================================================
// TCA9554
// ============================================================

TCA9554 TCA(0x20);

// ============================================================
// Display
// ============================================================

Arduino_DataBus *bus = new Arduino_ESP32SPI(
  LCD_DC,
  LCD_CS,
  SPI_SCLK,
  SPI_MOSI,
  SPI_MISO
);

Arduino_GFX *gfx = new Arduino_ST7796(
  bus,
  LCD_RST,
  0,            // rotation
  true,         // IPS
  LCD_HOR_RES,
  LCD_VER_RES
);

// ============================================================
// Sensor values
// ============================================================

int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;

// ============================================================
// Timing
// ============================================================

unsigned long lastConnectionAttempt = 0;
const unsigned long CONNECTION_INTERVAL = 3000;

// ============================================================
// Display helper
// ============================================================

void displaySensorValues()
{
  // Clear screen
  gfx->fillScreen(RGB565_BLACK);

  // Title
  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(15, 15);
  gfx->println("SENSOR VALUES");

  // ----------------------------------------------------------
  // Sensor 1
  // ----------------------------------------------------------

  gfx->setTextColor(RGB565_CYAN);
  gfx->setTextSize(2);
  gfx->setCursor(20, 75);
  gfx->print("Sensor 1");

  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(4);
  gfx->setCursor(190, 65);
  gfx->print(sensor1);

  // ----------------------------------------------------------
  // Sensor 2
  // ----------------------------------------------------------

  gfx->setTextColor(RGB565_CYAN);
  gfx->setTextSize(2);
  gfx->setCursor(20, 165);
  gfx->print("Sensor 2");

  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(4);
  gfx->setCursor(190, 155);
  gfx->print(sensor2);

  // ----------------------------------------------------------
  // Sensor 3
  // ----------------------------------------------------------

  gfx->setTextColor(RGB565_CYAN);
  gfx->setTextSize(2);
  gfx->setCursor(20, 255);
  gfx->print("Sensor 3");

  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(4);
  gfx->setCursor(190, 245);
  gfx->print(sensor3);

  // ----------------------------------------------------------
  // Sensor 4
  // ----------------------------------------------------------

  gfx->setTextColor(RGB565_CYAN);
  gfx->setTextSize(2);
  gfx->setCursor(20, 345);
  gfx->print("Sensor 4");

  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(4);
  gfx->setCursor(190, 335);
  gfx->print(sensor4);

  // ----------------------------------------------------------
  // Connection status
  // ----------------------------------------------------------

  gfx->setTextColor(RGB565_GREEN);
  gfx->setTextSize(2);
  gfx->setCursor(15, 440);
  gfx->println("CONNECTED");
}

// ============================================================
// Display connection status
// ============================================================

void displayStatus(const char* message)
{
  gfx->fillScreen(RGB565_BLACK);

  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(20, 30);
  gfx->println("SENSOR DISPLAY");

  gfx->setTextColor(RGB565_YELLOW);
  gfx->setTextSize(2);
  gfx->setCursor(20, 100);
  gfx->println(message);
}

// ============================================================
// Connect to Wi-Fi
// ============================================================

bool connectWiFi()
{
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  displayStatus("Connecting WiFi...");

  WiFi.disconnect(true);
  delay(100);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 15000)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    return true;
  }

  Serial.println("WiFi connection failed");

  displayStatus("WiFi FAILED");

  return false;
}

// ============================================================
// Connect to Arduino R4 TCP server
// ============================================================

bool connectTCP()
{
  Serial.print("Connecting to TCP server ");
  Serial.print(SERVER_IP);
  Serial.print(":");
  Serial.println(SERVER_PORT);

  displayStatus("Connecting TCP...");

  if (client.connect(SERVER_IP, SERVER_PORT))
  {
    Serial.println("TCP connected");
    return true;
  }

  Serial.println("TCP connection failed");

  displayStatus("TCP FAILED");

  return false;
}

// ============================================================
// Parse sensor CSV
// ============================================================

bool parseSensorData(String line)
{
  line.trim();

  if (line.length() == 0)
    return false;

  Serial.print("RX: ");
  Serial.println(line);

  int comma1 = line.indexOf(',');

  if (comma1 < 0)
    return false;

  int comma2 = line.indexOf(',', comma1 + 1);

  if (comma2 < 0)
    return false;

  int comma3 = line.indexOf(',', comma2 + 1);

  if (comma3 < 0)
    return false;

  String s1 = line.substring(0, comma1);
  String s2 = line.substring(comma1 + 1, comma2);
  String s3 = line.substring(comma2 + 1, comma3);
  String s4 = line.substring(comma3 + 1);

  sensor1 = s1.toInt();
  sensor2 = s2.toInt();
  sensor3 = s3.toInt();
  sensor4 = s4.toInt();

  Serial.print("Sensor 1 = ");
  Serial.println(sensor1);

  Serial.print("Sensor 2 = ");
  Serial.println(sensor2);

  Serial.print("Sensor 3 = ");
  Serial.println(sensor3);

  Serial.print("Sensor 4 = ");
  Serial.println(sensor4);

  return true;
}

// ============================================================
// Setup
// ============================================================

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-S3 SENSOR DISPLAY");
  Serial.println("=================================");

  // ----------------------------------------------------------
  // I2C / TCA9554
  // ----------------------------------------------------------

  Serial.println("Starting I2C...");

  Wire.begin(I2C_SDA, I2C_SCL);

  TCA.begin();

  Serial.println("TCA9554 initialized");

  // LCD reset through TCA9554 pin 0
  TCA.pinMode1(0, OUTPUT);

  TCA.write1(0, 1);
  delay(10);

  TCA.write1(0, 0);
  delay(10);

  TCA.write1(0, 1);
  delay(200);

  Serial.println("LCD reset complete");

  // ----------------------------------------------------------
  // LCD
  // ----------------------------------------------------------

  Serial.println("Starting LCD...");

  if (!gfx->begin())
  {
    Serial.println("ERROR: gfx->begin() FAILED");

    while (1)
    {
      delay(1000);
    }
  }

  Serial.println("gfx->begin() OK");

  // ----------------------------------------------------------
  // Backlight
  // ----------------------------------------------------------

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  Serial.println("Backlight ON");

  // Initial screen
  displayStatus("Starting...");

  // ----------------------------------------------------------
  // Wi-Fi
  // ----------------------------------------------------------

  if (!connectWiFi())
  {
    Serial.println("Could not connect to WiFi");
    return;
  }

  // ----------------------------------------------------------
  // TCP
  // ----------------------------------------------------------

  connectTCP();
}

// ============================================================
// Main loop
// ============================================================

void loop()
{
  // ----------------------------------------------------------
  // Wi-Fi connection lost
  // ----------------------------------------------------------

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi connection lost");

    client.stop();

    if (millis() - lastConnectionAttempt > CONNECTION_INTERVAL)
    {
      lastConnectionAttempt = millis();

      if (connectWiFi())
      {
        connectTCP();
      }
    }

    delay(100);
    return;
  }

  // ----------------------------------------------------------
  // TCP connection lost
  // ----------------------------------------------------------

  if (!client.connected())
  {
    if (millis() - lastConnectionAttempt > CONNECTION_INTERVAL)
    {
      lastConnectionAttempt = millis();

      Serial.println("TCP connection lost");

      client.stop();

      connectTCP();
    }

    delay(100);
    return;
  }

  // ----------------------------------------------------------
  // Receive sensor data
  // ----------------------------------------------------------

  if (client.available())
  {
    String line = client.readStringUntil('\n');

    if (parseSensorData(line))
    {
      displaySensorValues();
    }
  }

  delay(10);
}