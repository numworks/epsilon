#ifndef ION_FLTK_KBD
#define ION_FLTK_KBD

#include <FL/Fl_Group.H>
extern "C" {
#include <ion/keyboard.h>
}

class FltkKbd : public Fl_Group {
  friend void FltkKbdButtonCallback(Fl_Widget * w, void * context);
public:
  FltkKbd(int x, int y, int w, int h);
  bool scankey(ion_key_t key);
private:
  bool * m_keyState;
  void register_key_state(ion_key_t key, bool active);
};

#endif
