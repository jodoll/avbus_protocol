#ifndef AV_BUS_WRITER_H
#define AV_BUS_WRITER_H

#include "Clock.hpp"
#include "inttypes.h"

constexpr uint8_t COMMAND_LENGTH = 18;
constexpr uint32_t REPEAT_DELAY_US = 1000000;

class AvBusWriter {
 public:
  AvBusWriter(Clock &clock, uint8_t pin);
  ~AvBusWriter();
  void loadCommand(const uint16_t command);
  void onClockTick();

 private:
  Clock &clock;
  uint8_t pin;
  uint16_t loadedCommand[COMMAND_LENGTH];

  uint8_t commandIndex;
  uint16_t remainingTicksCurrentPhase;

  void printCommand(uint16_t *commandArray);
};

#endif  // AV_BUS_WRITER_H