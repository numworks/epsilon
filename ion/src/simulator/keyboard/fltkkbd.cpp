#include <assert.h>
#include "../init.h"
#include "fltkkbd.h"
#include "kbdimage_icon.h"


static const char* kCharForKey[Ion::Keyboard::NumberOfKeys] = {
  "\u25c1",   "\u25b3",     "\u25bd",   "\u25b7",  "OK",     "\u21ba",
  "home",   "PWR",  "",       "",       "",       "",
  "shift",  "alpha",  "x,n,t",  "var",    "Tbox",  "\u232b",
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
  if (currentEvent == (uint8_t)Ion::Events::None) {
    currentEvent = Ion::Events::Event((Ion::Keyboard::Key)key,
                                      Ion::Events::isShiftActive(),
                                      Ion::Events::isAlphaActive());
    updateModifiersFromEvent(currentEvent);
  }
}

static const int kXYWHForKey[Ion::Keyboard::NumberOfKeys][4] = {
  // Arrow keys: Left, Up, Down, Right
  {5, 44, 35, 30}, {39, 10, 28, 37}, {39, 72, 28, 37}, {66, 44, 35, 30},

  // OK and Back keys
  {218, 38, 44, 44}, {270, 38, 44, 44},

  // Home and Power keys
  {133, 18, 54, 35}, {133, 67, 54, 35},

  // Placeholders for inactive keys (B3, B4, B5, B6)
  {0,  0,  0,  0}, {0,  0,  0,  0}, {0,  0,  0,  0}, {0,  0,  0,  0},
};

static Fl_Group* Bkg_Image = NULL;

class Fl_Push_Button : public Fl_Button {
public:
  Fl_Push_Button(int X, int Y, int W, int H, const char *l = 0) : Fl_Button(X, Y, W, H, l) {
    this->box(FL_FLAT_BOX);
    this->down_color(Fl_Color(0XE2E2E200));
  }

protected:
  void draw() {
    Fl_Button::draw();
    if (!value()) {
      Bkg_Image->redraw();
    }
  }
};

class Fl_Push_Repeat_Button : public Fl_Repeat_Button {
public:
  Fl_Push_Repeat_Button(int X, int Y, int W, int H, const char *l = 0) : Fl_Repeat_Button(X, Y, W, H, l) {
    this->box(FL_FLAT_BOX);
    this->down_color(Fl_Color(0XE2E2E200));
  }

protected:
  void draw() {
    Fl_Repeat_Button::draw();
    if (!value()) {
      Bkg_Image->redraw();
    }
  }
};

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
    int bx, by, bw, bh;
    if (k < 12) {
      bx = kXYWHForKey[k][0] + x;
      by = kXYWHForKey[k][1] + y;
      bw = kXYWHForKey[k][2];
      bh = kXYWHForKey[k][3];
    }
    else if (k < 30) {
      bx = 4 + ((k-12)%6) * 53 + x;
      by = 124 + ((k-12)/6) * 43 + y;
      bw = 44;
      bh = 30;
    }
    else {
      bx = 4 + ((k-30)%6) * 64 + x;
      by = 253 + ((k-30)/6) * 48 + y;
      bw = 53;
      bh = 36;
    }
    if (shouldRepeatKey((Ion::Keyboard::Key)k)) {
      m_buttons[k] = new Fl_Push_Repeat_Button(bx, by, bw, bh, kCharForKey[k]);
    } else {
      m_buttons[k] = new Fl_Push_Button(bx, by, bw, bh, kCharForKey[k]);
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

  // The keyboard image includes the margins so shift the anchor point left & up
  int margin = x;
  Bkg_Image = new UserInterface (x-margin, y-margin, w+2*margin, h+2*margin);

  end();
}

bool FltkKbd::key_down(Ion::Keyboard::Key key) {
  return m_buttons[(int)key]->value();
}
