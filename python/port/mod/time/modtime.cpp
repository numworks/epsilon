extern "C" {
#include "modtime.h"
}
#include <ion/timing.h>
#include <limits.h>
#include <py/runtime.h>
#include <py/smallint.h>

#include "../../helpers.h"

mp_obj_t modtime_sleep(mp_obj_t seconds_o) {
#if MICROPY_PY_BUILTINS_FLOAT
  mp_float_t duration = mp_obj_get_float(seconds_o);
#else
  mp_int_t duration = mp_obj_get_int(seconds_o);
#endif
  int32_t duration32 =
      duration < 0 ? 0
                   : (duration > INT_MAX / 1000 ? INT_MAX : 1000 * duration);
  micropython_port_interruptible_msleep(duration32);
  return mp_const_none;
}

mp_obj_t modtime_monotonic() {
  return mp_obj_new_float(Ion::Timing::millis() / 1000.0);
}
