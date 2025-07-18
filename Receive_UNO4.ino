/*
BikeCarrierParkingSensor project

Unit inside the car:
Receive US Sensor signal
Display distance on LCD display

* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

// Initialize Radio
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

// Initialize LCD Display
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2);


void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // initialyze LCD display
  lcd.init();
  lcd.backlight();
}

void loop() {
  if (radio.available()) {
    //char text[32] = "";
    int distcm = 0;
    radio.read(&distcm, sizeof(distcm));
    //Serial.println(distcm);
    // print on LCD:
    lcd.setCursor(0,0);
    lcd.print("Distance in cm:");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print(distcm);
    delay(200);
  }
}
