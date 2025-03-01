#pragma once

struct Message {
  String type;
  String payload;
};

void splitIntoWords(String sentence, String words[MAX_WORDS]) {
  int wordCount = 0;

  // find start and end of current word
  int start = 0;
  int end = sentence.indexOf(' ');

  // while i haven't run out of spaces
  while (end != -1) {
    words[wordCount++] = sentence.substring(start, end);
    // get next word
    start = end + 1;
    end = sentence.indexOf(' ', start);
  }
  
  // add the word after the last space
  words[wordCount++] = sentence.substring(start);
}

void parseNumbers(String sentence, int numbers[MAX_PIXELS * 3], int* numberCount) {
  *numberCount = 0;

  // find start and end of current word
  int start = 0;
  int end = sentence.indexOf(' ');

  // while i haven't run out of spaces
  while (end != -1) {
    numbers[*numberCount] = sentence.substring(start, end).toInt();
    (*numberCount)++;
    // get next number
    start = end + 1;
    end = sentence.indexOf(' ', start);
  }
  
  // add the number after the last space
  numbers[*numberCount] = sentence.substring(start).toInt();
  (*numberCount)++;
}

Message readMessageFromSerial() {
  // read entire sentence
  String sentence = Serial.readStringUntil('\n');
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
