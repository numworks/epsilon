#include "fltkkbd.h"
#include "fltkkbdbutton.h"

void FltkKbdButtonCallback(Fl_Widget * w, void * context) {
  FltkKbd * kbd = (FltkKbd *)context;
  FltkKbdButton * button = (FltkKbdButton *)w;
  kbd->m_keyStates[button->m_key] = button->value();
}

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  for (int i=0; i<ION_NUMBER_OF_KEYS; i++) {
    m_keyStates[i] = false;
  }
  for (int k=0; k<ION_NUMBER_OF_KEYS; k++) {
    FltkKbdButton * b = new FltkKbdButton(x+(k*w)/ION_NUMBER_OF_KEYS, y, w/ION_NUMBER_OF_KEYS, h);
    b->m_key = (ion_key_t)k;
    b->when(FL_WHEN_CHANGED);
    b->callback(FltkKbdButtonCallback, (void *)this);
  }
  end();
}
