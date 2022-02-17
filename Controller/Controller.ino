/** DO NOT REMOVE OR BREAK ME */
#include <WebUSB.h>
WebUSB WebUSBSerial(1 /* https:// */, "sever.kylem.org/controller/");
#define Serial WebUSBSerial
/** DO NOT REMOVE OR BREAK ME */

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "FastLED.h"
#include "LEDMatrix.h"
#include "Adafruit_MPR121.h"

/**
 * Touch sensor defines
 */
#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif
#define TOTAL_BUTTONS 12
 
/**
   Matrix Stuff
*/
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
#define MATRIX_TYPE HORIZONTAL_ZIGZAG_MATRIX

#define CHIPSET WS2812B
#define LED_PIN 8
#define COLOR_ORDER RGB

/**
   Constants/Config
*/

#define DEBUG true // Enable or disable debug operation
#define JSON_BUFFER_SIZE 128 // Size of JSON file in bytes
#define CHAR_BUFFER_SIZE 128 // Size of CHAR* allocations in bytes
#define NULL_STRING "null" //String of null JSON might return
#define START_TRANSMISSION_CHAR "#"
#define END_TRANSMISSION_CHAR "$"
#define LOWEST_SLIDER_VALUE 200 //Lowest value threshold for Slider values

/**
   Action types
*/
#define ACTION "action"
#define ACTION_BUTTON_PRESSED "buttonPressed"
#define ACTION_BUTTON_RELEASED "buttonReleased"
//#define ACTION_TILE_REMOVED "tileRemoved"
//#define ACTION_TILE_PLACED "tilePlaced"
#define ACTION_SLIDER_MOVED "sliderMoved"
#define ACTION_ID "ID"
#define ACTION_TYPE "type"

/**
   Update types
*/
#define UPDATE "update"
#define UPDATE_LEDS "LEDS"
#define UPDATE_LEDS_VALUES "values"
#define UPDATE_LEDS_STYLE "style"
#define UPDATE_LEDS_STYLE_FULL "full"
#define UPDATE_LEDS_STYLE_BAR "bar"
#define UPDATE_LEDS_STYLE_DIAGONAL "diagonal"
#define UPDATE_LEDS_STYLE_ISO "iso"
#define UPDATE_LEDS_STYLE_DRUNK "drunk"
#define UPDATE_LEDS_FIRST_COLOR_R "first_color_R"
#define UPDATE_LEDS_FIRST_COLOR_G "first_color_G"
#define UPDATE_LEDS_FIRST_COLOR_B "first_color_B"
#define UPDATE_LEDS_SECOND_COLOR_R "second_color_R"
#define UPDATE_LEDS_SECOND_COLOR_G "second_color_G"
#define UPDATE_LEDS_SECOND_COLOR_B "second_color_B"

/**
   Define slider information
*/

#define SOFT_POT_PIN_1 A0 // Pin connected to slider1
#define SOFT_POT_PIN_2 A1 // Pin connected to slider2


cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
Adafruit_MPR121 cap = Adafruit_MPR121();
boolean foundTouchSensor = false; //Did we find the touch sensor? If not don't try!
uint16_t lastTouchedSensor = 0; //holding values of buttons
uint16_t currTouchedSensor = 0; //holding values of buttons

/**
   Arduino setup function on powerup.
*/
void setup()
{
  initalizePins();
  setupLEDS();
  while (!Serial) 
  {
    ; // Don't do anything unless Serial is active
  }

  Serial.begin(115200);
  if (DEBUG) 
  {
    sendToSite("{\"message\": \"Controller Paired.\"}");
  }
  Serial.flush();

  /**
   * Check for the Capacitive sensor
   */
  if (cap.begin(0x5A)) 
  {
    sendToSite("MPR121 touch sensor found!");
    foundTouchSensor = true;
  }
  else
  {
    sendToSite("MPR121 not found, check wiring?");
  }
}

void setupLEDS()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.clear();
  FastLED.show();
}

/**
   Initalize pins for use
*/
void initalizePins()
{
  pinMode(SOFT_POT_PIN_1, INPUT);
}

/**
   Send any data to site (Adds Start/end character to indicate start/end of a transmission)
   @args data char* data to send over serial
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

unsigned int firstColorG = 0;
unsigned int firstColorR = 0;
unsigned int firstColorB = 0;

unsigned int secondColorG = 0;
unsigned int secondColorR = 0;
unsigned int secondColorB = 0;

/**
   LED Controller
*/
void changeLEDS(DynamicJsonDocument json)
{
  FastLED.clear();
  const char* values = json[UPDATE_LEDS_VALUES];
  const char* style = json[UPDATE_LEDS_STYLE];

  if(json.containsKey(UPDATE_LEDS_FIRST_COLOR_R))
  {
    firstColorG = json[UPDATE_LEDS_FIRST_COLOR_R];
    firstColorR = json[UPDATE_LEDS_FIRST_COLOR_G];
    firstColorB = json[UPDATE_LEDS_FIRST_COLOR_B];
  }
  if(json.containsKey(UPDATE_LEDS_SECOND_COLOR_R))
  {
    secondColorG = json[UPDATE_LEDS_SECOND_COLOR_R];
    secondColorR = json[UPDATE_LEDS_SECOND_COLOR_G];
    secondColorB = json[UPDATE_LEDS_SECOND_COLOR_B];
  }

  char sizeChar[sizeof(unsigned int)];
  itoa(secondColorR, sizeChar, 10); //the 10 stands for base 10
  sendToSite("This is the LED value: ");
  sendToSite(sizeChar); //Make sure we are getting what we want

  if (strcmp(style, UPDATE_LEDS_STYLE_BAR) == 0)
  {
    sendToSite("Style of BAR");
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        leds(i, j) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
      for (int j = 4; j < 8; j++)
      {
        leds(i, j) = CRGB(secondColorR, secondColorG, secondColorB);
      }
    }
  }
  else if (strcmp(style, UPDATE_LEDS_STYLE_FULL) == 0)
  {
    sendToSite("Style of FULL");
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
      for (int j = 0; j < MATRIX_HEIGHT; j++)
      {
        leds(i, j) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
  }

  else if (strcmp(style, UPDATE_LEDS_STYLE_DIAGONAL) == 0)
  {
    sendToSite("Style of DIAG");
    for (int i = 0; i < MATRIX_WIDTH; i++) {
      for (int j = 0; j < i; j++) {
        leds(i, j) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
    for (int i = 0; i < MATRIX_WIDTH - 1; i++) {
      for (int j = i + 1; j < 8; j++) {
        leds(i, j) = CRGB(secondColorR, secondColorG, secondColorB);
      }
    }
  }
  else if (strcmp(style, UPDATE_LEDS_STYLE_DRUNK) == 0)
  {
    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 6; j++)
      {
        leds(i, j) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
    for (int m = 0; m < 4; m++)
    {
      leds(6, m) = CRGB(firstColorR, firstColorG, firstColorB);
      leds(m, 6) = CRGB(firstColorR, firstColorG, firstColorB);
    }
    for (int n = 0; n < 8; n++)
    {
      leds(7, n) = CRGB(secondColorR, secondColorG, secondColorB);
      leds(n, 7) = CRGB(secondColorR, secondColorG, secondColorB);
    }
    for (int i = 4; i < 7; i++)
    {
      leds(6, i) = CRGB(secondColorR, secondColorG, secondColorB);
      leds(i, 6) = CRGB(secondColorR, secondColorG, secondColorB);
      }
  }
  else if (strcmp(style, UPDATE_LEDS_STYLE_ISO) == 0)
  {
    //Isoceles
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
      leds(i, 0) = CRGB(firstColorR, firstColorG, firstColorB);
    }
    for (int k = 1; k < 3; k++)
    {
      for (int j = 1; j < 7; j++)
      {
        leds(j, k) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
    for (int m = 3; m < 5; m++)
    {
      for (int n = 2; n < 6; n++)
      {
        leds(n, m) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
    leds(3, 5) = CRGB(firstColorR, firstColorG, firstColorB);
    leds(4, 5) = CRGB(firstColorR, firstColorG, firstColorB);
    leds(3, 6) = CRGB(firstColorR, firstColorG, firstColorB);
    leds(4, 6) = CRGB(firstColorR, firstColorG, firstColorB);
    leds(3, 7) = CRGB(firstColorR, firstColorG, firstColorB);

    for (int i = 1; i < 8; i++)
    {
      leds(0, i) = CRGB(secondColorR, secondColorG, secondColorB);
      leds(7, i) = CRGB(secondColorR, secondColorG, secondColorB);
    }
    for (int j = 3; j < 8; j++)
    {
      leds(1, j) = CRGB(secondColorR, secondColorG, secondColorB);
      leds(6, j) = CRGB(secondColorR, secondColorG, secondColorB);
    }
    for (int k = 5; k < 8; k++)
    {
      leds(2, k) = CRGB(secondColorR, secondColorG, secondColorB);
      leds(5, k) = CRGB(secondColorR, secondColorG, secondColorB);
    }
    leds(4, 7) = CRGB(secondColorR, secondColorG, secondColorB);
  }
  else
  {
    sendToSite("Type of LED update not given.");
  }
  FastLED.show();
}

/**
   Process update sent to controller
*/
void processUpdate(DynamicJsonDocument json)
{
  const char* updateStr = json[UPDATE];

  if (DEBUG) 
  {
    sendToSite(updateStr);
  }

  if (strcmp(updateStr, NULL_STRING) == 0) 
  {
    return;
  }
  else if (strcmp(updateStr, UPDATE_LEDS) == 0)
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
   Handle the action passed
   @args json JSON from site
*/
void processAction(DynamicJsonDocument json)
{
//  const char* action = json[ACTION];
//  if (DEBUG) 
//  {
//    sendToSite(action);
//  }
//
//  if (strcmp(action, NULL_STRING) == 0) {
//    return; //don't do anything if there was no action key
//  }
//  else if (strcmp(action, ACTION_BUTTON_PRESSED) == 0)
//  {
//
//  }
//  else if (strcmp(action, ACTION_TILE_REMOVED) == 0)
//  {
//
//  }
//
//  //Handle cases here
}

/**
   Get the sent JSON from the site and convert it to JSON type
   @args
   @returns DynamicJsonDocument json
*/
DynamicJsonDocument getJsonFromSite()
{
  String jsonRecieved = Serial.readString();
  char jsonRecievedBuff[JSON_BUFFER_SIZE];
  jsonRecieved.toCharArray(jsonRecievedBuff, jsonRecieved.length() + 3); //the 3 is for the null termininator

  if (DEBUG)
  {
    sendToSite(jsonRecievedBuff);
  }

  DynamicJsonDocument json(JSON_BUFFER_SIZE);
  char* error = deserializeJson(json, jsonRecieved).c_str();
  if (error) //If deserializeJson failed, report this
  {
    sendToSite("{\"message\": \"deserialize error?\"}");
    sendToSite(error);
  }
  return json;
}

/**

*/
void serialAvailable()
{
  if (Serial && Serial.available())
  {
    sendToSite("Made it here");
    DynamicJsonDocument json = getJsonFromSite(); // Let's get the sent JSON
    if (!json.isNull())
    {
      sendToSite("Was not null. Test char size: 1234556778991234456678899012344546547i5i8");
      processAction(json);
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
   Check for all buttons to be pressed and send this to controller
*/
void checkForButtonPress()
{
  if(foundTouchSensor) //If we found the touch sensor we can procede!
  {
      //figure out which button is pressed
    bool touchDetected = false;
    bool releaseDetected = false;
    uint8_t buttonID = 0;
  
    /**
     * Check touch sensor
     */
    currTouchedSensor = cap.touched();
    
    for (uint8_t i=0; i<TOTAL_BUTTONS; i++) 
    {
      // it if *is* touched and *wasnt* touched before, alert!
      if ((currTouchedSensor & _BV(i)) && !(lastTouchedSensor & _BV(i)) ) 
      {
//        char touchID[sizeof(int)];
//        itoa(i, touchID, 10); //10 is the base
//        sendToSite("Touch was detected! Waiting on release of: ");
//        sendToSite(touchID);
        touchDetected = true;
        buttonID = i;
      }
      else if (!(currTouchedSensor & _BV(i)) && (lastTouchedSensor & _BV(i)) ) 
      {
//        char touchID[sizeof(int)];
//        itoa(i, touchID, 10); //10 is the base
//        sendToSite("Release was detected! Waiting on release of: ");
//        sendToSite(touchID);
        releaseDetected = true;
        buttonID = i;
      }

      if(touchDetected || releaseDetected)
      {
        //This is for handling which page we are viewing
        const int capacity = JSON_OBJECT_SIZE(6);
        StaticJsonDocument<capacity> doc;
        doc[ACTION_ID] = buttonID;
        if(touchDetected)
        {
          doc[ACTION] = ACTION_BUTTON_PRESSED;
        }
        else if(releaseDetected)
        {
          doc[ACTION] = ACTION_BUTTON_RELEASED;
        }
        char* output = malloc(CHAR_BUFFER_SIZE);
        serializeJson(doc, output, CHAR_BUFFER_SIZE);
        sendToSite(output);
        free(output);
      }
    }
    lastTouchedSensor = currTouchedSensor;
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
   See if the sliders has moved, if so report back to site
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
  if (softPotPosition1 != NULL && softPotPosition1 >= LOWEST_SLIDER_VALUE) //If it is not zero do something
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
   See if a palette change
*/
void checkForPaletteChange()
{
  //Do the checking here, check with teammates, button 0-5 is the palette buttons
  bool changeDetected = false;
  if (changeDetected)
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
   Check for all user inputs on the controller
*/
void checkForAnyUserInput()
{
  checkForButtonPress();
  checkForPaletteChange();
  checkForSlider();
}

/**
   Arduino loop function
*/
void loop()
{
  serialAvailable();
}
