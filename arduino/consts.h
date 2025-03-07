#pragma once

/* --------- consts --------- */

const int FRAME_DELAY = 10;  // in ms
const int MAX_WORDS = 5;  // maximum number of words i expect in a message from serial

// head force sensor
const int PAT_COOLDOWN = 5;  // in seconds
const int PAT_THRESHOLD = 300;  // minimum force sensor value that is registered as a headpat

// cheek LED
const int BLUSH_SPEED = 5;
const int BLUSH_DURATION = 5;  // in seconds

// arm
const int WAVE_THRESHOLD = 15;  // in degrees. minimum change in angle registered as a wave
const int WAVE_ATTEMPTS = 5;  // how many times to attempt to move arm before assuming i'm blocked by user
const int WAVE_ACCURACY = 10;  // how close to the reported value to try to move the servo

// screen
const int SCREEN_SIZE = 240;
const int MAX_PIXELS = 24*24;  // at most 24x24 chunks
const int EXPRESSION_RESOLUTION = 10;  // how many pixels in either direction
const int EXPRESSION_SIZE = EXPRESSION_RESOLUTION*EXPRESSION_RESOLUTION;  // total pixels
