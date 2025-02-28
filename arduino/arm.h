#pragma once

#include <Servo.h>
#include "consts.h"

class Arm {
  private:
    int potPin;
    int servoPin;
    String name;

    Servo servo;

    int currentAngle;  // detected by pot...
    int movingTo = -1;  // where pair told me to move...
    int attempts = 0;

  public:
    Arm() {}

    Arm(int potPin, int servoPin, String name) {
      this->potPin = potPin;
      this->servoPin = servoPin;
      this->name = name;

      // attach servo to pin
      this->servo.attach(servoPin);
      // move to neutral
      this->servo.write(Consts::neutralAngle);
    }

    /** get the angle of the arms as reported by the pot */
    int getPotAngle() {
      // assume entire 0-1023 range maps to the servo's 0-180 degrees.
      // they are inverted so max pot is min servo....
      // this likely won't be true, values here will have to be adjusted
      // TODO: calibrate
      int rawValue = analogRead(potPin);
      return floor(180.0 - (180.0 * rawValue) / 1023.0);
    }

    /** received signal from server that i should move here */
    void moveTo(int value) {
      movingTo = value;
      attempts = 0;
    }

    void sendPosition() {
      Serial.println("WAVE " + this->name + " " + String(getPotAngle()));
    }

    void loop() {
      int angleOfPot = getPotAngle();

      // if i am supposed to move somewhere, attempt to move there until pot reports i have reached my destination
      if (movingTo != -1) {
        servo.write(movingTo);
        currentAngle = angleOfPot;
        attempts++;
        
        if (abs(currentAngle - movingTo) <= Consts::waveAccuracy) {
          // destination has been reached
          movingTo = -1;  // no longer moving
        }

        else if (attempts > Consts::waveAttempts) {
          movingTo = -1;  // no longer moving
          sendPosition();  // but tell pair about my new position
        }

        return;
      }

      // otherwise, detect changes above the threshold (but only if not moved my moveTo)
      if (abs(angleOfPot - currentAngle) >= Consts::waveThreshold) {
        currentAngle = angleOfPot;
        sendPosition();
      }
    }
};