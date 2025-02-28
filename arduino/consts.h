#pragma once

/* --------- consts --------- */

class Consts {
  public:
    static const int frameDelay = 30;  // in ms
    static const int maxWords = 5;  // maximum number of words i expect in a message from serial

    // head force sensor
    static const int patCooldown = 5;  // in seconds
    static const int patThreshold = 300;  // minimum force sensor value that is registered as a headpat

    // cheek LED
    static const int blushSpeed = 5;
    static const int blushDuration = 5;  // in seconds

    // arm
    static const int neutralAngle = 90;    
    static const int waveThreshold = 5;  // in degrees. minimum change in angle registered as a wave
    static const int waveAttempts = 5;  // how many times to attempt to move arm before assuming i'm blocked by user
    static const int waveAccuracy = 3;  // how close to the reported value to try to move the servo
};