#include <Arduino.h>
#include <DS3231.h>
#include <Wire.h>

const uint8_t BUS_PIN = 13;
const uint8_t INTERRUPT_PIN = 2;
const uint32_t SAMPLE_RATE_HZ = 8000;
const uint16_t SAMPLE_ON_TICK = 32768 / SAMPLE_RATE_HZ;
const uint16_t DURATION_SEQUENCE_MS = 350;
const uint32_t DURATION_SEQUENCE_TICKS = (DURATION_SEQUENCE_MS * SAMPLE_RATE_HZ) / 1000;

void interruptHandler();
void sample();

DS3231 clock;

void setup(){
  Serial.begin(115200);
  Serial.println("Starting up...");

  Wire.begin();
  pinMode(BUS_PIN, INPUT);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  clock.enable32kHz(true);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), *interruptHandler, RISING);
}

void loop(){
  // put your main code here, to run repeatedly:
}

uint16_t ticks = 0;
void interruptHandler(){
  ticks = (ticks + 1) % (SAMPLE_ON_TICK);
  if (ticks == 0) sample();
}

bool sequenceStarted = false;
uint16_t sequenceLengthTicks = 0;
uint16_t lineLength = 0;
uint16_t cutoff = 200;
void sample(){
  bool isHigh = digitalRead(BUS_PIN);

  if (!sequenceStarted && isHigh) return;
  else sequenceStarted = true;

  if(sequenceLengthTicks < cutoff){
    Serial.print(isHigh);
    if ((++lineLength % 100) == 0) Serial.println();
  }
  
  //Is sequence finished
  if (++sequenceLengthTicks >= DURATION_SEQUENCE_TICKS){
    sequenceStarted = false;
    sequenceLengthTicks = 0;
    lineLength = 0;
    Serial.println();
    Serial.println();
  }
}