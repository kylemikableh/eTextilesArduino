/** Delete this shit and we are screwed */
#include <WebUSB.h>
WebUSB WebUSBSerial(1 /* https:// */, "webusb.github.io/arduino/demos/rgb");
//#define Serial WebUSBSerial
/** Delete this shit and we are screwed */
#include <Adafruit_NeoPixel.h>
#include "FastLED.h"
#include "LEDMatrix.h"

#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
#define MATRIX_TYPE HORIZONTAL_ZIGZAG_MATRIX

#define CHIPSET WS2812B
#define LED_PIN 8
#define COLOR_ORDER GRB

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
void setup() {
  // put your setup code here, to run once:

FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());

  // put your main code here, to run repeatedly:
  FastLED.clear();

//Bar
//  for(int i = 0; i < MATRIX_WIDTH; i++) {
//    for(int j = 0; j < 4; j++) {
//      leds(i, j) = CHSV(150, 255, 50);
//    }
//  }

//Full

//  for(int i = 0; i < MATRIX_WIDTH; i++) {
//    for(int j = 0; j < MATRIX_HEIGHT; j++) {
//      leds(i, j) = CHSV(150, 255, 50);
//    }
//  }

//Diagonal

  for(int i = 0; i < MATRIX_WIDTH; i++) {
    for(int j = 0; j < i; j++) {
      leds(i, j) = CHSV(150, 255, 50);
    }
  }

  
  FastLED.show();
}

void loop() {


}
