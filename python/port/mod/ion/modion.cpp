extern "C" {
#include "modion.h"
#include <py/objtuple.h>
#include <py/runtime.h>
}
#include <ion.h>
#include "port.h"

mp_obj_t modion_keyboard_keydown(mp_obj_t key_o) {
  Ion::Keyboard::Key key = static_cast<Ion::Keyboard::Key>(mp_obj_get_int(key_o));
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  micropython_port_interrupt_if_needed();
  return mp_obj_new_bool(state.keyDown(key));
}

mp_obj_t modion_battery() {
  return mp_obj_new_float(Ion::Battery::voltage());
}

mp_obj_t modion_battery_level(){
    return mp_obj_new_int(static_cast<int>(Ion::Battery::level()));
}

mp_obj_t modion_battery_ischarging(){
    return mp_obj_new_bool(Ion::Battery::isCharging());
}