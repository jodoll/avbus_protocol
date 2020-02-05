#include "AvBusWriter.hpp"
#include "Timings.hpp"

/*
Increase  volume
Device:  0110 01
Command: 0101 0101 1

Tuning Up
Device:  0101 10
Command: 1010 1010 0
*/

AvBusWriter::AvBusWriter(Clock &clock, uint8_t pin) : clock(clock), pin(pin) {}

AvBusWriter::~AvBusWriter() { clock.registerTickCallback(nullptr); }

void AvBusWriter::loadCommand(const uint16_t command) {
  // Delay between commands
  loadedCommand[0] = REPEAT_DELAY_US / clock.resolutionUs;
  // INIT
  loadedCommand[1] = INIT_DURATION_US / clock.resolutionUs;
  // Command
  for (int i = 0; i < 15; i++) {
    bool isHigh = (command >> (14 - i)) & 0b1;
    uint16_t delay;
    if (isHigh) {
      delay = HIGH_DURATION_US;
    } else {
      delay = LOW_DURATION_US;
    }
    loadedCommand[i + 2] = delay / clock.resolutionUs;
  }
  // HOLD
  loadedCommand[17] = HOLD_DURATION_US / clock.resolutionUs;
  // Reset position in command
  commandIndex = 0;
  remainingTicksCurrentPhase = loadedCommand[0];

  printCommand(loadedCommand);
}

void AvBusWriter::printCommand(uint16_t *commandArray) {
  Serial.print("Command: ");
  for (uint8_t i = 0; i < 18; i++) {
    Serial.print(commandArray[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void AvBusWriter::onClockTick() {
  if (commandIndex >= COMMAND_LENGTH) return;
  if (--remainingTicksCurrentPhase > 0) return;
  // Write next value
  bool nextBusValue = commandIndex % 2;
  digitalWrite(pin, nextBusValue);
  // Read next delay
  commandIndex++;
  remainingTicksCurrentPhase = loadedCommand[commandIndex % COMMAND_LENGTH];
  // Send command again if finished
  commandIndex = commandIndex % COMMAND_LENGTH;
  // Log completion of command
  if (commandIndex == 0) {
    Serial.println("<- Sent message");
    Serial.println();
  }
}
