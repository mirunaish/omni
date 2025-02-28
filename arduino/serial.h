#pragma once

struct Message {
  String type;
  String payload;
};

String readSentenceFromSerial() {
  String sentence;
  char byte;

  while (Serial.available()) {
    char byte = Serial.read();
    if (byte != '\n') {
      break;
    }
    sentence += byte;
  }

  return sentence;
}

Message readMessageFromSerial() {
  // read entire sentence
  String sentence = readSentenceFromSerial();
  Message message;
  
  int spaceIndex = sentence.indexOf(' ');  // find the first space
  if (spaceIndex != -1) {
    message.type = sentence.substring(0, spaceIndex); // first word
    message.payload = sentence.substring(spaceIndex + 1); // everything else
  } else {
    // no space. entire message must be type
    message.type = sentence;
  }
  
  return message;
}
