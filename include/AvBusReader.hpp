#ifndef READ_H
#define READ_H

#include "AvBusClock.hpp"
#include "inttypes.h"

class AvBusReader {
 public:
  AvBusReader(AvBusClock &clock, uint8_t pin) : clock(clock), pin(pin) {}
  ~AvBusReader() {}
  void onBusValueChanged();

 private:
  AvBusClock &clock;
  const uint8_t pin;

  bool sequenceStarted = false;
  uint8_t sequenceIndex = 0;
  uint32_t sequenceTimes[18];
  bool busValues[18];

  void printAbsoluteTimes();
  void printRelativeTimes();
  void printBusValues();
  void printCommand();
};

#endif  // READ_H