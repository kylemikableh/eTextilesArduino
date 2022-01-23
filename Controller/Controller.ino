/** DO NOT REMOVE OR BREAK ME */
#include <WebUSB.h>
WebUSB WebUSBSerial(1 /* http:// */, "localhost:4000/controller/");
#define Serial WebUSBSerial
/** DO NOT REMOVE OR BREAK ME */

#include <ArduinoJson.h>

/**
 * Constants/Config
 */

#define DEBUG true // Enable or disable debug operation
#define JSON_BUFFER_SIZE 1024

/**
 * Arduino setup function on powerup.
 */
void setup() {
  while (!Serial) {;} // Don't do anything unless Serial is active
  
  Serial.begin(115200);
  if(DEBUG) {Serial.println("Controller Paired.");}
  Serial.flush();
}

/**
 * Get the sent JSON from the site and convert it to JSON type
 * @args 
 * @returns DynamicJsonDocument json
 */
DynamicJsonDocument getJsonFromSite() {
  String jsonRecieved = Serial.readString();
  if(DEBUG) {sendToSite(jsonRecieved);}
  
  DynamicJsonDocument json(1024);
  deserializeJson(json, jsonRecieved);
  return json;
}

/**
 * Send any data to site (Adds Start/end character to indicate start/end of a transmission)
 * @args data String data to send
 */
void sendToSite(String data) {
  String toSend = "#" + data + "$"; // Start and end markers to help process the JSON on the controller (it recieves in chunks of data)
  char buff[JSON_BUFFER_SIZE];
  toSend.toCharArray(buff,JSON_BUFFER_SIZE);
  Serial.write(buff);
  Serial.flush();
}

void serialAvailable() {
  if (Serial && Serial.available()) {
    DynamicJsonDocument json = getJsonFromSite(); // Let's get the sent JSON
    
    String action = json["action"];
    String kyle = json["kyle"];

    if(DEBUG) {
      sendToSite(action);
    }
    Serial.flush();
  }
}

/**
 * Arduino loop function
 */
void loop() {
  serialAvailable();
}
