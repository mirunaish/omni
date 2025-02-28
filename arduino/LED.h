#pragma once

#include <Arduino.h>

int fadeAmount = 5;

class LED {
  private:
    int pin;
    int brightness = 0;

  public:
    LED(int pin) {
      this.pin = pin;

      pinMode(pin, OUTPUT);
    }
}