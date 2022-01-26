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

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());

  // put your main code here, to run repeatedly:
  FastLED.clear();
  leds[0] = CRGB::Purple; FastLED.show(); delay(30);
  //leds(1, 1) = CHSV(255, 255, 255);
  FastLED.show();
}

void loop() {


}
