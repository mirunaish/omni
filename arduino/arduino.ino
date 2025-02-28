#include "consts.h"
#include "utils.h"
#include "serial.h"
// #include "screen.h"
#include "arm.h"
#include "LED.h"
#include "forceSensor.h"

ForceSensor head;  // force sensor
LED cheeks;  // LEDs
Arm leftArm;
Arm rightArm;
// Screen screen;
// TODO add speaker

void setup() {
  // set up all the parts of the robot

  head = ForceSensor(A2);
  cheeks = LED(3);

  leftArm = Arm(A0, 6);
  rightArm = Arm(A1, 5);

  // screen = Screen();

  // start serial for communicating with the client
  Serial.begin(115200);
}

void loop() {
  // listen for messages from serial
  if (Serial.available()) {
    Message message = readMessageFromSerial();

    if (message.type == "HEADPAT") {
      cheeks.blush();
    } else {
      Serial.println("ERROR unknown message type " + message.type);
    }
  }

  // tell sensors to listen for changes and outputs to update their values
  
  head.loop();
  cheeks.loop();

  Serial.flush();  // force serial to write data
  delay(Consts::frameDelay);
}
