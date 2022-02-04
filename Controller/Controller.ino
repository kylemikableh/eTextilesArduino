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
#define LOWEST_SLIDER_VALUE 50 //Lowest value threshold for Slider values

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
#define ACTION_TYPE "type"

/**
 * Update types
 */
#define UPDATE "update"
#define UPDATE_LEDS "LEDS"
#define UPDATE_LEDS_VALUES "values"

/**
 * Define slider information
 */

#define SOFT_POT_PIN_1 A0 // Pin connected to softpot wiper
#define SOFT_POT_PIN_2 A1 // Pin connected to softpot wiper

String activeTile;
int activePallet; //0-5 for the palette squares

/**
 * Arduino setup function on powerup.
 */
void setup() 
{
  initalizePins();
  while (!Serial) {;} // Don't do anything unless Serial is active
  
  Serial.begin(115200);
  if(DEBUG) {sendToSite("{\"message\": \"Controller Paired.\"}");}
  Serial.flush();
  activeTile = "NONE";
  activePallet = 0; // Let's initalize the top left palette
                    // 10
                    // 00
                    // 00
}

/**
 * Initalize pins for use
 */
 void initalizePins()
 {
  pinMode(SOFT_POT_PIN_1, INPUT);
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
  //figure out which button is pressed
  bool changeDetected = true;
  
  if(changeDetected) 
  {
    //This is for handling which page we are viewing
    int buttonID = 0; //Set this to button ID detected
    const int capacity = JSON_OBJECT_SIZE(6);
    StaticJsonDocument<capacity> doc;
    doc[ACTION] = ACTION_BUTTON_PRESSED;
    doc[ACTION_ID] = buttonID;
    String output;
    serializeJson(doc, output);
    sendToSite(output);
    delay(200); //Remove for testing only
   }
}

void writeSliderInfo(char *type, int value)
{
  const int capacity = JSON_OBJECT_SIZE(6);
  StaticJsonDocument<capacity> doc;
  doc[ACTION] = ACTION_SLIDER_MOVED;
  doc[ACTION_ID] = value;
  doc[ACTION_TYPE] = String(type);
  String output;
  serializeJson(doc, output);
  sendToSite(output);
}

/**
 * See if the sliders has moved, if so report back to site
 */
void checkForSlider()
{
  // Read in the soft pot's ADC value
  int softPotADC1 = analogRead(SOFT_POT_PIN_1);
  // Map the 0-1023 value to 0-365
  int softPotPosition1 = map(softPotADC1, 0, 1023, 0, 365);

  // Read in the soft pot's ADC value
  int softPotADC2 = analogRead(SOFT_POT_PIN_2);
  // Map the 0-1023 value to 0-255
  int softPotPosition2 = map(softPotADC2, 0, 1023, 0, 365);
  if(softPotPosition1 != NULL && softPotPosition1 >= LOWEST_SLIDER_VALUE) //If it is not zero do something
  {
    writeSliderInfo("S0", softPotPosition1);
  }
  if(softPotPosition2 != NULL && softPotPosition2 >= LOWEST_SLIDER_VALUE) //If it is not zero do something
  {
    writeSliderInfo("S1", softPotPosition2);
  }
  //For demonstration purposes
  for(int i = 0; i < 355; i++) 
  {
    writeSliderInfo("S1", i);
    delay(100);
  }
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
 * See if a palette change
 */
void checkForPaletteChange()
{
  //Do the checking here, check with teammates, button 0-5 is the palette buttons
  bool changeDetected = true;
  if(changeDetected) 
  {
    int buttonID = 0; //Set this to button ID detected
    activePallet = buttonID;
    const int capacity = JSON_OBJECT_SIZE(6);
    StaticJsonDocument<capacity> doc;
    doc[ACTION] = ACTION_BUTTON_PRESSED;
    doc[ACTION_ID] = buttonID;
    String output;
    serializeJson(doc, output);
    sendToSite(output);
    delay(200); //Remove for testing only
   }
}


/**
 * Check for all user inputs on the controller
 */
void checkForAnyUserInput()
{
  checkForButtonPress();
  checkForPaletteChange();
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
