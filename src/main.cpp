#include <Arduino.h>
#include <DS3231.h>
#include <Wire.h>

const uint8_t BUS_INTERRUPT_PIN = 3;
const uint8_t CLOCK_INTERRUPT_PIN = 2;

const uint32_t COUNT_RATE_HZ = 8000;
const uint16_t COUNT_ON_TICK = 32768 / COUNT_RATE_HZ;
const uint16_t CLOCK_RESOLUTION_US = 1000000 / COUNT_RATE_HZ;

void clockInterruptHandler();
void resetClock();
void busInterruptHandler();
void printAbsoluteTimes();
void printRelativeTimes();
void printBusValues();
void printCommand();

DS3231 clock;

void setup(){
  Serial.begin(115200);
  Serial.println("Starting up...");

  Wire.begin();
  pinMode(BUS_INTERRUPT_PIN, INPUT);
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  clock.enable32kHz(true);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), *clockInterruptHandler, RISING);
  attachInterrupt(digitalPinToInterrupt(BUS_INTERRUPT_PIN), *busInterruptHandler, CHANGE);
}

void loop(){
  // put your main code here, to run repeatedly:
}

uint32_t sequenceDurationUs = 0;
uint16_t ticks = 0;
void clockInterruptHandler(){
  ticks = (ticks + 1) % (COUNT_ON_TICK);
  if (ticks == 0) {
    sequenceDurationUs += CLOCK_RESOLUTION_US;
  }
}

void resetClock(){
  ticks = 0;
  sequenceDurationUs = 0;
}

bool sequenceStarted = false;
uint8_t sequenceIndex = 0;
uint32_t sequenceTimes[18];
bool busValues[18];
void busInterruptHandler(){
  bool isHigh = digitalRead(BUS_INTERRUPT_PIN);

  if (!sequenceStarted && isHigh) return;
  else if (!sequenceStarted){
    resetClock();
    sequenceStarted = true;
  }

  sequenceTimes[sequenceIndex] = sequenceDurationUs;
  busValues[sequenceIndex] = isHigh;
  sequenceIndex = (sequenceIndex +1) % 18;
  
  if(sequenceIndex == 0){
    printAbsoluteTimes();
    printRelativeTimes();
    printBusValues();
    printCommand();

    Serial.println();
    sequenceStarted = false;
  }
}

void printAbsoluteTimes(){
  Serial.println("Absolute Times:");
  for(uint8_t i = 0; i < 18; i++){
    Serial.print(sequenceTimes[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void printRelativeTimes(){
  Serial.println("Relative Times:");
  for(uint8_t i = 0; i < 18; i++){
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i-1, 0)];
    Serial.print(duration);
    Serial.print(" ");
  }
  Serial.println();
}

void printBusValues(){
  Serial.println("Bus Values:");
  for(uint8_t i = 2; i < 17; i++){
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i-1, 0)];
    for(uint16_t j = 0; j< min(duration/125, 10); j++){
      Serial.print(busValues[i]);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void printCommand(){
  Serial.println("Command:");
  for(uint8_t i = 2; i < 17; i++){
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i-1, 0)];
    
    if(i > 2 && (i - 2) % 4 == 0) Serial.print(" ");
    Serial.print(duration > 500);
  }
  Serial.println();
}