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
  return MP_OBJ_NEW_SMALL_INT(state.keyDown(key));
}
