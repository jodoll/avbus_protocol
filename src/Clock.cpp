#include "Clock.hpp"

void Clock::init() {
  pinMode(pin, INPUT_PULLUP);
  clock.enable32kHz(true);
}

void Clock::reset() {
  currentTime = 0;
  ticks = 0;
}

uint32_t Clock::time() { return currentTime; }

void Clock::registerTickCallback(void (*onTick)()) { this->onTick = onTick; }

void Clock::tick() {
  ticks = (ticks + 1) % (countOnTick);
  if (ticks == 0) {
    currentTime += resolutionUs;
    if (onTick != nullptr) onTick();
  }
}