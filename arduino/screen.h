#pragma once

#include <SPI.h>
#include <TFT_eSPI.h>

#define TFT_CS 10  // Chip Select pin
#define TFT_DC 9  // Data/Command pin
#define TFT_RST 8  // Reset pin

class Screen {
  private:
    TFT_eSPI tft = TFT_eSPI();

  public:
    Screen() {
      tft.init();
      tft.setRotation(2);  // TODO change back to 0?
      tft.fillScreen(TFT_BLACK);  // initialize to black
    }
};
