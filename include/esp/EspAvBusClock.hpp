#ifndef ESP_AV_BUS_CLOCK_H
#define ESP_AV_BUS_CLOCK_H

#include <Arduino.h>
#include "AvBusClock.hpp"

class EspAvBusClock : public AvBusClock {
 public:
  EspAvBusClock(uint16_t frequencyHz) : resolutionUs(1000000 / frequencyHz) {}
  ~EspAvBusClock() {}
  void init(void (*clockInterruptHandler)()) override;
  void reset() override;
  void tick() override;

  uint32_t time() const override;
  uint16_t getResolutionUs() const override;

 private:
  uint16_t resolutionUs;
  hw_timer_t* timer = nullptr;
  uint32_t currentTime = 0;
};

#endif  // ESP_AV_BUS_CLOCK_H