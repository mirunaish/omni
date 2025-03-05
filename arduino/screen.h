#pragma once

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#include "expressions.h"

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

    void reset() {
      tft.fillScreen(color565(0, 0, 0));  // fill screen with black
    }

    void print(String text) {
      tft.setTextColor(color565(255, 255, 255));
      tft.setTextSize(1);
      tft.setCursor(20, cursorY);
      tft.println(text);
      cursorY+=10;
    }

    void makeExpression(String expressionName) {
      int* colors = chooseExpression(expressionName);
      for (int y = 0; y < EXPRESSION_RESOLUTION; y++) {
        for (int x = 0; x < EXPRESSION_RESOLUTION; x++) {
          setPixel(x, y, colors[x + y * EXPRESSION_RESOLUTION]);
        }
      }
    }

    void setPixel(int x, int y, int color) {
      int size = SCREEN_SIZE / EXPRESSION_RESOLUTION;
      tft.fillRect(x*size, y*size, size, size, color);
    }

    void setPixels(int x, int y, int size, uint16_t colors[MAX_PIXELS]) {
      // set colors of pixels in the size*size rectangle with corner at x, y
      tft.drawRGBBitmap(x, y, colors, size, size);
    }

    void loop() {
      // in case anything needs to be updated every frame
    }
};