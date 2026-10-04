# BikeCarrierParkingSensor
Parking sensor for bicycle carrier

Many cars are equipped with sophisticated parking sensors. But when you take bikes with a bike carrier, the functionality is rightfully swithced off.
Actually when you need the functionality the most. 
The goal of this project to develop a parking sensor for a bike carrier. 
The setup is as follows:

4 weather resistant IR-distance sensors wired to an Arduino R4 Wifi. This is the part attached to the bike carrier, outside of the vehicle.

A WaveshareESP32-S3-Touch-LCD-3.5 inside the car, for displaying the measuring results.

The project currently contains following sketches:

ArduinoR4Server:
- simulates 4 sensor results and sends them through Wifi. The board serves as dedicated Wifi server. Currently only one client can connect.

ESP32Client:
- simulates Wifi client. Display the received sensor results on Serial Monitor

ESP32ClientDisplay:
- simulates Wifi client. Display the received sensor results on 0.9'' Oled doisplay

For the ESP32: best push reset button while uploading and reconnect physically to test

ESP32TouchClientDisplayText:
- simulates Wifi Client. Displays sensor result as text on the LCD display.
- The WaveshareESP32-S3-Touch-LCD-3.5 is quite delicate, there are several attention points for compilation and connection....

