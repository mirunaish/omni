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

    
    // if they are not inverted, max pot is min servo, and servo 0 is down
    // if they are inverted, min pot is min servo, and servo 0 is up, 180 is down
    bool inverted;

    int currentAngle;  // detected by pot...
    int movingTo = -1;  // where pair told me to move...
    int attempts = 0;
    int cooldown = 0;

    int upValue = 1024;  // set during pot calibration
    int downValue = 0;

  public:
    Arm() {}

    void setup(int potPin, int servoPin, int powerPin, String name, bool inverted) {
      this->potPin = potPin;
      this->servoPin = servoPin;
      this->powerPin = powerPin;
      this->name = name;

      this->inverted = inverted;

      // set power pin as output
      pinMode(powerPin, OUTPUT);

      // attach servo to pin
      this->servo.attach(servoPin);
      // move to neutral
      this->currentAngle = inverted ? 180 : 0;
      this->servo.write(this->currentAngle);

      this->cooldown = 0;
    }

    /** toggleServo(true) turns it on, false turns it off */
    void toggleServo(bool value) {
      digitalWrite(powerPin, value ? HIGH : LOW);
    }

    /** when first starting, find mapping of servo to pot, to accurately map back later */
    void calibrate() {
      // turn on servos
      toggleServo(true);

      // find up value
      servo.write(inverted ? 0 : 180);
      delay(1500);  // wait for servo to move....
      this->upValue = analogRead(potPin);

      // find down value
      servo.write(inverted ? 180 : 0);
      delay(1500);  // wait for servo to move...
      this->downValue = analogRead(potPin);

      // move to neutral...
      servo.write(inverted ? 180 : 0);
      delay(1000);  // wait for it to move

      // turn off servos
      toggleServo(false);

      Serial.println("LOG arm " + this->name + " calibrated. set down " + String(downValue) + " and up " + String(upValue));
    }

    /** get the angle of the arms as reported by the pot */
    int getPotAngle() {
      // upValue - downValue pot range maps to the servo's 0-180 degrees.
      double rawValue = analogRead(potPin);  // double to avoid numerical precision issues

      // if they are not inverted, max pot is min servo, and servo 0 is down
      // if they are inverted, min pot is min servo, and servo 0 is up, 180 is down
      
      // percentage between down and up...
      double percentage = (rawValue - downValue) * 1.0 / (upValue - downValue);
      
      int value = floor(180.0 * percentage);
      return inverted ? 180 - value : value;
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
      Serial.println("LOG " + String(angleOfPot));
      cooldown--;

      // if i am supposed to move somewhere, attempt to move there until pot reports i have reached my destination
      if (movingTo != -1) {
        Serial.println("LOG moving arm");
        servo.write(movingTo);
        currentAngle = angleOfPot;
        attempts++;
        
        if (abs(currentAngle - movingTo) <= WAVE_ACCURACY) {
          // destination has been reached
          movingTo = -1;  // no longer moving
        }

        else if (attempts > WAVE_ATTEMPTS) {
          movingTo = -1;  // no longer moving
          // sendPosition();  // but tell pair about my new position
          toggleServo(false);  // and turn off servos
        }

        return;
      }

      // if cooldown, don't send message...
      if (cooldown > 0) {
        return;
      }

      // otherwise, detect changes above the threshold (but only if not moved my moveTo)
      if (abs(angleOfPot - currentAngle) >= WAVE_THRESHOLD) {
        currentAngle = angleOfPot;
        sendPosition();
        this->cooldown = getFrames(WAVE_COOLDOWN);
      }
    }
};