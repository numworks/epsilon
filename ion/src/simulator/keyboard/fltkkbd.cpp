#include "fltkkbd.h"

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  for (int k=0; k<ION_NUMBER_OF_KEYS; k++) {
    m_buttons[k] = new Fl_Button(x+(k*w)/ION_NUMBER_OF_KEYS, y, w/ION_NUMBER_OF_KEYS, h);
  }
  end();
}

bool FltkKbd::key_down(ion_key_t key) {
  return m_buttons[key]->value();
}
