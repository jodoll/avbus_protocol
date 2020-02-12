#include <Arduino.h>
#include <Wire.h>
#include "AvBusClock.hpp"
#include "AvBusReader.hpp"
#include "AvBusWriter.hpp"

#if !defined(UNO) && !defined(ESP32)
#define UNO
#endif

#if defined(UNO)
constexpr uint8_t BUS_INTERRUPT_PIN = 3;
constexpr uint8_t BUS_SEND_PIN = 4;
constexpr uint8_t CLOCK_INTERRUPT_PIN = 2;
#elif defined(ESP32)
constexpr uint8_t BUS_INTERRUPT_PIN = 2;
constexpr uint8_t BUS_SEND_PIN = 39;
constexpr uint8_t CLOCK_INTERRUPT_PIN = 15;
#endif

constexpr uint32_t CLOCK_FREQUENCY_HZ = 8000;

void clockInterruptHandler();
void busInterruptHandler();
void onClockTick();

AvBusClock avBusClock(CLOCK_FREQUENCY_HZ, CLOCK_INTERRUPT_PIN);
AvBusReader reader(avBusClock, BUS_INTERRUPT_PIN);
AvBusWriter writer(avBusClock, BUS_SEND_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up...");

  Wire.begin();
  avBusClock.init();

  pinMode(BUS_INTERRUPT_PIN, INPUT_PULLUP);
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  pinMode(BUS_SEND_PIN, OUTPUT);
  // Tune up
  writer.loadCommand(0b010110101010100);
  // clock.registerTickCallback(&onClockTick);

  attachInterrupt(digitalPinToInterrupt(BUS_INTERRUPT_PIN), &busInterruptHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), &clockInterruptHandler, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void clockInterruptHandler() { avBusClock.tick(); }

void busInterruptHandler() { 
  reader.onBusValueChanged(); }

void onClockTick() { writer.onClockTick(); }
