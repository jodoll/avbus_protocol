#include <Arduino.h>
#include <Wire.h>
#include "Clock.hpp"
#include "AvBusReader.hpp"

#define SEND_MODE 1

constexpr uint8_t BUS_INTERRUPT_PIN = 3;
constexpr uint8_t BUS_SEND_PIN = 4;
constexpr uint8_t CLOCK_INTERRUPT_PIN = 2;

constexpr uint32_t CLOCK_FREQUENCY_HZ = 8000;

void clockInterruptHandler();
void busInterruptHandler();
void translateCommand();

Clock clock(CLOCK_FREQUENCY_HZ, CLOCK_INTERRUPT_PIN);
AvBusReader reader(clock, BUS_INTERRUPT_PIN);

void setup(){
  Serial.begin(115200);
  Serial.println("Starting up...");
  
  Wire.begin();
  clock.init();

  pinMode(BUS_SEND_PIN, OUTPUT);
  digitalWrite(BUS_SEND_PIN, HIGH);
  translateCommand();

  attachInterrupt(digitalPinToInterrupt(BUS_INTERRUPT_PIN), &busInterruptHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), &clockInterruptHandler, RISING);
}

void loop(){
  // put your main code here, to run repeatedly:
}

void clockInterruptHandler(){
  clock.tick();
}

void busInterruptHandler(){
  reader.onBusValueChanged();
}

const uint8_t PREAMBLE_PULLDOWN_TICKS = 15;
const uint8_t ONE_TICKS = 2;
const uint8_t ZERO_TICKS = 1;
const uint16_t POSTAMBLE_PULLDOWN_TICKS = 15;
const uint16_t COOLDOWN_TICKS = 5000;

uint8_t currentCommandIndex = 0;
uint16_t command[18];
uint16_t remainingPhaseDuration;
bool nextValue = false;


/* 
Increase  volume
Device:  0110 01
Command: 0101 0101 1

Tuning Up
Device:  0101 10
Command: 1010 1010 0
*/
void translateCommand(){
  uint16_t rawCommand = 0b010110101010100;
  command[0] = COOLDOWN_TICKS * 3;
  command[1]= PREAMBLE_PULLDOWN_TICKS * 3;
  for(int i = 0; i<15; i++){
    command[i+2] = (((rawCommand >> (14-i)) & 0b1) + 1)*3;
  }
  command[17] = POSTAMBLE_PULLDOWN_TICKS * 3;
  remainingPhaseDuration = command[currentCommandIndex];

  Serial.print("Translated Command to: ");
  for(uint8_t i = 0; i < 18; i++){
    Serial.print(command[i]);
    Serial.print(" ");
  }
  Serial.println();
}

uint32_t lastTime;
void tick(){
  if(currentCommandIndex >= 18) return;
  if(--remainingPhaseDuration>0) return;
  //Serial.print("Sending ");
  //Serial.print(nextValue);
  //Serial.print(" after ");
  //Serial.println(sequenceDurationUs - lastTime);
  digitalWrite(BUS_SEND_PIN, nextValue);
  nextValue = !nextValue;
  remainingPhaseDuration = command[(++currentCommandIndex)%18];
  currentCommandIndex = currentCommandIndex%18;
  if(currentCommandIndex == 0){
    Serial.println("<- Sent message");
    Serial.println();
  }
  //lastTime = clock.time();
}