////////////////////////////////////////////////////////////////////////////////////
//  Serial Port Debugger/Monitor 
//  https://github.com/ma-ha/arduino-serial-debugger
//  by ma-ha 
//  Copyright (c) 2023 ma-ha, MIT License
//  --------------------------------------------------------------------------------
//  Adapted by Franziska Walter
//  2025/02 
//  https://github.com/FranziHH/arduino-serial-debugger
////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <U8g2lib.h>

#define DISPLAY_TXT 'T'
#define DISPLAY_HEX 'H'
#define DISPLAY_BIN 'B'

/* Print Serial Input to Serial Console 
 * ! only when second hardware serial available !
 */
#define DEBUG
/* print Button Actions */
#define DEBUG_MSG

// include config
#include "config.h"

#if defined(__AVR__)
  #include "PinChangeInterrupt.h"

  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    //Code in here will only be compiled if an Arduino Uno (or older) is used.
    #if defined(DEBUG) || defined(DEBUG_MSG)
      #undef DEBUG
      #undef DEBUG_MSG
      #warning Disable Debug - Only one Serial available!
    #endif

    #define mySerial Serial
    #define RECOGNIZED
  #endif
  
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
    //Code in here will only be compiled if an Arduino Leonardo is used.
    #define mySerial Serial1
    #define RECOGNIZED
  #endif
#endif

// ============================================================================

#if defined(ESP8266)
  #define ISR_ATTR ICACHE_RAM_ATTR

  #if defined(DEBUG) || defined(DEBUG_MSG)
    #undef DEBUG
    #undef DEBUG_MSG
    #warning Disable Debug - Only one Serial available!
  #endif

  #define mySerial Serial
  #define RECOGNIZED
#endif

// ============================================================================

#if defined(ESP32)
  #define ISR_ATTR IRAM_ATTR

  #include <HardwareSerial.h>
  HardwareSerial mySerial(2);
  #define RECOGNIZED
#endif

// ============================================================================

#ifndef RECOGNIZED
  #error Board was not recognized
#endif

#ifndef ISR_ATTR
  #define ISR_ATTR 
#endif

#if defined(DEBUG_MSG) && !defined(DEBUG) 
  #define DEBUG
#endif

#ifdef U8X8_HAVE_HW_SPI
  #include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
  #include <Wire.h>
#endif

unsigned long baud = BAUD_DEFAULT_0;
char mode = DISPLAY_DEFAULT_0;

String str[8];
String tmp;
boolean isStartScreen = true;
boolean changed = false;
boolean changedMode = false;
boolean changedBaud = false;
boolean changedScreen = false;
byte strLen = 0;
byte inputMode = 0;
byte lastInputMode = 0;
byte debounceTime = 100;
unsigned long lastTime[4];

// ============================================================================

void readInputSwitch() {
  inputMode = 0;
  #if defined(INP_PIN_1)
    if (digitalRead(INP_PIN_1) == LOW) {
      inputMode = 1;
    }
  #endif
  #if defined(INP_PIN_2)
    if (digitalRead(INP_PIN_2) == LOW) {
      inputMode = 2;
    }
  #endif
  if (inputMode != lastInputMode) {
    switch (inputMode) {
      #if defined(INP_PIN_1)
      case 1: baud = BAUD_DEFAULT_1; mode = DISPLAY_DEFAULT_1; break;
      #endif
      #if defined(INP_PIN_2)
      case 2: baud = BAUD_DEFAULT_2; mode = DISPLAY_DEFAULT_2; break;
      #endif
      default: baud = BAUD_DEFAULT_0; mode = DISPLAY_DEFAULT_0; break;
    }
    // clear screen
    if (!isStartScreen) for (byte i = 0; i < 8; i++) str[i] = "";
    changedBaud = true;
    changedMode = true;
    lastInputMode = inputMode;
    changed = true; // is double ..
  }
}

// ============================================================================

void loop(void) {
  readInputSwitch();

  if (changedBaud) {
    mySerial.end(); 
    mySerial.begin(baud); 
    changedBaud = false;
    changed = true;
    #if defined(DEBUG_MSG)
      Serial.print("Baud ");
      Serial.println(baud);
    #endif
  }
  if (changedMode) {
    changedMode = false;
    changed = true;
    #if defined(DEBUG_MSG)
      Serial.print("Mode ");
      Serial.println(mode);
    #endif
  }
  if (changedScreen) {
    changedScreen = false;
    changed = true;
    #if defined(DEBUG_MSG)
      Serial.println("Clear Screen");
    #endif
  }
  readStr();
  if (changed) {
    writeOLED();
    changed = false;
  }
}

// ============================================================================

void writeOLED() {
  u8g2.clearBuffer(); 
  u8g2.setFont( u8g2_font_5x7_tf ); 
  for ( byte i=0; i < 8; i++ ) {
    u8g2.drawStr( 0, 64-7*i, str[i].c_str() );  
  }
  u8g2.setFont( u8g2_font_tinytim_tf ); 
  u8g2.drawStr( 95, 5, String( baud ).c_str()  );
  switch (mode) {
    case DISPLAY_TXT: u8g2.drawStr( 85, 5, "T" ); break;
    case DISPLAY_BIN: u8g2.drawStr( 85, 5, "B" ); break;
    case DISPLAY_HEX: u8g2.drawStr( 85, 5, "H" ); break;
    default: u8g2.drawStr( 85, 5, "t" ); break;
  }
  switch (inputMode) {
    #if defined(INP_PIN_1)
    case 1: u8g2.drawStr( 0, 5, INP_TXT_1 ); break;
    #endif
    #if defined(INP_PIN_2)
    case 2: u8g2.drawStr( 0, 5, INP_TXT_2 ); break;
    #endif
    default: u8g2.drawStr( 0, 5, INP_TXT_0 ); break;
  }
  u8g2.sendBuffer();    
}

// ============================================================================

void readStr() {
  while ( mySerial.available() > 0 ) {
    char inChar = (char) mySerial.read();
    
    switch (mode) {
      case DISPLAY_TXT: 
        strLen ++;
        if ( inChar == '\n' ) {
          newLine();
        } else if ( strLen > 23 ) {
          newLine();
          str[0] += inChar;
        } else {
          str[0] += inChar;
        }
        break;
      case DISPLAY_BIN: 
        if ( strLen > 2 ) {
          newLine();
        }
        str[0] += String( inChar, BIN ) + " ";
        strLen ++;
        break;
      case DISPLAY_HEX: 
        strLen += 3;
        tmp = String( inChar, HEX );
        if ( tmp.length() < 2 ) {
          str[0] += "0" + tmp + " ";
        } else if ( tmp.length() > 2 ) {
          // wrong char
          str[0] += "xx ";
        } else {
          str[0] += tmp + " ";
        }
        if ( strLen > 23 ) {
          newLine();
        }
        break;
      default: break;
    }

    if (isStartScreen && ((millis() - lastTime[0]) > 1000)) {
      for (byte i = 1; i < 8; i++) str[i] = "";
      isStartScreen = false;
    }

    changed = true;
  }
}

// ============================================================================

void newLine() {
  #if defined(DEBUG)
    Serial.println( str[0].c_str() );
  #endif
  for ( byte i = 7; i > 0; i-- ) {
    str[i] = str[i-1];
  }
  str[0] = "";
  strLen = 0;
}

// ============================================================================

void ISR_ATTR changeBaud() {
  if ((millis() - lastTime[1]) > debounceTime) {
    lastTime[1] = millis();
    if (baud == 1200) {
      baud =  2400;
    } else if (baud == 2400 ) {
      baud =  4800;
    } else if (baud == 4800 ) {
      baud =  9600;
    } else if (baud == 9600 ) {
      baud =  19200;
    } else if (baud ==  19200 ) {
      baud =  38400;
    } else if (baud ==  38400 ) {
      baud =  57600;
    } else if (baud ==  57600 ) {
      baud =  115200;
    } else if (baud == 115200 ) {
      baud = 230400;
    } else if (baud == 230400 ) {
      baud = 1200;
    }
    changedBaud = true;
  }
}

// ============================================================================

void ISR_ATTR changeMode() {
  if ((millis() - lastTime[2]) > debounceTime) {
    lastTime[2] = millis();
    if (mode == DISPLAY_TXT) {
      mode = DISPLAY_HEX;
    } else if (mode == DISPLAY_HEX) {
      mode = DISPLAY_BIN;    
    } else if (mode == DISPLAY_BIN) {
      mode = DISPLAY_TXT;
    }
    changedMode = true;
  }
}

// ============================================================================

void ISR_ATTR clearScreen() {
  if ((millis() - lastTime[3]) > debounceTime) {
    lastTime[3] = millis();
    for (byte i = 0; i < 8; i++) {
      str[i] = "";
    }
    strLen = 0;
    changedScreen = true;
  }
}

// ============================================================================

void setup(void) {
  #if defined(DEBUG)
    Serial.begin(9600); 
  #endif
  mySerial.begin(baud); 
  u8g2.begin();

  for ( byte i = 0; i < 8; i++ ) {
    str[i].reserve(25);
    str[i] = "";
  }
  str[6] = "  SERIAL PORT DEBUGGER V4";
  str[5] = "            BY";
  str[4] = "      MAKER GARAGE DE";
  str[3] = "  ----------------------";
  str[2] = "    2025/03 ADAPTED BY";
  str[1] = "      FRANZI.HAMBURG";
  str[0] = "";

  #if defined(DEBUG)
    Serial.println("SERIAL PORT DEBUGGER v4");
  #endif

  // button config
  pinMode(BAUD_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);
  pinMode(CLS_PIN, INPUT_PULLUP);
  #if defined(INP_PIN_1)
    pinMode(INP_PIN_1, INPUT_PULLUP);
  #endif
  #if defined(INP_PIN_2)
    pinMode(INP_PIN_2, INPUT_PULLUP);
  #endif

#if defined(__AVR__)
  attachPinChangeInterrupt( 
    digitalPinToPinChangeInterrupt(BAUD_PIN), 
    changeBaud, 
    RISING 
  );
  attachPinChangeInterrupt( 
    digitalPinToPinChangeInterrupt(MODE_PIN), 
    changeMode, 
    RISING   
  );
  attachPinChangeInterrupt( 
    digitalPinToPinChangeInterrupt(CLS_PIN), 
    clearScreen, 
    RISING   
  );
// #elif defined(ESP32) || defined(ESP8266)
#else
  attachInterrupt(BAUD_PIN, changeBaud, RISING);
  attachInterrupt(MODE_PIN, changeMode, RISING);
  attachInterrupt(CLS_PIN, clearScreen, RISING);
#endif

  readInputSwitch();
  writeOLED();
  lastTime[0] = millis();

}
