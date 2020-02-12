#include "AvBusClock.hpp"

void AvBusClock::init() {
  pinMode(pin, INPUT_PULLUP);
  clock.enable32kHz(true);
}

void AvBusClock::reset() {
  currentTime = 0;
  ticks = 0;
}

uint32_t AvBusClock::time() { return currentTime; }

void AvBusClock::registerTickCallback(void (*onTick)()) { this->onTick = onTick; }

void AvBusClock::tick() {
  ticks = (ticks + 1) % (countOnTick);
  if (ticks == 0) {
    currentTime += resolutionUs;
    if (onTick != nullptr) onTick();
  }
}