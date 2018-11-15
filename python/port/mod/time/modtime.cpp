extern "C" {
#include "modtime.h"
}
#include <ion/timing.h>

#include "py/smallint.h"
#include "py/runtime.h"

static void delay_ms(mp_uint_t delay) {
  uint32_t start = Ion::Timing::millis();
  while (Ion::Timing::millis() - start < delay && !micropython_port_should_interrupt()) {
    Ion::Timing::msleep(1);
  }
}

mp_obj_t modtime_sleep(mp_obj_t seconds_o) {
#if MICROPY_PY_BUILTINS_FLOAT
  delay_ms((mp_uint_t)(1000 * mp_obj_get_float(seconds_o)));
#else
  delay_ms(1000 * mp_obj_get_int(seconds_o));
#endif
  return mp_const_none;
}

mp_obj_t modtime_monotonic(void) {
    return mp_obj_new_float(Ion::Timing::millis() / 1000.0);
}
