#pragma once

#include "consts.h"

class LED {
  private:
    int pin;
    int brightness = 0;
    int blushTimer = 0;

  public:
    LED() {}

    void setup(int pin) {
      this->pin = pin;

      pinMode(pin, OUTPUT);
      setBrightness(0);
    }

    void setBrightness(int value) {
      this->brightness = max(0, min(value, 255));  // clamp to [0, 255]
      analogWrite(pin, brightness);
    }

    void loop() {
      if (blushTimer > 0) {
        blushTimer--;
        // fade in blush
        if (brightness < 255) setBrightness(brightness + BLUSH_SPEED);
      } else if (blushTimer == 0 && brightness > 0) {
        // fade out blush
        setBrightness(brightness - BLUSH_SPEED);
      }
    }

    void blush() {
      blushTimer = getFrames(BLUSH_DURATION);
    }
};