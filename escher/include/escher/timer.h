#ifndef ESCHER_TIMER_H
#define ESCHER_TIMER_H

#include <stdint.h>

/**
 * A timer that can be used to schedule events.
 * We organize timers in a linked list.
 */
class Timer {
public:
  static constexpr int TickDuration = 300; // In Miliseconds
  Timer(uint32_t period); // Period is in ticks
  bool tick();
  void reset(uint32_t NewPeriod = -1);
  void setNext(Timer * next) { m_next = next; }
  Timer * next() { return m_next; }
protected:
  virtual bool fire() = 0;
  uint32_t m_period;
  uint32_t m_numberOfTicksBeforeFire;
private:
  Timer * m_next;
};

#endif
