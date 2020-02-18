#include "esp/EspAvBusClock.hpp"

void EspAvBusClock::init(void (*clockInterruptHandler)()) {
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, clockInterruptHandler, true);
  timerAlarmWrite(timer, resolutionUs, true);
  timerAlarmEnable(timer);
}

void EspAvBusClock::reset() { currentTime = 0; }

uint32_t EspAvBusClock::time() const { return currentTime; }

uint16_t EspAvBusClock::getResolutionUs() const { return resolutionUs; }

void EspAvBusClock::tick() {
  currentTime++;
  if (onTick != nullptr) onTick();
}