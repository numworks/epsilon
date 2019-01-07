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
  0, 0, 0, 0, 0, FL_BackSpace,
  0, 0, 0, 0, ',', 0,
  0, 0, 0, 0, 0, 0,
  FL_SHIFT | '7', FL_SHIFT | '8', FL_SHIFT | '9', '(',')', 0,
  FL_SHIFT | '4', FL_SHIFT | '5', FL_SHIFT | '6', '*', '/', 0,
  FL_SHIFT | '1', FL_SHIFT | '2', FL_SHIFT | '3', '+', '-', 0,
  FL_SHIFT | '0', '.', 0, 0, FL_KP_Enter, 0
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

static void alphabetEventHandler(Fl_Widget *, long c) {
  static Ion::Events::Event lowerEvents[FltkKbd::sNumberOfLettersInAlphabet] = {
    Ion::Events::LowerA, Ion::Events::LowerB, Ion::Events::LowerC, Ion::Events::LowerD, Ion::Events::LowerE,
    Ion::Events::LowerF, Ion::Events::LowerG, Ion::Events::LowerH, Ion::Events::LowerI, Ion::Events::LowerJ,
    Ion::Events::LowerK, Ion::Events::LowerL, Ion::Events::LowerM, Ion::Events::LowerN, Ion::Events::LowerO,
    Ion::Events::LowerP, Ion::Events::LowerQ, Ion::Events::LowerR, Ion::Events::LowerS, Ion::Events::LowerT,
    Ion::Events::LowerU, Ion::Events::LowerV, Ion::Events::LowerW, Ion::Events::LowerX, Ion::Events::LowerY,
    Ion::Events::LowerZ };
  static Ion::Events::Event upperEvents[FltkKbd::sNumberOfLettersInAlphabet] = {
    Ion::Events::UpperA, Ion::Events::UpperB, Ion::Events::UpperC, Ion::Events::UpperD, Ion::Events::UpperE,
    Ion::Events::UpperF, Ion::Events::UpperG, Ion::Events::UpperH, Ion::Events::UpperI, Ion::Events::UpperJ,
    Ion::Events::UpperK, Ion::Events::UpperL, Ion::Events::UpperM, Ion::Events::UpperN, Ion::Events::UpperO,
    Ion::Events::UpperP, Ion::Events::UpperQ, Ion::Events::UpperR, Ion::Events::UpperS, Ion::Events::UpperT,
    Ion::Events::UpperU, Ion::Events::UpperV, Ion::Events::UpperW, Ion::Events::UpperX, Ion::Events::UpperY,
    Ion::Events::UpperZ
  };
  if (currentEvent == Ion::Events::None) {
    if (c >= 'a' && c <= 'z') {
      currentEvent = lowerEvents[c - 'a'];
    } else if (c >= 'A' && c <= 'Z') {
      currentEvent = upperEvents[c - 'A'];
    }
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

  // Get keyboard letters event
  for (int k = 0; k < sNumberOfLettersInAlphabet; k++) {
    char l = 'a' + k;
    m_alphabetShortcuts[k] = new Fl_Button(0, 0, 0, 0, "");
    m_alphabetShortcuts[k]->callback(alphabetEventHandler,l);
    m_alphabetShortcuts[k]->shortcut(l);
    m_alphabetShortcuts[k]->clear_visible_focus();
  }
  for (int k = 0; k < sNumberOfLettersInAlphabet; k++) {
    char l = 'A' + k;
    int index = sNumberOfLettersInAlphabet + k;
    m_alphabetShortcuts[index] = new Fl_Button(0, 0, 0, 0, "");
    m_alphabetShortcuts[index]->callback(alphabetEventHandler, l);
    m_alphabetShortcuts[index]->shortcut(FL_SHIFT | ('a' + k));
    m_alphabetShortcuts[index]->clear_visible_focus();
  }

  end();
}

bool FltkKbd::key_down(Ion::Keyboard::Key key) {
  return m_buttons[(int)key]->value();
}
