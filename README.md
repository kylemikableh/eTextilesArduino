# eTextiles Arduino Web Controller

Welcome to the eTextiles Web Controller source code. This project is entirly written in the Arduino Programming language, which is primarially a C/C++ language with libraries for the Arduino. 

# What does this do?

## Actions

The controller is designed to detect actions on the controller board, such as a button press or slider move event. If this is detected, through an Adafruit touch sensor or Adafruit soft pot, the controller places the data into a format that the connected website can use, JSON. Using the ArduinoJson library, the now parsed JSON is converted into a char array that is then sent to the website over a serial connection.

## Updates

The controller is also designed to recieve updates from the website, telling it to update something on the controller, such as a Neopixel Matrix. This works similarly to the actions part, accept the Arduino is always listening for an update JSON packet to be sent through Serial (using the WebUSB standard with the Arduino Web USB project). When it detects an update, the code deserializes the String into a JSON, and from there determines the update type and what to do to the controller as a result. 

# Requirements

To use this code, there are some requirements that are needed. 
1) An Arduino with a USB chip, such as an Arduino Leonardo (Arduinos such as the UNO will not work)
2) [Arduino IDE](https://www.arduino.cc/en/software/)
3) Required libraries: ArduinoJson, Adafruit_NeoPixel, Adafruit_MPR121, FastLED, LEDMatrix, WebUSB
4) When using the WebUSB, as part of the installation instructions there is a critical step that is required for the Arduino to be recognised on Windows devices. You must change the USB version that is default on the Arduino from USB 2.0 to USB 2.1. Once this is changed, all sketches placed on the Arduino must inclde the WebUSB library, or else Windows computers will stop recognizing it. [Follow the installation](https://webusb.github.io/arduino/)

# Important Notes

Here are some important information that should be retained when working with this code.

## Watch Memory Usage

Both the program memory space and dynamic memory is nearly maxed out on the Leonardo. There is approximately 2.6K SRAM for the dynamic memory (program stack), and approximately 26.6K of program space memory. The libraries (specifically the ArduinoJSON library) takes up most of this memory. Many tricks were used to use the least amout of Dynamic memory such as only allocating memory when needed, using the `malloc()` function and freeing up memory using the `free()` function whenever possible. Further additions to this code will most likely need an Arduino with more memory.