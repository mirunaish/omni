#include "screen.h"
#include "arm.h"
#include "LED.h"

Arm leftArm;
Arm rightArm;
Screen screen;
LED cheeks;
// TODO add speakers

void setup() {
  // set up all the parts of the robot
  leftArm = new Arm(A0, 6);
  rightArm = new Arm(A1, 5);
  screen = new Screen();
  cheeks = new LED(3);

  // start serial for communicating with the client
  Serial.begin(115200);
}

void loop() {
  // write the position outputs of the pots
  // val_R = analogRead(pot_R);
  // val_L = analogRead(pot_L);

  // Serial.print(val_R);
  // Serial.print(",");
  // Serial.println(val_L); 

  // delay(3*1000);
}
