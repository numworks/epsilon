#ifndef ESCHER_TIMER_H
#define ESCHER_TIMER_H

#include <escher/invocation.h>
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
  static constexpr int TickDuration = 100; // In Miliseconds
  Timer(Invocation invocation, uint32_t period); // Period is in ticks
  void tick();
  void reset();
private:
  void fire();
  Invocation m_invocation;
  uint32_t m_period;
  uint32_t m_numberOfTicksBeforeFire;
};

#endif
