#include "AvBusClock.hpp"

void AvBusClock::registerTickCallback(void (*onTick)()) { this->onTick = onTick; }