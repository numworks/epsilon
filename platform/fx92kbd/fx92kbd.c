#include "fx92kbd.h"

#define FX92_KBD_NUM_ROWS 8
#define FX92_KBD_NUM_COLUMNS 8

void fx92kbd_initialize(fx92kbd_t * controller) {
  controller->row_write(0);
  //TODO: assert uint8_t can hold NUM_ROWS and NUM_COLUMNS
}

//FIXME!
static int delay(int usec) {
  for (int i=0; i<usec; i++) {
  }
}

fx92kbd_key_t fx92kbd_get_current_key(fx92kbd_t * controller) {
  /* There is concept of a current key since multiple keys can be pressed at the
   * same time. However, we can scan keys and return the first pressed one that
   * we find.
   * We're scanning rows, and break as soon as we find a non-zero column. Then,
   * we scan the column and return the first pressed key. */
  for (int i=0; i<FX92_KBD_NUM_ROWS; i++) {
    controller->row_write(1 << i);
    delay(10000);
    uint8_t column = controller->column_read();
    if (column != 0) {
      // At least one key is pressed in row 'i'. Let's figure out which.
      for (int j=0; j<FX92_KBD_NUM_COLUMNS; j++) {
        if (column & (1<<j)) {
          // At this point, we know that the key at row i, column j is pressed.
          return (i*FX92_KBD_NUM_COLUMNS  + j);
        }
      }
      // TODO: Assert that we never reach here
    }
  }
  return FX92_KEY_NONE;
}

fx92kbd_key_t fx92kbd_getkey(fx92kbd_t * controller) {
  fx92kbd_key_t key = FX92_KEY_NONE;
  while (key == FX92_KEY_NONE) {
    key = fx92kbd_get_current_key(controller);
  }
  return key;
}
