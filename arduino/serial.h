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

/** returns number of ints read */
int readIntsFromSerial(int *buffer, size_t count) {
  size_t bytesToRead = count * 2;  // an int is 2 bytes
  size_t bytesRead = 0;

  while (bytesRead < bytesToRead) {
    if (Serial.available() >= 2) {
      uint8_t lowByte = Serial.read();
      uint8_t highByte = Serial.read();
      buffer[bytesRead / 2] = (highByte << 8) | lowByte;  // combine both bytes into a single 16-bit number
      bytesRead += 2;
    }
  }

  return bytesRead / 2;
}

/**
 * serial prints endlines so messages are human readable.
 * if arduino doesn't need it, use this function to remove it from serial buffer
 * this also has the benefit of recovering from errors caused by buffer overflow
 */
void readUntilEndline() {
  char byte = 0;
  int charsRead = 0;

  while (byte != '\n') {
    if (Serial.available()) {
      byte = Serial.read();
      charsRead++;
    }
  }

  if (charsRead == 0) Serial.println("ERROR readUntilEndline failed to read endline");
  if (charsRead > 1) Serial.println("ERROR readUntilEndline read " + String(charsRead - 1) + " non-endline characters");
}
