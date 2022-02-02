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
#define JSON_BUFFER_SIZE 1024 // Size of JSON file in bytes
#define NULL_STRING "null" //String of null JSON might return
#define START_TRANSMISSION_CHAR "#"
#define END_TRANSMISSION_CHAR "$"

/**
 * Action types
 */
#define ACTION "action"
#define ACTION_BUTTON_PRESSED "buttonPressed"
#define ACTION_TILE_REMOVED "tileRemoved"
#define ACTION_TILE_PLACED "tilePlaced"
#define ACTION_SLIDER_MOVED "sliderMoved"
#define ACTION_ID "ID"
#define ACTION_ID_BUTTON "B"
#define ACTION_ID_TILE "T"
#define ACTION_ID_SLIDER "S"

/**
 * Update types
 */
#define UPDATE "update"
#define UPDATE_LEDS "LEDS"
#define UPDATE_LEDS_VALUES "values"

String active_tile;

/**
 * Arduino setup function on powerup.
 */
void setup() 
{
  while (!Serial) {;} // Don't do anything unless Serial is active
  
  Serial.begin(115200);
  if(DEBUG) {sendToSite("{\"message\": \"Controller Paired.\"}");}
  Serial.flush();
  active_tile = "NONE";
}

/**
 * Get the sent JSON from the site and convert it to JSON type
 * @args 
 * @returns DynamicJsonDocument json
 */
DynamicJsonDocument getJsonFromSite() 
{
  String jsonRecieved = Serial.readString();
  if(DEBUG) 
  {
    sendToSite(jsonRecieved);
  }
  
  DynamicJsonDocument json(1024);
  deserializeJson(json, jsonRecieved);
  return json;
}

/**
 * Send any data to site (Adds Start/end character to indicate start/end of a transmission)
 * @args data String data to send
 */
void sendToSite(String data) 
{
  String toSend = START_TRANSMISSION_CHAR + data + END_TRANSMISSION_CHAR; // Start and end markers to help process the JSON on the controller (it recieves in chunks of data)
  char buff[JSON_BUFFER_SIZE];
  toSend.toCharArray(buff,JSON_BUFFER_SIZE);
  Serial.write(buff);
  Serial.flush();
}

/**
 * LED Controller
 */
 void changeLEDS(DynamicJsonDocument json)
 {
    String values = json[UPDATE_LEDS_VALUES];
    sendToSite(values); //We parrot what we got
 }

/**
 * Process update sent to controller
 */
void processUpdate(DynamicJsonDocument json)
{
  String update = json[UPDATE];
  if(DEBUG){sendToSite(update);}

  if(update.equalsIgnoreCase(NULL_STRING)) {return;}
  else if (update.equalsIgnoreCase(UPDATE_LEDS)) 
  {
    //Handle LED update
    changeLEDS(json);
  }
  else
  {
    sendToSite("Update passed not supported, " + update);
  }
}

/**
 * Handle the action passed
 * @args json JSON from site
 */
void processAction(DynamicJsonDocument json) 
{
  String action = json[ACTION];
  if(DEBUG){sendToSite(action);}
  
  if(action.equalsIgnoreCase(NULL_STRING)) {return;} //don't do anything if there was no action key
  else if (action.equalsIgnoreCase(ACTION_BUTTON_PRESSED))
  {
    
  }
  else if (action.equalsIgnoreCase(ACTION_TILE_REMOVED))
  {
    
  }
  
  //Handle cases here
}

/**
 * 
 */
void serialAvailable() 
{
  if (Serial && Serial.available()) 
  {
    DynamicJsonDocument json = getJsonFromSite(); // Let's get the sent JSON
    
    processAction(json);
    processUpdate(json);
    Serial.flush();
  }
  checkForAnyUserInput();
}

/**
 * Check for all buttons to be pressed and send this to controller
 */
void checkForButtonPress()
{
  
}

/**
 * See if the slider has moved, if so report back to site
 */
void checkForSlider()
{
  
}

/**
 * See if a tile was removed, if so store this information and report
 */
void checkForTileRemoval()
{
  
}

/**
 * See if a tile was placed, if so act accordingly
 */
void checkForTilePlacement()
{
  
}

/**
 * Check for all user inputs on the controller
 */
void checkForAnyUserInput()
{
  checkForButtonPress();
  checkForSlider();
  checkForTileRemoval();
  checkForTilePlacement();
}

/**
 * Arduino loop function
 */
void loop() 
{
  serialAvailable();
}
