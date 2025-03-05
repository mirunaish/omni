#pragma once

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#define TFT_CS 10  // Chip Select pin
#define TFT_DC 9  // Data/Command pin
#define TFT_RST 8  // Reset pin


class Screen {
  private:
    Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);
    int cursorY = 50;

  public:
    Screen() {}

    void setup() {
      tft.begin();
      tft.setRotation(2);  // TODO change back to 0?
      reset();  // initialize to black
    }

    void print(String text) {
      tft.setTextColor(0xFFFF);
      tft.setTextSize(1);
      tft.setCursor(20, cursorY);
      tft.println(text);
      cursorY+=10;
    }

    void reset() {
      tft.fillScreen(0x0000);  // fill screen with black
    }

    void setPixel(int x, int y, int color) {
      if (SCREEN_SCALE == 1) tft.drawPixel(x, y, color);
      else tft.fillRect(x*SCREEN_SCALE, y*SCREEN_SCALE, SCREEN_SCALE, SCREEN_SCALE, color);
    }

    void setPixels(int x, int y, int size, uint16_t colors[MAX_PIXELS]) {
      // set colors of pixels in the size*size rectangle with corner at x, y
      tft.drawRGBBitmap(x, y, colors, size, size);
    }

    void loop() {
      // in case anything needs to be updated every frame
    }
};