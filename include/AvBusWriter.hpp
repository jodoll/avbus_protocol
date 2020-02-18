#ifndef AV_BUS_WRITER_H
#define AV_BUS_WRITER_H

#include "AvBusClock.hpp"
#include "inttypes.h"
#include "project.hpp"

#ifdef STDLIB
#include "Command.hpp"
#endif

constexpr uint8_t COMMAND_LENGTH = 17;
constexpr uint32_t COOLDOWN_DELAY_US = 20000;

class AvBusWriter {
 public:
  AvBusWriter(AvBusClock* clock, uint8_t pin);
  ~AvBusWriter();
  void setCommand(const uint16_t command);
#ifdef STDLIB
  void queueCommand(const Command command);
#endif
  void onClockTick();

 private:
  AvBusClock* clock;
  uint8_t pin;
  uint16_t defaultCommand[COMMAND_LENGTH];
  uint16_t *loadedCommand = nullptr;

  uint8_t commandIndex;
  uint16_t remainingTicksCurrentPhase;

#ifdef STDLIB
  std::vector<Command> commandQueue;
#endif

  void loadNextCommand();
  void printCommand(uint16_t *commandArray);
};

#endif  // AV_BUS_WRITER_H