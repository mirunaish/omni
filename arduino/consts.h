#pragma once

/* --------- consts --------- */

class Consts {
  public:
    static const int frameDelay = 30;  // in ms

    // head force sensor
    static const int patCooldown = 5;  // in seconds
    static const int patThreshold = 300;  // minimum force sensor value that is registered as a headpat

    // cheek LED
    static const int blushSpeed = 5;
    static const int blushDuration = 5;  // in seconds

    // arm
    static const int neutralAngle = 90;    
};