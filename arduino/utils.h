#pragma once

int getFrames(float durationInSeconds) {
  return (int) (durationInSeconds * 1000 / Consts::frameDelay);
}