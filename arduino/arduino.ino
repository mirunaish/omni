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

  leftArm = Arm(A0, 6, "LEFT");
  rightArm = Arm(A1, 5, "RIGHT");

  // screen = Screen();

  // start serial for communicating with the client
  Serial.begin(115200);
}

void loop() {
  // listen for messages from serial
  if (Serial.available()) {
    Message message = readMessageFromSerial();
    Serial.println("LOG arduino received message " + message.type + " " + message.payload);  // tell client i got the message

    if (message.type == "HEADPAT") {
      cheeks.blush();
    } else if (message.type == "WAVE") {
      String words[Consts::maxWords];
      splitIntoWords(message.payload, words);
      String name = words[0];
      int value = words[1].toInt();
      Serial.println("LOG parsed value from string " + String(value));  // tell client what value i parsed, to make sure it's correct

      if (name == "LEFT") leftArm.moveTo(value);
      else if (name == "RIGHT") rightArm.moveTo(value);
      else Serial.println("ERROR unknown arm " + name);
    }

    else {
      Serial.println("ERROR unknown message type " + message.type);
    }
  }

  // tell sensors to listen for changes and outputs to update their values
  
  head.loop();
  cheeks.loop();
  leftArm.loop();
  rightArm.loop();

  Serial.flush();  // force serial to write data
  delay(Consts::frameDelay);
}
