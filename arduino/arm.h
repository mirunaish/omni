#pragma once

#include <Servo.h>
#include "consts.h"

class Arm {
  private:
    int potPin;
    int servoPin;
    int powerPin;
    String name;

    Servo servo;

    int neutralAngle;

    int currentAngle;  // detected by pot...
    int movingTo = -1;  // where pair told me to move...
    int attempts = 0;

    int minAngleValue = 1024;  // set during pot calibration
    int maxAngleValue = 0;

  public:
    Arm() {}

    void setup(int potPin, int servoPin, int powerPin, String name, int neutralAngle) {
      this->potPin = potPin;
      this->servoPin = servoPin;
      this->powerPin = powerPin;
      this->name = name;

      this->neutralAngle = neutralAngle;

      // set power pin as output
      pinMode(powerPin, OUTPUT);

      // attach servo to pin
      this->servo.attach(servoPin);
      // move to neutral
      this->servo.write(neutralAngle);
      this->currentAngle = neutralAngle;
    }

    /** toggleServo(true) turns it on, false turns it off */
    void toggleServo(bool value) {
      digitalWrite(powerPin, value ? HIGH : LOW);
    }

    /** when first starting, find mapping of servo to pot, to accurately map back later */
    void calibrate() {
      // turn on servos
      toggleServo(true);

      // find min value
      servo.write(180-neutralAngle);
      delay(1500);  // wait for servo to move...
      this->maxAngleValue = analogRead(potPin);

      // find max value
      servo.write(neutralAngle);
      delay(1500);  // wait for servo to move....
      this->minAngleValue = analogRead(potPin);

      // turn off servos
      toggleServo(false);

      Serial.println("LOG arm " + this->name + " calibrated. set min " + String(minAngleValue) + " and max " + String(maxAngleValue));
    }

    /** get the angle of the arms as reported by the pot */
    int getPotAngle() {
      // minAngleValue - maxAngleValue pot range maps to the servo's 0-180 degrees.
      // except they are inverted so max pot is min servo....
      double rawValue = analogRead(potPin);  // double to avoid numerical precision issues
      return floor(180.0 - (180.0 * (rawValue - minAngleValue)) / (maxAngleValue - minAngleValue));
    }

    /** received signal from server that i should move here */
    void moveTo(int value) {
      // turn on servo
      toggleServo(true);
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
        toggleServo(true);
        Serial.println("servo is movinnn " + String(movingTo));
        servo.write(movingTo);
        currentAngle = angleOfPot;
        attempts++;
        
        if (abs(currentAngle - movingTo) <= WAVE_ACCURACY) {
          // destination has been reached
          movingTo = -1;  // no longer moving
        }

        else if (attempts > WAVE_ATTEMPTS) {
          movingTo = -1;  // no longer moving
          sendPosition();  // but tell pair about my new position
          toggleServo(false);  // and turn off servos
        }

        return;
      }

      // otherwise, detect changes above the threshold (but only if not moved my moveTo)
      if (abs(angleOfPot - currentAngle) >= WAVE_THRESHOLD) {
        currentAngle = angleOfPot;
        sendPosition();
      }
    }
};