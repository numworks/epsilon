#ifndef ION_FLTK_KBD_BUTTON
#define ION_FLTK_KBD_BUTTON

#include <FL/Fl_Button.H>
extern "C" {
#include <ion/keyboard.h>
}

class FltkKbdButton : public Fl_Button {
  public:
    FltkKbdButton(int x, int y, int w, int h, const char * label = 0);
    ion_key_t m_key;
};

#endif
