#pragma once

#include "consts.h"
#include "utils.h"

class ForceSensor {
  private: 
    int pinId;
    int cooldown = 0;  // in frames

  public:
    ForceSensor() {}
    
    ForceSensor(int pinId) {
      this->pinId = pinId;
    }

    void loop() {
      if (cooldown > 0) {
        cooldown--;
        return;
      }

      // read value from sensor
      int value = analogRead(pinId);

      // if value is above the threshold, tell serial a pat happened
      if (value > PAT_THRESHOLD) {
        Serial.println("HEADPAT");
        this->cooldown = getFrames(PAT_COOLDOWN);
      }
    }
};
