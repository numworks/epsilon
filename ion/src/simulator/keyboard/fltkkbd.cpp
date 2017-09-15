#include <assert.h>
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

#define KEY_X   0
#define KEY_Y   1
#define KEY_W   2
#define KEY_H   3

static const int kXYWHForKey[Ion::Keyboard::NumberOfKeys][4] = {
  // Arrow keys: Left, Up, Down, Right
  {10, 48, 26, 22}, {42, 16, 22, 26}, {42, 78, 22, 26}, {70, 48, 26, 22},

  // OK and Back keys
  {224, 44, 30, 30}, {277, 44, 30, 30},

  // Home and Power keys
  {142, 24, 34, 22}, {142, 72, 34, 22},

  // Placeholders for inactive keys (B3, B4, B5, B6)
  {0,  0,  0,  0}, {0,  0,  0,  0}, {0,  0,  0,  0}, {0,  0,  0,  0},

  // 3x6 function keys
  {10, 128, 32, 22}, {63, 128, 32, 22}, {116, 128, 32, 22}, {169, 128, 32, 22}, {222, 128, 32, 22}, {275, 128, 32, 22},
  {10, 171, 32, 22}, {63, 171, 32, 22}, {116, 171, 32, 22}, {169, 171, 32, 22}, {222, 171, 32, 22}, {275, 171, 32, 22},
  {10, 214, 32, 22}, {63, 214, 32, 22}, {116, 214, 32, 22}, {169, 214, 32, 22}, {222, 214, 32, 22}, {275, 214, 32, 22},

  // 4x5 numeric keys (plus iactive keys F6, G6, H6 and I6)
  {14, 260, 34, 22}, {78, 260, 34, 22}, {142, 260, 34, 22}, {206, 260, 34, 22}, {270, 260, 34, 22}, {0, 0,  0,  0},
  {14, 308, 34, 22}, {78, 308, 34, 22}, {142, 308, 34, 22}, {206, 308, 34, 22}, {270, 308, 34, 22}, {0, 0,  0,  0},
  {14, 356, 34, 22}, {78, 356, 34, 22}, {142, 356, 34, 22}, {206, 356, 34, 22}, {270, 356, 34, 22}, {0, 0,  0,  0},
  {14, 404, 34, 22}, {78, 404, 34, 22}, {142, 404, 34, 22}, {206, 404, 34, 22}, {270, 404, 34, 22}, {0, 0,  0,  0}
};

static Fl_Group* Bkg_Image = NULL;

class Fl_Push_Button : public Fl_Button {
public:
    Fl_Push_Button (int X, int Y, int W, int H, const char *l = 0) : Fl_Button(X, Y, W, H, l) {
        this->box(FL_FLAT_BOX);
        this->down_color(FL_BACKGROUND_COLOR);
    }

protected:
    void draw() {
        Fl_Button::draw();
        if (!value()) {
            Bkg_Image->redraw();
        }
    }
};

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
      m_buttons[k] = new Fl_Push_Button(kXYWHForKey[k][KEY_X] + x,
                                        kXYWHForKey[k][KEY_Y] + y,
                                        kXYWHForKey[k][KEY_W],
                                        kXYWHForKey[k][KEY_H],
                                        kCharForKey[k]);
    if (kCharForKey[k][0] == '\0') {
      m_buttons[k]->deactivate();
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
