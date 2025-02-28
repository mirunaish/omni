#pragma once

#include "consts.h"

class LED {
  private:
    int pin;
    int brightness = 0;

  public:
    LED() {}

    LED(int pin) {
      this->pin = pin;

      pinMode(pin, OUTPUT);
    }

    void loop() {
      ;
    }

    void blush() {
      ; 
    }
};