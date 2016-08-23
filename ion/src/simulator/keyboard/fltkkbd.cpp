#include <assert.h>
#include "fltkkbd.h"

constexpr int KeyboardRows = 10;
constexpr int KeyboardColumns = 5;

static const char* kCharForKey[Ion::Keyboard::NumberOfKeys] = {
  "F1",      "F2",    "F3",      "F4",     "F5",
  "2nd",     "Shift", "ESC",     "LEFT",   "UP",
  "Diamond", "alpha", "APPS",    "DOWN",   "RIGHT",
  "HOME",    "MODE",  "CTLG", "Delete", "CLEAR",
  "X",       "Y",     "Z",       "T",      "^",
  "=",       "(",     ")",       ",",      "%",
  "|",       "7",     "8",       "9",      "x",
  "EE",      "4",     "5",       "6",      "-",
  "STO",     "1",     "2",       "3",      "+",
  "ON",      "0",     ".",       "(-)",    "ENTER"
};

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  assert(KeyboardRows*KeyboardColumns == Ion::Keyboard::NumberOfKeys);
  int key_width = w/KeyboardColumns;
  int key_height = h/KeyboardRows;
  for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
    m_buttons[k] = new Fl_Button(x + (k%KeyboardColumns)*key_width,
                                 y + (k/KeyboardColumns)*key_height,
                                 key_width,
                                 key_height,
                                 kCharForKey[k]);
  }
  end();
}

bool FltkKbd::key_down(Ion::Keyboard::Key key) {
  return m_buttons[(int)key]->value();
}
