#include "AvBusWriter.hpp"
#include "Timings.hpp"
#include "project.hpp"

AvBusWriter::AvBusWriter(AvBusClock *clock, uint8_t pin) : clock(clock), pin(pin) {}

AvBusWriter::~AvBusWriter() { clock->registerTickCallback(nullptr); }

#ifdef ESP32
void AvBusWriter::queueCommand(const Command &command) { commandQueue.push_back(command); }
#else
void AvBusWriter::setCommand(const uint16_t command) {
  loadedCommand = commandTimings;
  // Delay between commands
  loadedCommand[0] = COOLDOWN_DELAY_US;
  // INIT
  loadedCommand[1] = INIT_DURATION_US;
  // Command
  for (int i = 0; i < 15; i++) {
    bool isHigh = (command >> (14 - i)) & 0b1;
    uint16_t delay;
    if (isHigh) {
      delay = HIGH_DURATION_US;
    } else {
      delay = LOW_DURATION_US;
    }
    loadedCommand[i + 2] = delay;
  }
  // HOLD
  loadedCommand[17] = HOLD_DURATION_US;
  // Reset position in command
  commandIndex = 0;
  remainingTicksCurrentPhase = loadedCommand[0];

  if (VERBOSE) printCommand(loadedCommand);
}
#endif

void AvBusWriter::loadNextCommand() {
#ifdef ESP32
  if (!commandQueue.empty()) {
    if (VERBOSE) Serial.println("Loading new command");
    Command command = commandQueue.front();
    commandQueue.erase(commandQueue.begin());
    commandTimings = command.getTimings();
    loadedCommand = commandTimings.data();
    commandIndex = 0;
    remainingTicksCurrentPhase = loadedCommand[0] / clock->getResolutionUs();
  }
#endif
}

void AvBusWriter::printCommand(uint16_t *commandArray) {
  Serial.print("Command: ");
  for (uint8_t i = 0; i < COMMAND_LENGTH; i++) {
    Serial.print(commandArray[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void AvBusWriter::onClockTick() {
  // Decrease time, if some is left
  remainingTicksCurrentPhase = max(remainingTicksCurrentPhase - 1, 0);
  if (remainingTicksCurrentPhase > 0) return;

  // Try to load next command
  if (loadedCommand == nullptr) {
    digitalWrite(pin, HIGH);
    loadNextCommand();
    if (loadedCommand != nullptr) {
      digitalWrite(pin, LOW);
    }
    return;
  }

  // Write next value
  bool nextBusValue = (commandIndex + 1) % 2;
  digitalWrite(pin, nextBusValue);

  // Read next delay
  commandIndex++;
  remainingTicksCurrentPhase = loadedCommand[commandIndex % COMMAND_LENGTH] / clock->getResolutionUs();
  // Send command again if finished
  commandIndex = commandIndex % COMMAND_LENGTH;
  // Log completion of command
  if (commandIndex == 0) {
    if (VERBOSE) {
      Serial.println("<- Sent message");
      printCommand(loadedCommand);
    }
    loadedCommand = nullptr;
    remainingTicksCurrentPhase = COOLDOWN_DELAY_US / clock->getResolutionUs();
  }
}
