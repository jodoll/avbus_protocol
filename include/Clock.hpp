#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include <DS3231.h>

constexpr uint32_t CLOCK_SIGNAL_HZ = 32768;

class Clock{
public:
    Clock(const uint32_t frequency, const uint8_t pin): frequency(frequency), pin(pin){}
    ~Clock(){}
    void init();
    void reset();
    uint32_t time();
    void tick();

private:
    const uint32_t frequency;
    const uint16_t countOnTick = CLOCK_SIGNAL_HZ / frequency;
    const uint16_t resolutionUs = 1000000 / frequency;

    DS3231 clock;

    uint8_t pin;
    uint32_t currentTime = 0;
    uint16_t ticks = 0;
};

#endif //CLOCK_H