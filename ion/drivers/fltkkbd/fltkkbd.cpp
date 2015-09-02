#include "fltkkbd.h"
#include "fltkkbdbutton.h"
#include <stdio.h>

void FltkKbdButtonCallback(Fl_Widget * w, void * context) {
  FltkKbd * kbd = (FltkKbd *)context;
  FltkKbdButton * button = (FltkKbdButton *)w;
  kbd->register_key_state(button->m_key, button->value());
  printf("Register state %d at %d\n", button->m_key, button->value());
}

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  m_keyState = new bool[ION_NUMBER_OF_KEYS];
  for (int i=0; i<ION_NUMBER_OF_KEYS; i++) {
    m_keyState[i] = false;
  }
  for (int k=0; k<ION_NUMBER_OF_KEYS; k++) {
    FltkKbdButton * b = new FltkKbdButton(x+(k*w)/ION_NUMBER_OF_KEYS, y, w/ION_NUMBER_OF_KEYS, h);
    b->m_key = (ion_key_t)k;
    b->when(FL_WHEN_CHANGED);
    b->callback(FltkKbdButtonCallback, (void *)this);
  }
  end();
}

// FIXME: Destructor: delete[] m_keyState

bool FltkKbd::scankey(ion_key_t key) {
  return m_keyState[key];
}

void FltkKbd::register_key_state(ion_key_t key, bool active) {
  m_keyState[key] = active;
}
