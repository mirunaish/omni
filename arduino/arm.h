#pragma once

#include <Servo.h>
#include "consts.h"

class Arm {
  private:
    int potPin;
    int servoPin;
    Servo servo;

    int currentAngle;

  public:
    Arm() {}

    Arm(int potPin, int servoPin) {
      this->potPin = potPin;
      this->servoPin = servoPin;
      // attach servo to pin
      this->servo.attach(servoPin);
      // move to neutral
      this->servo.write(Consts::neutralAngle);
    }

    void loop() {
      // write the position outputs of the pots
      // val_R = analogRead(pot_R);
      // val_L = analogRead(pot_L);

      // Serial.print(val_R);
      // Serial.print(",");
      // Serial.println(val_L); 
    }
};