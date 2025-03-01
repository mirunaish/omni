#include "consts.h"
#include "utils.h"
#include "serial.h"
#include "screen.h"
// #include "arm.h"
// #include "LED.h"
// #include "forceSensor.h"

// ForceSensor head;  // force sensor
// LED cheeks;  // LEDs
// Arm leftArm;
// Arm rightArm;
// TODO add speaker

int frames = 0; // for testing, will be removed

void setup() {
  // start serial for communicating with the client
  Serial.begin(9600);
  Serial.setTimeout(2000);
  Serial.println("LOG arduino is setting up");

  // set up all the parts of the robot

  // head = ForceSensor(A2);
  // cheeks = LED(3);

  // leftArm = Arm(A0, 6, "LEFT");
  // rightArm = Arm(A1, 5, "RIGHT");

  screenSetup();

  // calibrate arms
  // leftArm.calibrate();
  // rightArm.calibrate();

  frames = 0;
}

void loop() {
  if (frames == 10) Serial.println("SEND_ME_THE_IMAGE");
  frames++;

  // listen for messages from serial
  if (Serial.available()) {
    Message message = readMessageFromSerial();

    // if (message.type == "HEADPAT") {
    //   cheeks.blush();
    // } else if (message.type == "WAVE") {
    //   String words[MAX_WORDS];
    //   splitIntoWords(message.payload, words);
    //   String name = words[0];
    //   int value = words[1].toInt();
    //   Serial.println("LOG parsed value from string " + String(value));  // tell client what value i parsed, to make sure it's correct

    //   if (name == "LEFT") leftArm.moveTo(value);
    //   else if (name == "RIGHT") rightArm.moveTo(value);
    //   else Serial.println("ERROR unknown arm " + name);
    // }
    // else if (message.type == "PIXEL") {
      int numbers[MAX_PIXELS * 3];
      int numberCount = 0;
      parseNumbers(message.payload, numbers, &numberCount);
      Serial.println("LOG received " + String(numberCount) + " numbers");  // tell client i got the message
      // each pixel is 3 numbers. loop over them and set
      for (int i=0; i<numberCount/3; i++) {
        // Serial.println("LOG setting " + String(numbers[i*3]) + " " + String(numbers[i*3+1]) + " to " + String(numbers[i*3+2]));
        screenSetPixel(numbers[i*3], numbers[i*3+1], numbers[i*3+2]);
      }
    // }

    // else {
    //   Serial.println("ERROR unknown message type " + message.type);
    // }
  }

  // tell sensors to listen for changes and outputs to update their values
  
  // head.loop();
  // cheeks.loop();
  // leftArm.loop();
  // rightArm.loop();

  // screenLoop();

  Serial.flush();  // force serial to write data
  delay(FRAME_DELAY);
}
