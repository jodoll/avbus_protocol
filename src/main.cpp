#include <Arduino.h>
#include <Wire.h>
#include "AvBusReader.hpp"
#include "AvBusWriter.hpp"
#include "Clock.hpp"

constexpr uint8_t BUS_INTERRUPT_PIN = 3;
constexpr uint8_t BUS_SEND_PIN = 4;
constexpr uint8_t CLOCK_INTERRUPT_PIN = 2;

constexpr uint32_t CLOCK_FREQUENCY_HZ = 8000;

void clockInterruptHandler();
void busInterruptHandler();
void onClockTick();

Clock clock(CLOCK_FREQUENCY_HZ, CLOCK_INTERRUPT_PIN);
AvBusReader reader(clock, BUS_INTERRUPT_PIN);
AvBusWriter writer(clock, BUS_SEND_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up...");

  Wire.begin();
  clock.init();

  pinMode(BUS_SEND_PIN, OUTPUT);
  // Tune up
  writer.loadCommand(0b010110101010100);
  clock.registerTickCallback(&onClockTick);

  attachInterrupt(digitalPinToInterrupt(BUS_INTERRUPT_PIN), &busInterruptHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), &clockInterruptHandler, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void clockInterruptHandler() { clock.tick(); }

void busInterruptHandler() { reader.onBusValueChanged(); }

void onClockTick() { writer.onClockTick(); }
