/*
* main.cpp
* 
* alisa yurevich, ee14nspring 2025
*/

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h> 
const char* ssid = "ESP32-Car"; // wifi discoverable name
const char* password = "12345678"; // wifi password

// listens for incoming client connections @port 80
// note: on wifi, esp32 draws 200mA and needs a high impedance external battery
WiFiServer server(80); 
#define I2C_DEV_ADDR 0x0F // stm32 target address

/*
* name: i2c_setup
* 
*/
void i2c_setup() {
  Wire.begin(18, 19, 400000); // set up esp 32 i2c
  Wire.beginTransmission(I2C_DEV_ADDR); // address match check
  Wire.write(1); 
  Wire.endTransmission(true);
}

/*
* name: Wifi_setup
* 
*/
void Wifi_setup() {
  WiFi.softAP(ssid, password); // setup soft wifi access point 
  Serial.println(WiFi.softAPIP()); 
  server.begin(); // 
}

/*
* name: setup
* 
*/
void setup() {
  Serial.begin(115200); // set up baudrate -> standard 115200
  delay(2000);
  i2c_setup(); 
  Wifi_setup();
}

/*
* name: sendToSTM32
* 
*/
void sendToSTM32(const char* command) {
  Wire.beginTransmission(I2C_DEV_ADDR); 
  Wire.write(command);
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.print("successfully sent: ");
    Serial.println(command);
  } else {
    Serial.print("I2C error: ");
    Serial.println(error);
  }
}

void loop() {
  WiFiClient client = server.available(); //checks who @port 80

  if (client) { //if not null client
    while (client.connected()) {
      if (client.available()) { //keep checking for data
        String command = client.readStringUntil('\n'); //
        Serial.println("received: " + command); //check
        sendToSTM32(command.c_str());
      } 
    }
    Serial.println("client disconnected");
    client.stop(); 
   }
}
