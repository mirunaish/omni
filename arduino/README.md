# O.M.N.I. Arduino project

## Setup instructions

1. Install the TFT_eSPI and SPI Arduino libraries
2. Copy the contents of [User_Setup.h](../res/User_Setup.h) into the User_Setup.h file in your TFT_eSPI
   library installation (`Documents/Arduino/libraries/TFT_eSPI/` on Windows)
3. Change the receiving buffer size to 256.
   Find the `HardwareSerial.h` file in your Arduino installation.
   On my computer (Windows) this is in `C:\Users\myuser\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\cores\arduino`
   Find the following section:
   ```
   #if ((RAMEND - RAMSTART) < 1023)
   #define SERIAL_RX_BUFFER_SIZE 16
   #else
   #define SERIAL_RX_BUFFER_SIZE 64
   #endif
   ```
   Change it to the following:
   ```
   #if ((RAMEND - RAMSTART) < 1023)
   #define SERIAL_RX_BUFFER_SIZE 256
   #else
   #define SERIAL_RX_BUFFER_SIZE 256
   #endif
   ```
