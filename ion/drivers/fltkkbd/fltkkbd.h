#ifndef ION_FLTK_KBD
#define ION_FLTK_KBD

#include <FL/Fl_Group.H>
extern "C" {
#include <ion/keyboard.h>
}

class FltkKbd : public Fl_Group {
public:
  FltkKbd(int x, int y, int w, int h);
  bool m_keyStates[ION_NUMBER_OF_KEYS];
};

#endif
