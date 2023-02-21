extern "C" {
#include "modmatplotlib.h"

#include "pyplot/modpyplot.h"
}
#include <assert.h>

// Internal functions

mp_obj_t modmatplotlib___init__() {
  modpyplot___init__();
  return mp_const_none;
}
