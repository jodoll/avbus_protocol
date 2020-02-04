#include <Arduino.h>
#include <DS3231.h>
#include <Wire.h>

#define SEND_MODE 1

const uint8_t BUS_INTERRUPT_PIN = 3;
const uint8_t BUS_SEND_PIN = 4;
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
void translateCommand();
void tick();

DS3231 clock;

void setup(){
  Serial.begin(115200);
  Serial.println("Starting up...");

  Wire.begin();
  pinMode(BUS_SEND_PIN, OUTPUT);
  digitalWrite(BUS_SEND_PIN, HIGH);
  translateCommand();

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
    #if SEND_MODE
    tick();
    #endif
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
    Serial.println("-> Received Message");
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
  for(uint8_t i = 0; i < 17; i++){
    uint32_t duration = sequenceTimes[i+1] - sequenceTimes[i];
    for(uint16_t j = 0; j< min(duration/250, 30); j++){
      Serial.print(busValues[i]);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void printCommand(){
  Serial.print("Device:  ");
  for(uint8_t i = 2; i < 2+6; i++){
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i-1, 0)];
    
    if(i > 2 && (i - 2) % 4 == 0) Serial.print(" ");
    Serial.print(duration > 500);
  }
  Serial.println();
  
  Serial.print("Command: ");
  for(uint8_t i = 8; i < 17; i++){
    uint32_t duration = sequenceTimes[i] - sequenceTimes[max(i-1, 0)];
    
    if(i > 8 && i % 4 == 0) Serial.print(" ");
    Serial.print(duration > 500);
  }
  Serial.println();
}

const uint8_t PREAMBLE_PULLDOWN_TICKS = 27;
const uint8_t ONE_TICKS = 2;
const uint8_t ZERO_TICKS = 1;
const uint16_t POSTAMBLE_PULLDOWN_TICKS = 768;
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
    resetClock();
  }
  lastTime = sequenceDurationUs;
}