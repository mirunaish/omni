#pragma once

/** define colors */
int ___ = color565(210, 170, 0);  // base yellow for emoji expressions (the name is that to make it easier to see)
int WHT = color565(255, 255, 255);
int BLK = color565(20, 10, 10);  //  a warm black...
int BLU = color565(100, 160, 200);
int PNK = color565(220, 170, 160);
int BWN = color565(80, 50, 20);

/** define expressions */

int SMILE[EXPRESSION_SIZE] = {
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, WHT, BWN, ___, ___, WHT, BWN, ___, ___,
  ___, ___, BWN, BWN, ___, ___, BWN, BWN, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, BWN, ___, ___, ___, ___, BWN, ___, ___,
  ___, ___, ___, BWN, BWN, BWN, BWN, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___
};

int SAD[EXPRESSION_SIZE] = {
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
  ___, ___, ___, ___, ___, ___, ___, ___, ___, ___
};

/** this is annoying but Arduino doesn't support dictionaries so i have to do it manually */
int* chooseExpression(String exp) {
  if (false);  // this is just so everything else is nicely aligned...

  else if (exp == "SMILE") return SMILE;
  else if (exp == "SAD") return SAD;
  // etc....

  else Serial.println("ERROR unrecognized expression" + exp);
}