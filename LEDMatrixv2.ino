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

//Isoceles
  for (int i = 0; i < MATRIX_WIDTH; i++)
    {
      leds(i,0) = CHSV(150, 255, 50);
      }
  for (int k = 1; k < 3; k++)
    {
      for (int j = 1; j < 7; j++)
        {
        leds(j,k) = CHSV(150, 255, 50);
          }
      }
  for (int m = 3; m < 5; m++)
    {
      for (int n = 2; n < 6; n++)
        {
          leds(n,m) = CHSV(150, 255, 50);
          }
      }
  leds(3,5) = CHSV(150, 255, 50);
  leds(4,5) = CHSV(150, 255, 50);
  leds(3,6) = CHSV(150, 255, 50);
  leds(4,6) = CHSV(150, 255, 50);
  leds(3,7) = CHSV(150, 255, 50);

//Drunkard's
  for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 6; j++)
        {
          leds(i,j) = CHSV(150, 255, 50);
          }
      }
  for (int m = 0; m < 5; m++)
    {
      leds(6,m) = CHSV(150, 255, 50);
      leds(m,6) = CHSV(150, 255, 50);
      }
  for (int n = 0; n < 2; n++)
    {
      leds(7,n) = CHSV(150, 255, 50);
      leds(n,7) = CHSV(150, 255, 50);
      }
  

  
  FastLED.show();
}

void loop() {


}
