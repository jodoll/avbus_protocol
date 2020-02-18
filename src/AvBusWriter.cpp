#include "AvBusWriter.hpp"
#include "Timings.hpp"

AvBusWriter::AvBusWriter(AvBusClock* clock, uint8_t pin) : clock(clock), pin(pin) {}

AvBusWriter::~AvBusWriter() { clock->registerTickCallback(nullptr); }

void AvBusWriter::setCommand(const uint16_t command) {
  loadedCommand = defaultCommand;
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

  printCommand(loadedCommand);
}

#ifdef STDLIB
void AvBusWriter::queueCommand(const Command command) {commandQueue.push_back(command);}
#endif

void AvBusWriter::loadNextCommand() {
#ifdef STDLIB
  if (!commandQueue.empty()) {
    Serial.println("Loading new command");
    Command command = commandQueue.front();
    commandQueue.erase(commandQueue.begin());
    std::vector<uint16_t> timings = command.getTimings();
    memccpy(defaultCommand, timings.data(), 0, timings.size() * sizeof(uint16_t));
    loadedCommand = defaultCommand;
    commandIndex = 0;
    remainingTicksCurrentPhase = loadedCommand[0] / clock->resolutionUs;
  }
#endif
}

void AvBusWriter::printCommand(uint16_t *commandArray) {
  Serial.print("Command: ");
  for (uint8_t i = 0; i < COMMAND_LENGTH; i++) {
    Serial.print(commandArray[i]);
    Serial.print(" ");
  }
}

void AvBusWriter::onClockTick() {
  // Decrease time, if some is left
  if (remainingTicksCurrentPhase > 0) {
    remainingTicksCurrentPhase--;
    return;
  }

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
  bool nextBusValue = commandIndex % 2;
  digitalWrite(pin, nextBusValue);

  // Read next delay
  commandIndex++;
  remainingTicksCurrentPhase = loadedCommand[commandIndex % COMMAND_LENGTH] / clock->resolutionUs;
  // Send command again if finished
  commandIndex = commandIndex % COMMAND_LENGTH;
  // Log completion of command
  if (commandIndex == 0) {
    printCommand(loadedCommand);
    Serial.println("<- Sent message");
    loadedCommand = nullptr;
    remainingTicksCurrentPhase = COOLDOWN_DELAY_US / clock->resolutionUs;
  }
}
