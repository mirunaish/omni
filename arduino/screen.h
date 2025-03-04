#pragma once

#include <SPI.h>
#include <TFT_eSPI.h>

#define TFT_CS 10  // Chip Select pin
#define TFT_DC 9  // Data/Command pin
#define TFT_RST 8  // Reset pin


class Screen {
  private:
    TFT_eSPI tft = TFT_eSPI();
    int cursorY = 50;

  public:
    Screen() {
      tft.init();
      tft.setRotation(2);  // TODO change back to 0?
      tft.fillScreen(TFT_BLACK);  // initialize to black
    }

    void print(String text) {
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(1);
      tft.setCursor(20, cursorY);
      tft.println(text);
      cursorY+=10;
    }

    void setPixel(int x, int y, int color) {
      if (SCREEN_SCALE == 1) tft.drawPixel(x, y, color);
      else tft.fillRect(x*SCREEN_SCALE, y*SCREEN_SCALE, SCREEN_SCALE, SCREEN_SCALE, color);
    }

    void setPixels(int x, int y, int size, int colors[MAX_PIXELS]) {
      // set colors of pixels in the size*size rectangle with corner at x, y
      tft.setAddrWindow(x, y, size, size);
      tft.pushColors(colors, size*size);
    }

    void loop() {
      // in case anything needs to be updated every frame
    }
};