extern "C" {
#include "modtime.h"
}
#include <ion/timing.h>
#include "../../helpers.h"
#include <py/smallint.h>
#include <py/runtime.h>

mp_obj_t modtime_sleep(mp_obj_t seconds_o) {
#if MICROPY_PY_BUILTINS_FLOAT
  micropython_port_interruptible_msleep(1000 * mp_obj_get_float(seconds_o));
#else
  micropython_port_interruptible_msleep(1000 * mp_obj_get_int(seconds_o));
#endif
  return mp_const_none;
}

mp_obj_t modtime_monotonic() {
    return mp_obj_new_float(Ion::Timing::millis() / 1000.0);
}
