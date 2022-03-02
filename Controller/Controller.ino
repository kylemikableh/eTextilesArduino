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
#define SERIAL_SPEED 115200
#define JSON_BUFFER_SIZE 120 // Size of JSON file in bytes
#define CHAR_BUFFER_SIZE 120 // Size of CHAR* allocations in bytes
#define JSON_OBJ_CAPACITY JSON_OBJECT_SIZE(6) // JSON Object sizes for sending
#define NULL_STRING "null" //String of null JSON might return
#define START_TRANSMISSION_CHAR "#"
#define END_TRANSMISSION_CHAR "$"
#define LOWEST_SLIDER_VALUE 0 //Lowest value threshold for Slider values

/**
   Action types
*/
#define ACTION "action"
#define ACTION_BUTTON_PRESSED "buttonPressed"
#define ACTION_BUTTON_RELEASED "buttonReleased"
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
#define UPDATE_LEDS_STYLE_DIAGONAL "dia"
#define UPDATE_LEDS_STYLE_ISO "iso"
#define UPDATE_LEDS_STYLE_DRUNK "drunk"
#define UPDATE_LEDS_FIRST_COLOR_R "f_c_R"
#define UPDATE_LEDS_FIRST_COLOR_G "f_c_G"
#define UPDATE_LEDS_FIRST_COLOR_B "f_c_B"
#define UPDATE_LEDS_SECOND_COLOR_R "s_c_R"
#define UPDATE_LEDS_SECOND_COLOR_G "s_c_G"
#define UPDATE_LEDS_SECOND_COLOR_B "s_c_B"

/**
   Define slider information
*/

#define SOFT_POT_PIN A0 // Pin connected to slider
#define SLIDER_PAUSE_INTERVAL 100
#define DIFFERENCE_DETECT_INTERVAL 3 //At what point would we call a change in slider value a change?


cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
Adafruit_MPR121 cap = Adafruit_MPR121();
boolean foundTouchSensor = false; //Did we find the touch sensor? If not don't try!
uint16_t lastTouchedSensor = 0; //holding values of buttons
uint16_t currTouchedSensor = 0; //holding values of buttons
unsigned long lastSliderUpdateTime = 0; // Hold last EPOCH time the sider was updated
unsigned int lastSliderValue = 0; // Hold the last value the slider read

// Hold onto the color values for the first color LED (0-355)
unsigned int firstColorG = 0;
unsigned int firstColorR = 0;
unsigned int firstColorB = 0;

// Hold onto the color values for the second color LED (0-355)
unsigned int secondColorG = 0;
unsigned int secondColorR = 0;
unsigned int secondColorB = 0;

/**
   Arduino setup function on powerup.
*/
void setup()
{
  initalizePins(); // Initalize the pins for reading data
  setupLEDS(); // Setup the LEDS for use
  while (!Serial) { ; } // Don't do anything unless Serial is active
  Serial.begin(SERIAL_SPEED);
  if (DEBUG) { sendToSite("{\"message\": \"Controller Paired.\"}"); }
  Serial.flush();

  // Check for the Capacitive sensor
  if (cap.begin(0x5A)) 
  {
    foundTouchSensor = true;
  }
  else
  {
    sendToSite("MPR121 not found, check wiring?");
  }
}

/**
 * Set the LED parameters and reset the display when connecting
 */
void setupLEDS()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size()); // Define the LEDS 
  FastLED.clear();
  FastLED.show();
}

/**
   Initalize pins for use
*/
void initalizePins()
{
  pinMode(SOFT_POT_PIN, INPUT); // Set the Slider analogue read pin
}

/**
   Send any data to site (Adds Start/end character to indicate start/end of a transmission)
   @args data char* data to send over serial
*/
void sendToSite(char* data)
{
  const char* startChar = START_TRANSMISSION_CHAR; // Define the start char
  const char* endChar = END_TRANSMISSION_CHAR; // Define the end char
  const char* nullTerminator = "\0"; // Define the nullTerminator for ending
  char* toSend = malloc(2 + CHAR_BUFFER_SIZE); // Allocate memory for new string. Size probably needs to change
  strcpy(toSend, startChar); // Add the start char to char buffer
  strcat(toSend, data); // Add the data to the char buffer
  strcat(toSend, endChar); // Add the end char to the char buffer
  strcat(toSend, nullTerminator); // Add the null terminator to the char buffer
  Serial.write(toSend); // Send the data
  Serial.flush(); // Reset the serial 
  free(toSend); // Don't want no memory leaks! Free the memory of the char buffer
}

/**
 * Send JSON object to site, handling allocation and freeing of SRAM
 */
void allocateAndSend(StaticJsonDocument<JSON_OBJ_CAPACITY> doc)
{
  char* output = malloc(CHAR_BUFFER_SIZE); // Allocate char buffer for data to send
  serializeJson(doc, output, CHAR_BUFFER_SIZE); // Write the serialized Json data to the data buffer
  sendToSite(output); // Send the data to the site
  free(output); // No memory leaks! Free output buffer
}

/**
   LED Controller
*/
void changeLEDS(DynamicJsonDocument json)
{
  FastLED.clear(); //Reset the LEDs
  const char* values = json[UPDATE_LEDS_VALUES]; // Get pointer to value char* array
  const char* style = json[UPDATE_LEDS_STYLE]; // Get pointer to style char* array

  // Check to see if we are updating first or second color
  if(json.containsKey(UPDATE_LEDS_FIRST_COLOR_R))
  {
    firstColorG = json[UPDATE_LEDS_FIRST_COLOR_R];
    firstColorR = json[UPDATE_LEDS_FIRST_COLOR_G];
    firstColorB = json[UPDATE_LEDS_FIRST_COLOR_B];
  }
  // Check to see if we are updating first or second color
  if(json.containsKey(UPDATE_LEDS_SECOND_COLOR_R))
  {
    secondColorG = json[UPDATE_LEDS_SECOND_COLOR_R];
    secondColorR = json[UPDATE_LEDS_SECOND_COLOR_G];
    secondColorB = json[UPDATE_LEDS_SECOND_COLOR_B];
  }

//  char sizeChar[sizeof(unsigned int)]; 
//  itoa(secondColorR, sizeChar, 10); //the 10 stands for base 10
//  sendToSite("This is the LED value: ");
//  sendToSite(sizeChar); //Make sure we are getting what we want

  if (strcmp(style, UPDATE_LEDS_STYLE_BAR) == 0) // Is the style BAR?
  {
    sendToSite("Style of BAR");
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        leds(i, j) = CRGB(secondColorR, secondColorG, secondColorB);
      }
    }
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
      for (int j = 4; j < 8; j++)
      {
        leds(i, j) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
  }
  else if (strcmp(style, UPDATE_LEDS_STYLE_FULL) == 0) // Is the style FULL/SQUARE?
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

  else if (strcmp(style, UPDATE_LEDS_STYLE_DIAGONAL) == 0) // Is the style DIAGONAL?
  {
    sendToSite("Style of DIAG");
    for (int i = 0; i < MATRIX_WIDTH; i++) {
      for (int j = 0; j < 7 - i; j++) {
        leds(i, j) = CRGB(secondColorR, secondColorG, secondColorB);
      }
    }
    for (int i = 0; i < MATRIX_WIDTH; i++) {
      for (int j = 8 - i; j < 8; j++) {
        leds(i, j) = CRGB(firstColorR, firstColorG, firstColorB);
      }
    }
  }
  else if (strcmp(style, UPDATE_LEDS_STYLE_DRUNK) == 0) // Is the style DRUNKARDS?
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
  else if (strcmp(style, UPDATE_LEDS_STYLE_ISO) == 0) // Is the style ISOCELES TRIANGLE?
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
  const char* updateStr = json[UPDATE]; // Get pointer to the update char* array

  if (DEBUG) 
  {
    sendToSite(updateStr);
  }

  // If the update is null don't do anything
  if (strcmp(updateStr, NULL_STRING) == 0) 
  {
    return;
  }
  else if (strcmp(updateStr, UPDATE_LEDS) == 0) // If the update is for LEDS handle it
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
   Get the sent JSON from the site and convert it to JSON type
   @args
   @returns DynamicJsonDocument json
*/
DynamicJsonDocument getJsonFromSite()
{
  String jsonRecieved = Serial.readString(); // Get the string sent from site (Had to be String data type, tried to avoid it but this seems to be the only way)
  char jsonRecievedBuff[JSON_BUFFER_SIZE]; // Hold the char* array of the JSON recieved
  jsonRecieved.toCharArray(jsonRecievedBuff, jsonRecieved.length() + 3); //the 3 is for the null termininator

  if (DEBUG)
  {
    sendToSite(jsonRecievedBuff);
  }

  DynamicJsonDocument json(JSON_BUFFER_SIZE); // This is the JSON document we recieved that will be passed around
  char* error = deserializeJson(json, jsonRecieved).c_str(); // Deserialize the json and read the error char* array if it exists
  if (error) //If deserializeJson failed, report this
  {
    sendToSite(error);
  }
  return json; // Return the JSON
}

/**
* Loop when serial was found, and check for user inputs
*/
void serialAvailable()
{
  if (Serial && Serial.available())
  {
    DynamicJsonDocument json = getJsonFromSite(); // Let's get the sent JSON
    if (!json.isNull())
    {
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
  
    currTouchedSensor = cap.touched(); // Check touch sensor

    // Loop over all buttons to see which one was pressed
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

      //If a touch or release was found we need to do an action
      if(touchDetected || releaseDetected)
      {
        //This is for handling which page we are viewing
        StaticJsonDocument<JSON_OBJ_CAPACITY> doc;
        doc[ACTION_ID] = buttonID;
        if(touchDetected) // Which action type was this?
        {
          doc[ACTION] = ACTION_BUTTON_PRESSED;
        }
        else if(releaseDetected)
        {
          doc[ACTION] = ACTION_BUTTON_RELEASED;
        }
        allocateAndSend(doc); // Send this new found information over to the site
      }
    }
    lastTouchedSensor = currTouchedSensor; // Change which sensor was last touched
  }
}

/**
 * Send the slider action to the site
 */
void writeSliderInfo(char *type, int value)
{
  StaticJsonDocument<JSON_OBJ_CAPACITY> doc; // Create the Static JSON Document with allocation size
  doc[ACTION] = ACTION_SLIDER_MOVED; // Set the action to Slider
  doc[ACTION_ID] = value; // Set the ID to the read Analogue value
  doc[ACTION_TYPE] = type; // Set the type to the type
  allocateAndSend(doc); // Send this over to the site
}

/**
   See if the sliders has moved, if so report back to site
*/
void checkForSlider()
{
  int softPotADC = analogRead(SOFT_POT_PIN); // Read the value from the slider
  int softPotPosition = map(softPotADC, 0, 1023, 1, 365); // Map the 0-1023 value to 0-365
  
  if (softPotPosition != NULL && softPotPosition >= LOWEST_SLIDER_VALUE) //If it is not zero do something
  {
    unsigned long currentTime = millis();
    if (currentTime - lastSliderUpdateTime > SLIDER_PAUSE_INTERVAL)
    {
      lastSliderUpdateTime = currentTime;
      int difBeforeAbs = softPotPosition - lastSliderValue;
      int absOfDif = abs(difBeforeAbs);
      if(absOfDif > DIFFERENCE_DETECT_INTERVAL) // If the time difference is greater than the amount to detect
      {
        //A (large) change was detected!
        writeSliderInfo("S0", softPotPosition);
      }
      lastSliderValue = softPotPosition; // Set the last slider value to the current value
    }
  }
}

/**
   See if a palette change (Might need to remove and consolidate)
*/
void checkForPaletteChange()
{
  //Do the checking here, check with teammates, button 0-5 is the palette buttons
  bool changeDetected = false; // Was there a change detected?
  if (changeDetected)
  {
    char buttonID = 0; //Set this to button ID detected
    StaticJsonDocument<JSON_OBJ_CAPACITY> doc;
    doc[ACTION] = ACTION_BUTTON_PRESSED;
    doc[ACTION_ID] = buttonID;
    allocateAndSend(doc);
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
