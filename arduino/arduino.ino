#include "consts.h"
#include "utils.h"
#include "serial.h"
#include "screen.h"
#include "arm.h"
#include "LED.h"
#include "forceSensor.h"

ForceSensor head;  // force sensor
LED cheeks;  // LEDs
Screen screen;  // face
Arm leftArm;
Arm rightArm;
// TODO add speaker

int frames = 0; // for testing, will be removed

void setup() {
  // start serial for communicating with the client
  Serial.begin(1000000);  // 1 million!!! bits per second
  Serial.println("LOG arduino is setting up");

  // set up all the parts of the robot

  head.setup(A4);
  cheeks.setup(4);

  leftArm.setup(A0, 2, "LEFT");
  rightArm.setup(A2, 3, "RIGHT");

  screen.setup();

  // calibrate arms
  // leftArm.calibrate();
  // rightArm.calibrate();

  frames = 0;
}

int colors[MAX_PIXELS];  // here so i don't have to constantly reallocate

void loop() {
  // listen for messages from serial
  if (Serial.available()) {
    String type = Serial.readStringUntil(';');  // read message type
    // depending on type, each component will read the rest of the data

    if (type == "HEADPAT") {
      cheeks.blush();
      readUntilEndline();
    }

    else if (type == "WAVE") {
      // message format: name value
      String name = Serial.readStringUntil(' ');
      int value = Serial.readStringUntil('\n').toInt();

      if (name == "LEFT") leftArm.moveTo(value);
      else if (name == "RIGHT") rightArm.moveTo(value);
      else Serial.println("ERROR unknown arm " + name);
    }

    else if (type == "RESET_SCREEN") {
      screen.reset();
      readUntilEndline();
    }

    else if (type == "EXPRESSION") {
      // message format is: [colors as 16 bit ints]
      // there should be EXPRESSION_SIZE (EXPRESSION_RESOLUTION*EXPRESSION_RESOLUTION) uint_16 bytes being sent. read them
      readIntsFromSerial(colors, EXPRESSION_SIZE);
      screen.makeExpression(colors);

      readUntilEndline();
      // only one message so it's fine to delay
    }

    else if (type == "PIXELS") {
      // message format is: x y size [colors as 16 bit ints]
      // read x y and size
      int values[3];
      readIntsFromSerial(values, 3);
      // there should be size*size uint_16 bytes being sent. read them
      readIntsFromSerial(colors, values[2]*values[2]);
      screen.setPixels(values[0], values[1], values[2], colors);

      readUntilEndline();

      // don't do anything else (esp delay). just loop again to read the next message
      return;
    }

    else {
      Serial.println("ERROR unknown message type");
    }
  }

  // tell sensors to listen for changes and outputs to update their values
  head.loop();
  cheeks.loop();
  leftArm.loop();
  rightArm.loop();
  screen.loop();

  Serial.flush();  // force serial to write data
  delay(FRAME_DELAY);
}
