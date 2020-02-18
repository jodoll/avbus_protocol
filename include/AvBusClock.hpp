#ifndef AV_BUS_CLOCK_H
#define AV_BUS_CLOCK_H

#include <Arduino.h>
#include <inttypes.h>

class AvBusClock {
 public:
  virtual void init(void (*clockInterruptHandler)());
  virtual void reset();
  virtual void tick();

  void registerTickCallback(void (*onTick)());

  virtual uint32_t time() const;
  virtual uint16_t getResolutionUs() const;

 protected:
  void (*onTick)() = nullptr;
};

#endif  // AV_BUS_CLOCK_H