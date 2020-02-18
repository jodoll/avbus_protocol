#include "uno/UnoAvBusClock.hpp"

void UnoAvBusClock::init(void (*clockInterruptHandler)()) {
  Wire.begin();
  clock.enable32kHz(true);
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), clockInterruptHandler, RISING);
}

void UnoAvBusClock::reset() {
  currentTime = 0;
  ticks = 0;
}

uint32_t UnoAvBusClock::time() const { return currentTime; }

uint16_t UnoAvBusClock::getResolutionUs() const { return resolutionUs; }

void UnoAvBusClock::tick() {
  ticks = (ticks + 1) % (countOnTick);
  if (ticks == 0) {
    currentTime += resolutionUs;
    if (onTick != nullptr) onTick();
  }
}