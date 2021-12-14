#include "py/mpstate.h"
#include "py/mphal.h"

#if MICROPY_KBD_EXCEPTION

int mp_interrupt_char;

void mp_hal_set_interrupt_char(int c) {
  if (c != -1) {
    mp_obj_exception_clear_traceback(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)));
  }
  mp_interrupt_char = c;
}

void mp_keyboard_interrupt(void) {
  MP_STATE_MAIN_THREAD(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception));
}

#endif
