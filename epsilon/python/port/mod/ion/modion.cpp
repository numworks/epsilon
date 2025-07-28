extern "C" {
#include "modion.h"

#include <py/objtuple.h>
#include <py/runtime.h>
}
#include <ion.h>

#include "port.h"

mp_obj_t modion_keyboard_keydown(mp_obj_t key_o) {
  Ion::Keyboard::Key key =
      static_cast<Ion::Keyboard::Key>(mp_obj_get_int(key_o));
  if (key >= Ion::Keyboard::Key::None) {
    return mp_obj_new_bool(false);
  }
  /* In version 22, scan was greatly sped up. We wait 1 ms after the scan to
   * keep the keydown function consistent with older versions.
   * WARNING: Do not use Timing::msleep to avoid putting the device to sleep,
   * which alters timings (mostly on N120) */
  uint64_t currentTime = Ion::Timing::millis();
  Ion::Keyboard::State state = Ion::Keyboard::scan(true);
  while (currentTime + 1 > Ion::Timing::millis()) {}
  micropython_port_interrupt_if_needed();
  return mp_obj_new_bool(state.keyDown(key));
}
