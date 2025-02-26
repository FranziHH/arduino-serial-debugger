# ! Fork From arduino-serial-debugger
This is Forked From https://github.com/ma-ha/arduino-serial-debugger

# arduino-serial-debugger

Wire a Arduino Micro with a I2C OLED and 2 buttons, put it in a case 
... and you have a "Serial Port Debugger".

Push Buttons:
1. Toggle BAUD rate
2. Toggle display mode: HEX, BIN or TEXT

I've made a YouTube video about this project (German):
https://youtu.be/XUHYciV1GeM


# New
- Boards Arduino Nano and Uno added: Here debugging is disabled, there is only a hardware serial interface
- The display definition has been moved to config.h. Individual settings are possible here.

# Changes
- for easier use converted to vsCode
- third Button: Clear Screen added
- Added code for use with ESP32
- Error: interfaces must not be accessed in the interrupt
- ToDo: It is possible to add other boards for use

### ESP32-WROOM-32D

[ESP32 Pinout Reference](https://lastminuteengineers.com/esp32-pinout-reference/)
<br>

##### Connections
I2C Display, e.g.: 2,4-Zoll-OLED-Display 128x64 I2C SSD1309
- SCL - GPIO22
- SDA - GPIO21

Buttons - one PIN to GND
- Button Baud - GPIO19
- Button Mode - GPIO14
- Button Cls  - GPIO18

Serial Port (2)
- TX2 - GPIO17 (unused)
- RX2 - GPIO16
<br>

##### Additional
In addition, an HC-12 module was integrated
to monitor the wireless transmission
<br>

##### Images
<img src="/Images/IMG_20250225_192316.jpg" height="400px" title="PCB with ESP32">   
<img src="/Images/IMG_20250225_192532.jpg" height="400px" title="Boot Screen">   
<img src="/Images/IMG_20250225_192747.jpg" height="400px" title="Monitoring">   

