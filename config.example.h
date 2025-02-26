/**********************************************************************

The configuration file for Serial Debugger

rename / or copy this File to config.h

**********************************************************************/

#if defined(__AVR__)
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    // UNO, Nano

    // I2C: SDA - A4, SCL - A5
    // Serial: RX - 1, TX - 0

    // Buttons PIN
    #define BAUD_PIN 10
    #define MODE_PIN 11
    #define CLS_PIN  12

    // Define Display: Display Defines, see U8g2 Contructor List.txt
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  #endif
  
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
    // Leonardo, Micro

    // I2C: SDA - 2, SCL - 3
    // Serial: RX - 1, TX - 0

    // Buttons PIN
    #define BAUD_PIN 10
    #define MODE_PIN 9
    #define CLS_PIN  16

    // Define Display: Display Defines, see U8g2 Contructor List.txt
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  #endif
#endif

#if defined(ESP8266)
  // ESP8266 Boards

  // I2C: SDA - D2 (GPIO4), SCL - D1 (GPIO5)
  // Serial: RX - GPIO3, TX - GPIO1

    // Buttons PIN
    #define BAUD_PIN D5
    #define MODE_PIN D6
    #define CLS_PIN  D7

    // Define Display: Display Defines, see U8g2 Contructor List.txt
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#endif

#if defined(ESP32)
  // ESP32 Boards

  // I2C: SDA - 21, SCL - 22
  // Serial: RX - 16, TX - 17

  // Buttons PIN
  #define BAUD_PIN 14
  #define MODE_PIN 19
  #define CLS_PIN  18

  // Define Display: Display Defines, see U8g2 Contructor List.txt
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#endif

// Defaults at Start
// DISPLAY_TXT / DISPLAY_HEX / DISPLAY_BIN
#define DISPLAY_DEFAULT DISPLAY_TXT
// 1200 / 2400 / 4800 / 9600 / 19200 / 38400 / 57500 / 115200 / 230400
#define BAUD_DEFAULT 115200
