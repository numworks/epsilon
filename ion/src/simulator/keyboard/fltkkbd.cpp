#include <assert.h>
#include "fltkkbd.h"

#define KEYBOARD_ROWS 7
#define KEYBOARD_COLUMNS 5

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  assert(KEYBOARD_ROWS*KEYBOARD_COLUMNS == ION_NUMBER_OF_KEYS);
  int key_width = w/KEYBOARD_COLUMNS;
  int key_height = h/KEYBOARD_ROWS;
  for (int k=0; k<ION_NUMBER_OF_KEYS; k++) {
    m_buttons[k] = new Fl_Button(x + (k%KEYBOARD_COLUMNS)*key_width, y + (k/KEYBOARD_COLUMNS)*key_height, key_width, key_height);
  }
  end();
}

bool FltkKbd::key_down(ion_key_t key) {
  return m_buttons[key]->value();
}
