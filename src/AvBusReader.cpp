#include "AvBusReader.hpp"
#include "Arduino.h"

void AvBusReader::onBusValueChanged() {
  bool isHigh = digitalRead(pin);

  if (!sequenceStarted && isHigh) {
    return;
  } else if (!sequenceStarted) {
    clock.reset();
    sequenceStarted = true;
  }

  sequenceTimes[sequenceIndex] = clock.time();
  busValues[sequenceIndex] = isHigh;
  sequenceIndex = (sequenceIndex + 1) % 18;

  if (sequenceIndex == 0) {
    Serial.println("-> Received Message");
    printRelativeTimes();
    printCommand();
    Serial.println();
    sequenceStarted = false;
  }
}

void AvBusReader::printAbsoluteTimes() {
  Serial.println("Absolute Times:");
  for (uint8_t i = 0; i < 18; i++) {
    Serial.print(sequenceTimes[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void AvBusReader::printRelativeTimes() {
  Serial.println("Relative Times:");
  for (uint8_t i = 0; i < 18; i++) {
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i - 1, 0)];
    Serial.print(duration);
    Serial.print(" ");
  }
  Serial.println();
}

void AvBusReader::printBusValues() {
  Serial.println("Bus Values:");
  for (uint8_t i = 0; i < 17; i++) {
    uint32_t duration = sequenceTimes[i + 1] - sequenceTimes[i];
    for (uint16_t j = 0; j < min(duration / 250, 30); j++) {
      Serial.print(busValues[i]);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void AvBusReader::printCommand() {
  Serial.print("Device:  ");
  for (uint8_t i = 2; i < 2 + 6; i++) {
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i - 1, 0)];

    if (i > 2 && (i - 2) % 4 == 0) Serial.print(" ");
    Serial.print(duration > 500);
  }
  Serial.println();

  Serial.print("Command: ");
  for (uint8_t i = 8; i < 17; i++) {
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i - 1, 0)];

    if (i > 8 && i % 4 == 0) Serial.print(" ");
    Serial.print(duration > 500);
  }
  Serial.println();
}