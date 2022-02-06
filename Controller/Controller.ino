/** DO NOT REMOVE OR BREAK ME */
#include <WebUSB.h>
WebUSB WebUSBSerial(1 /* https:// */, "sever.kylem.org/controller/");
#define Serial WebUSBSerial
/** DO NOT REMOVE OR BREAK ME */

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "FastLED.h"
#include "LEDMatrix.h"

/**
 * Matrix Stuff
 */
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
#define MATRIX_TYPE HORIZONTAL_ZIGZAG_MATRIX

#define CHIPSET WS2812B
#define LED_PIN 8
#define COLOR_ORDER GRB

/**
 * Constants/Config
 */

#define DEBUG true // Enable or disable debug operation
#define JSON_BUFFER_SIZE 128 // Size of JSON file in bytes
#define CHAR_BUFFER_SIZE 128 // Size of CHAR* allocations in bytes
#define NULL_STRING "null" //String of null JSON might return
#define START_TRANSMISSION_CHAR "#"
#define END_TRANSMISSION_CHAR "$"
#define LOWEST_SLIDER_VALUE 100 //Lowest value threshold for Slider values

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
#define UPDATE_LEDS_STYLE "style"
#define UPDATE_LEDS_STYLE_FULL "full"
#define UPDATE_LEDS_STYLE_BAR "bar"
#define UPDATE_LEDS_STYLE_DIAGONAL "diagonal"
#define UPDATE_LEDS_FIRST_COLOR "first_color" // 8-digit string
#define UPDATE_LEDS_SECOND_COLOR "second_color"

/**
 * Define slider information
 */

#define SOFT_POT_PIN_1 A0 // Pin connected to softpot wiper
#define SOFT_POT_PIN_2 A1 // Pin connected to softpot wiper


cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

/**
 * Arduino setup function on powerup.
 */
void setup() 
{
  initalizePins();
  setupLEDS();
  while (!Serial) {;} // Don't do anything unless Serial is active
  
  Serial.begin(115200);
  if(DEBUG) {sendToSite("{\"message\": \"Controller Paired.\"}");}
  Serial.flush();
}

void setupLEDS()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.clear();
}

/**
 * Initalize pins for use
 */
 void initalizePins()
 {
  pinMode(SOFT_POT_PIN_1, INPUT);
 }

/**
 * Send any data to site (Adds Start/end character to indicate start/end of a transmission)
 * @args data char* data to send over serial
 */
void sendToSite(char* data) 
{
  const char* startChar = START_TRANSMISSION_CHAR;
  const char* endChar = END_TRANSMISSION_CHAR;
  const char* nullTerminator = "\0";
  char* toSend = malloc(2 + CHAR_BUFFER_SIZE); //Allocate memory for new string. Size probably needs to change
  strcpy(toSend, startChar);
  strcat(toSend, data);
  strcat(toSend, endChar);
  strcat(toSend, nullTerminator);
  Serial.write(toSend);
  Serial.flush();
  free(toSend); //Don't want no memory leaks!
}

/**
 * LED Controller
 */
 void changeLEDS(DynamicJsonDocument json)
 {
    const char* values = json[UPDATE_LEDS_VALUES];
    const char* style = json[UPDATE_LEDS_STYLE]; 
    int firstColor = json[UPDATE_LEDS_FIRST_COLOR];
    int secondColor = json[UPDATE_LEDS_SECOND_COLOR];
    sendToSite(style);
    if(strcmp(style, UPDATE_LEDS_STYLE_BAR) == 0)
    {
      sendToSite("Style of BAR");
      for(int i = 0; i < MATRIX_WIDTH; i++) 
      {
        for(int j = 0; j < 4; j++) 
        {
          leds(i, j) = CHSV(150, 255, 50);
        }
      }
    }
    else if(strcmp(style, UPDATE_LEDS_STYLE_FULL) == 0)
    {
      sendToSite("Style of FULL");
      for(int i = 0; i < MATRIX_WIDTH; i++) 
      {
        for(int j = 0; j < MATRIX_HEIGHT; j++) 
        {
          leds(i, j) = CHSV(150, 255, 50);
        }
      }
    }
    
    else if(strcmp(style, UPDATE_LEDS_STYLE_DIAGONAL) == 0)
    {
      sendToSite("Style of DIAG");
      for(int i = 0; i < MATRIX_WIDTH; i++) {
        for(int j = 0; j < i; j++) {
          leds(i, j) = CHSV(150, 255, 50);
        }
      }
    }
    else
    {
      sendToSite("Type of LED update not given.");     
    }
    FastLED.show();
 }

/**
 * Process update sent to controller
 */
void processUpdate(DynamicJsonDocument json)
{
  sendToSite("Processing update.");
  
//  char sizeChar[8]; 
//  itoa(json.size(),sizeChar,8);
//  sendToSite(sizeChar);
  
  const char* updateStr = json[UPDATE];
  if(DEBUG){sendToSite(updateStr);}
  if(strcmp(updateStr,NULL_STRING) == 0) {return;}
  else if (strcmp(updateStr,UPDATE_LEDS) == 0) 
  {
    //Handle LED update
    changeLEDS(json);
  }
  else
  {
    sendToSite("Update not supported");
    sendToSite(updateStr);
    //Update not supported
  }
}

/**
 * Handle the action passed
 * @args json JSON from site
 */
void processAction(DynamicJsonDocument json) 
{
  const char* action = json[ACTION];
  if(DEBUG){sendToSite(action);}
  
  if(strcmp(action,NULL_STRING) == 0) {return;} //don't do anything if there was no action key
  else if (strcmp(action,ACTION_BUTTON_PRESSED) == 0)
  {
    
  }
  else if (strcmp(action,ACTION_TILE_REMOVED) == 0)
  {
    
  }
  
  //Handle cases here
}

/**
 * Get the sent JSON from the site and convert it to JSON type
 * @args 
 * @returns DynamicJsonDocument json
 */
DynamicJsonDocument getJsonFromSite() 
{
  String jsonRecieved = Serial.readString();
  char jsonRecievedBuff[JSON_BUFFER_SIZE];
  jsonRecieved.toCharArray(jsonRecievedBuff,jsonRecieved.length()+3); //the 3 is for the null termininator
  
  if(DEBUG) 
  {
    sendToSite(jsonRecievedBuff);
  }
  
  DynamicJsonDocument json(JSON_BUFFER_SIZE);
  char* error = deserializeJson(json, jsonRecieved).c_str();
//  deserializeJson(json, jsonRecieved);
  if(error)//If deserializeJson failed, report this
  {
    sendToSite("{\"message\": \"deserialize error?\"}");
    sendToSite(error);
  }
  return json;
}

/**
 * 
 */
void serialAvailable() 
{
  if (Serial && Serial.available()) 
  {
    sendToSite("Made it here");
    DynamicJsonDocument json = getJsonFromSite(); // Let's get the sent JSON
    if(!json.isNull())
    {
      sendToSite("Was not null. Test char size: 1234556778991234456678899012344546547i5i8");
      //processAction(json);
      processUpdate(json);
    }
    else
    {
      sendToSite("JSON decode failed");
    }
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
  bool changeDetected = false;
  
  if(changeDetected) 
  {
    //This is for handling which page we are viewing
    char buttonID = 0; //Set this to button ID detected
    const int capacity = JSON_OBJECT_SIZE(6);
    StaticJsonDocument<capacity> doc;
    doc[ACTION] = ACTION_BUTTON_PRESSED;
    doc[ACTION_ID] = buttonID;
    char* output = malloc(CHAR_BUFFER_SIZE);
    serializeJson(doc, output, CHAR_BUFFER_SIZE);
    sendToSite(output);
    free(output);
   }
}

void writeSliderInfo(char *type, int value)
{
  const int capacity = JSON_OBJECT_SIZE(6);
  StaticJsonDocument<capacity> doc;
  doc[ACTION] = ACTION_SLIDER_MOVED;
  doc[ACTION_ID] = value;
  doc[ACTION_TYPE] = type;
  char* output = malloc(CHAR_BUFFER_SIZE);
  serializeJson(doc, output, CHAR_BUFFER_SIZE);
  sendToSite(output);
  free(output);
}

/**
 * See if the sliders has moved, if so report back to site
 */
void checkForSlider()
{
  // Read in the soft pot's ADC value
  int softPotADC1 = analogRead(SOFT_POT_PIN_1);
  // Map the 0-1023 value to 0-365
  int softPotPosition1 = map(softPotADC1, 0, 1023, 1, 365);

  // Read in the soft pot's ADC value
  int softPotADC2 = analogRead(SOFT_POT_PIN_2);
  // Map the 0-1023 value to 0-255
  int softPotPosition2 = map(softPotADC2, 0, 1023, 1, 365);
  if(softPotPosition1 != NULL && softPotPosition1 >= LOWEST_SLIDER_VALUE) //If it is not zero do something
  {
    writeSliderInfo("S0", softPotPosition1);
  }
//  if(softPotPosition2 != NULL && softPotPosition2 >= LOWEST_SLIDER_VALUE) //If it is not zero do something
//  {
//    writeSliderInfo("S1", softPotPosition2);
//  }
  //For demonstration purposes
//  for(int i = 0; i < 355; i++) 
//  {
//    writeSliderInfo("S1", i);
//    delay(100);
//  }
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
  bool changeDetected = false;
  if(changeDetected) 
  {
    char buttonID = 0; //Set this to button ID detected
    const int capacity = JSON_OBJECT_SIZE(6);
    StaticJsonDocument<capacity> doc;
    doc[ACTION] = ACTION_BUTTON_PRESSED;
    doc[ACTION_ID] = buttonID;
    char* output = malloc(CHAR_BUFFER_SIZE);
    serializeJson(doc, output, CHAR_BUFFER_SIZE);
    sendToSite(output);
    free(output);
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
