#ifndef ION_FLTK_KBD
#define ION_FLTK_KBD

#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <ion/keyboard.h>

class FltkKbd : public Fl_Group {
public:
  constexpr static int sNumberOfLettersInAlphabet = 26;
  FltkKbd(int x, int y, int w, int h);
  bool key_down(Ion::Keyboard::Key key);
private:
  Fl_Button * m_buttons[Ion::Keyboard::NumberOfKeys];
  Fl_Button * m_alphabetShortcuts[2*sNumberOfLettersInAlphabet];
};

#endif
