#ifndef ION_FLTK_KBD
#define ION_FLTK_KBD

#include <FL/Fl_Group.H>
extern "C" {
#include <ion/keyboard.h>
}

class FltkKBD : public Fl_Group {
  public:
    FltkKBD(int x, int y, int w, int h);
    bool scankey(ion_key_t key);
};

#endif
