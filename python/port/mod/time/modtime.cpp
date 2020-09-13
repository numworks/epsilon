extern "C" {
#include "modtime.h"
#include <py/runtime.h>
}
#include <ion/timing.h>
#include "../../helpers.h"
#include <py/smallint.h>

mp_obj_t modtime_sleep(mp_obj_t seconds_o) {
#if MICROPY_PY_BUILTINS_FLOAT
  mp_float_t tts = 1000 * mp_obj_get_float(seconds_o);
#else
  mp_int_t tts = 1000 * mp_obj_get_int(seconds_o);
#endif
  if(tts < 0){
    mp_raise_ValueError("sleep length must be non-negative");
  }
  micropython_port_interruptible_msleep(tts);
  return mp_const_none;
}

mp_obj_t modtime_monotonic() {
    return mp_obj_new_float(Ion::Timing::millis() / 1000.0);
}
