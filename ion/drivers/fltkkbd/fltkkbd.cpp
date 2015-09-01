#include "fltkkbd.h"

FltkKBD::FltkKBD(int x, int y, int w, int h) :
  Fl_Group(x, y, w, h) {
}

bool FltkKBD::scankey(ion_key_t key) {
  return false;
}
