#ifndef AV_BUS_WRITER_H
#define AV_BUS_WRITER_H

#include "AvBusClock.hpp"
#include "inttypes.h"
#include "project.hpp"

#ifdef ESP32
#include "esp/Command.hpp"
#endif

constexpr uint8_t COMMAND_LENGTH = 17;
constexpr uint32_t COOLDOWN_DELAY_US = 20000;

class AvBusWriter {
 public:
  AvBusWriter(AvBusClock *clock, uint8_t pin);
  ~AvBusWriter();
#ifdef ESP32
  void queueCommand(const Command &command);
#else
  void setCommand(const uint16_t command);
#endif
  void onClockTick();

 private:
  AvBusClock *clock;
  uint8_t pin;
  uint16_t *loadedCommand = nullptr;

  uint8_t commandIndex;
  uint16_t remainingTicksCurrentPhase;

#ifdef ESP32
  std::vector<Command> commandQueue;
  std::vector<uint16_t> commandTimings;
#else
  uint16_t commandTimings[COMMAND_LENGTH];
#endif

  void loadNextCommand();
  void printCommand(uint16_t *commandArray);
};

#endif  // AV_BUS_WRITER_H