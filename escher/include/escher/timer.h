#ifndef ESCHER_TIMER_H
#define ESCHER_TIMER_H

#include <stdint.h>

/* Timers we'll need
 * - Blink cursor timer
 * - Dim Screen timer
 * - Power down timer
 * - Watchdog timer ?
 * - Battery level timer
 * - LED blink timer
 */

class Timer {
public:
  static constexpr int TickDuration = 300; // In Miliseconds
  constexpr Timer(uint32_t period) : m_period(period), m_numberOfTicksBeforeFire(period) {} // Period is in ticks
  bool tick();
  void reset() { m_numberOfTicksBeforeFire = m_period; }
protected:
  virtual bool fire() = 0;
private:
  uint32_t m_period;
  uint32_t m_numberOfTicksBeforeFire;
};

#endif
