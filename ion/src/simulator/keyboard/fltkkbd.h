#ifndef ION_FLTK_KBD
#define ION_FLTK_KBD

#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
extern "C" {
#include <ion/keyboard.h>
}

class FltkKbd : public Fl_Group {
public:
  FltkKbd(int x, int y, int w, int h);
  bool key_down(ion_key_t key);
private:
  Fl_Button * m_buttons[ION_NUMBER_OF_KEYS];
};

#endif
