#ifndef UNO_AV_BUS_CLOCK_H
#define UNO_AV_BUS_CLOCK_H

#include <Arduino.h>
#include <DS3231.h>
#include "AvBusClock.hpp"

class UnoAvBusClock : public AvBusClock {
 public:
  static constexpr uint32_t CLOCK_SIGNAL_HZ = 32768;

  UnoAvBusClock(const uint32_t frequency, const uint8_t pin) : frequency(frequency), pin(pin) {}
  ~UnoAvBusClock() {}
  void init(void (*clockInterruptHandler)()) override;
  void reset() override;
  void tick() override;

  uint32_t time() const override;
  uint16_t getResolutionUs() const override;

 private:
  const uint32_t frequency;
  const uint16_t countOnTick = CLOCK_SIGNAL_HZ / frequency;
  const uint16_t resolutionUs = 1000000 / frequency;

  DS3231 clock;
  uint8_t pin;
  uint32_t currentTime = 0;
  uint16_t ticks = 0;
};

#endif  // UNO_AV_BUS_CLOCK_H