#pragma once

int getFrames(float durationInSeconds) {
  return (int) (durationInSeconds * 1000 / FRAME_DELAY);
}

/** chatgpt wrote this vvvv */
uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
}