/*

BikeCarrierParkingSensor project

*** Transmittor ***

Unit on the Bike carrier
- radio
- rain resistant Ultrasonic Sensor

* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

// Radio initialisation
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

// Data structure for data transfer
struct DataPackage {
  int dist1 = 0;
  int dist2 = 0;
  int dist3 = 0;
  int dist4 = 0;
};

DataPackage data;

// Ultrasnic sensor intialisation
const int trigPin = 3;
const int echoPin = 4;

float duration, distance;

int tel = 0;
int distcm = 0;

void setup() {
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  

  Serial.begin(9600);
  
  radio.begin();
  
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  distcm = round(distance);
  data.dist1 = distcm;
  data.dist2 = distcm;
  data.dist3 = distcm;
  data.dist4 = distcm;

  Serial.print("Distance: ");
  Serial.println(distcm);

  //const char text[30] = "Hello World " ;
  //char buffer[5];
  //itoa(tel, buffer,10);
  //strcat(text, buffer);
  //radio.write(&text, sizeof(text));
  radio.write(&data,sizeof(data));
  Serial.println(tel);
  tel = tel + 1;
  delay(100);
}
