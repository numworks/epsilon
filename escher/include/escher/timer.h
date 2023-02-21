#ifndef ESCHER_TIMER_H
#define ESCHER_TIMER_H

#include <stdint.h>

namespace Escher {

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
  constexpr static int TickDuration = 300;  // In Miliseconds
  Timer(uint32_t period);                   // Period is in ticks
  bool tick();
  void reset();

 protected:
  virtual bool fire() = 0;

 private:
  uint32_t m_period;
  uint32_t m_numberOfTicksBeforeFire;
};

}  // namespace Escher
#endif
