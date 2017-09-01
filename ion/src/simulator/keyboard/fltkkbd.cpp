#include <assert.h>
#include "../init.h"
#include "fltkkbd.h"
#include <FL/Fl_Repeat_Button.H>
#undef None // TODO: Remove me

constexpr int KeyboardRows = 9;
constexpr int KeyboardColumns = 6;

static const char* kCharForKey[Ion::Keyboard::NumberOfKeys] = {
  "\u25c1",   "\u25b3",     "\u25bd",   "\u25b7",  "OK",     "\u21ba",
  "Home",   "Power",  "",       "",       "",       "",
  "shift",  "alpha",  "x,n,t",  "var",    "Toolbox",  "\u232b",
  "e\u02e3",    "ln",     "log",    "i",      ",",      "x\u02b8",
  "sin",    "cos",    "tan",    "\u03c0",     "\u221a\u203e\u203e",   "x\u00b2",
  "7",      "8",      "9",      "(",      ")",      "",
  "4",      "5",      "6",      "\u00d7",      "\u00f7",      "",
  "1",      "2",      "3",      "+",      "\u2212",      "",
  "0",      ".",      "\u00d710\u02e3",  "Ans",    "EXE",    ""
};

static const int kShortcutForKey[Ion::Keyboard::NumberOfKeys] = {
  FL_Left, FL_Up, FL_Down, FL_Right, FL_Enter, FL_Escape,
  FL_Home, 0, 0, 0, 0, 0,
  0, 0, 'x', 0, 0, FL_BackSpace,
  0, 0, 0, 'i', ',', 0,
  0, 0, 0, 0, 0, 0,
  '7', '8', '9', '(',')', 0,
  '4', '5', '6', '*', '/', 0,
  '1', '2', '3', '+', '-', 0,
  '0', '.', 0, 0, FL_KP_Enter, 0
};

static bool shouldRepeatKey(Ion::Keyboard::Key k) {
  return k <= Ion::Keyboard::Key::A4 || k == Ion::Keyboard::Key::A6;
}

static void keyHandler(Fl_Widget *, long key) {
  if (currentEvent == Ion::Events::None) {
    currentEvent = Ion::Events::Event((Ion::Keyboard::Key)key,
                                      Ion::Events::isShiftActive(),
                                      Ion::Events::isAlphaActive());
    updateModifiersFromEvent(currentEvent);
  }
}

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  assert(KeyboardRows*KeyboardColumns == Ion::Keyboard::NumberOfKeys);
  int key_width = w/KeyboardColumns;
  int key_height = h/KeyboardRows;
  for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
    int key_x = x + (k%KeyboardColumns)*key_width;
    int key_y = y + (k/KeyboardColumns)*key_height;
    if (shouldRepeatKey((Ion::Keyboard::Key)k)) {
      m_buttons[k] = new Fl_Repeat_Button(key_x, key_y, key_width, key_height, kCharForKey[k]);
    } else {
      m_buttons[k] = new Fl_Button(key_x, key_y, key_width, key_height, kCharForKey[k]);
    }
#if defined(_WIN32) || defined(_WIN64)
    m_buttons[k]->labelfont(FL_SYMBOL);
#endif
    if (kCharForKey[k][0] == '\0') {
      m_buttons[k]->deactivate();
    } else {
      m_buttons[k]->callback(keyHandler, k);
    }
    if (kShortcutForKey[k]) {
      m_buttons[k]->shortcut(kShortcutForKey[k]);
    }
    m_buttons[k]->clear_visible_focus();
  }
  end();
}

bool FltkKbd::key_down(Ion::Keyboard::Key key) {
  return m_buttons[(int)key]->value();
}
