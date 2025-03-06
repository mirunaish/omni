#pragma once

#include <SPI.h>
#include <TFT_eSPI.h>

#define TFT_CS 10  // Chip Select pin
#define TFT_DC 9  // Data/Command pin
#define TFT_RST 8  // Reset pin

TFT_eSPI tft = TFT_eSPI();
int cursorY = 50;

screenSetup() {
  tft.init();
  tft.setRotation(2);  // TODO change back to 0?
  tft.fillScreen(TFT_BLACK);  // initialize to black
}

void screenPrint(String text) {
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, cursorY);
  tft.println(text);
  cursorY+=10;
}

void screenSetPixel(int x, int y, int color) {
  if (SCREEN_SCALE == 1) tft.drawPixel(x, y, color);
  else tft.fillRect(x*SCREEN_SCALE, y*SCREEN_SCALE, SCREEN_SCALE, SCREEN_SCALE, color);
}

void screenSetPixels(int x, int y, int size, int colors[MAX_PIXELS]) {
  // set colors of pixels in the size*size rectangle with corner at x, y
  tft.setAddrWindow(x, y, size, size);
  tft.pushColors(colors, size*size);
}

void screenLoop() {
  // in case anything needs to be updated every frame
}
