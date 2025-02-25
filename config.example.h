/**********************************************************************

The configuration file for Serial Debugger

rename / or copy this File to config.h

**********************************************************************/

#if defined(__AVR__)
  // Buttons PIN
  #define BAUD_PIN 10
  #define MODE_PIN 9
  #define CLS_PIN  16

  // Pins for Software Serial, eg Arduino Nano
  #define rxPin 6
  #define txPin 7
#endif

#if defined(ESP32)
  // ESP32 Buttons PIN == GPIO
  #define BAUD_PIN 19
  #define MODE_PIN 14
  #define CLS_PIN  18
#endif

// Defaults at Start
// DISPLAY_TXT / DISPLAY_HEX / DISPLAY_BIN
#define DISPLAY_DEFAULT DISPLAY_TXT
// 1200 / 2400 / 4800 / 9600 / 19200 / 38400 / 57500 / 115200 / 230400
#define BAUD_DEFAULT 115200
