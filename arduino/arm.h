#pragma once

#include <Servo.h>

int neutralAngle = 90;

class Arm {
  private:
    int potPin;
    int servoPin;
    Servo servo;

    int currentAngle;

  public:
    Arm(int potPin, int servoPin) {
      this.potPin = potPin;
      this.servoPin = servoPin;
      // attach servo to pin
      this.servo.attach(servoPin);
      // move to neutral
      this.servo.write(neutralAngle);
    }
}