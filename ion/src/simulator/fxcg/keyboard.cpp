#include <stdlib.h>
#include <unistd.h>

#include <gint/display-cg.h>
#include <gint/display.h>
#include <gint/keycodes.h>
#include <gint/keyboard.h>
#include <gint/gint.h>
#include <gint/clock.h>

#include <ion/keyboard.h>
#include <ion/events.h>
#include <ion/power.h>

#include "keyboard.h"
#include "layout_keyboard.h"
#include "main.h"
#include "menuHandler.h"


using namespace Ion::Keyboard;

class KeyPair {
public:
  constexpr KeyPair(Key key, bool numworksShift, bool numworksAlpha, int gintKey, bool gintShift, bool gintAlpha, bool ignoreShiftAlpha = false) :
    m_key(key),
    m_numworksShift(numworksShift),
    m_numworksAlpha(numworksAlpha),
    m_gintKey(gintKey),
    m_gintShift(gintShift),
    m_gintAlpha(gintAlpha),
    m_ignoreShiftAlpha(ignoreShiftAlpha)
  {}
  Key key() const { return m_key; }
  bool numworksShift() const { return m_numworksShift; }
  bool numworksAlpha() const { return m_numworksAlpha; }
  int gintKey() const { return m_gintKey; }
  bool gintShift() const { return m_gintShift; }
  bool gintAlpha() const { return m_gintAlpha; }
  bool ignoreShiftAlpha() const { return m_ignoreShiftAlpha; }
private:
  Key m_key;
  bool m_numworksShift;
  bool m_numworksAlpha;
  int m_gintKey;
  bool m_gintShift;
  bool m_gintAlpha;
  bool m_ignoreShiftAlpha;
};

constexpr static KeyPair sKeyPairs[] = {
  KeyPair(Key::Down, false, false, KEY_DOWN, false, false, true),
  KeyPair(Key::Left, false, false, KEY_LEFT, false, false, true),
  KeyPair(Key::Right, false, false, KEY_RIGHT, false, false, true),
  KeyPair(Key::Up, false, false, KEY_UP, false, false, true),
  KeyPair(Key::Back, false, false, KEY_EXIT, false, false),
  KeyPair(Key::Home, false, false, KEY_MENU, false, false),
  KeyPair(Key::Shift, false, false, KEY_SHIFT, false, false, true),
  KeyPair(Key::Alpha, false, false, KEY_ALPHA, false, false, true),
  KeyPair(Key::XNT, false, false, KEY_XOT, false, false),
  KeyPair(Key::Var, false, false, KEY_VARS, false, false),
  KeyPair(Key::Toolbox, false, false, KEY_OPTN, false, false),
  KeyPair(Key::Backspace, false, false, KEY_DEL, false, false),
  KeyPair(Key::Exp, false, false, KEY_LN, true, false),
  KeyPair(Key::Ln, false, false, KEY_LN, false, false),
  KeyPair(Key::Log, false, false, KEY_LOG, false, false),
  KeyPair(Key::Imaginary, false, false, KEY_0, true, false),
  KeyPair(Key::Comma, false, false, KEY_COMMA, false, false),
  KeyPair(Key::Power, false, false, KEY_POWER, false, false),
  KeyPair(Key::Sine, false, false, KEY_SIN, false, false),
  KeyPair(Key::Cosine, false, false, KEY_COS, false, false),
  KeyPair(Key::Tangent, false, false, KEY_TAN, false, false),
  KeyPair(Key::Pi, false, false, KEY_EXP, true, false),
  KeyPair(Key::Sqrt, false, false, KEY_SQUARE, true, false),
  KeyPair(Key::Square, false, false, KEY_SQUARE, false, false),
  KeyPair(Key::Seven, false, false, KEY_7, false, false),
  KeyPair(Key::Eight, false, false, KEY_8, false, false),
  KeyPair(Key::Nine, false, false, KEY_9, false, false),
  KeyPair(Key::LeftParenthesis, false, false, KEY_LEFTP, false, false),
  KeyPair(Key::RightParenthesis, false, false, KEY_RIGHTP, false, false),
  KeyPair(Key::Four, false, false, KEY_4, false, false),
  KeyPair(Key::Five, false, false, KEY_5, false, false),
  KeyPair(Key::Six, false, false, KEY_6, false, false),
  KeyPair(Key::Multiplication, false, false, KEY_MUL, false, false),
  KeyPair(Key::Division, false, false, KEY_DIV, false, false),
  KeyPair(Key::Division, false, false, KEY_FRAC, false, false),
  KeyPair(Key::One, false, false, KEY_1, false, false),
  KeyPair(Key::Two, false, false, KEY_2, false, false),
  KeyPair(Key::Three, false, false, KEY_3, false, false),
  KeyPair(Key::Plus, false, false, KEY_ADD, false, false),
  KeyPair(Key::Minus, false, false, KEY_SUB, false, false),
  KeyPair(Key::Zero, false, false, KEY_0, false, false),
  KeyPair(Key::Dot, false, false, KEY_DOT, false, false),
  KeyPair(Key::EE, false, false, KEY_EXP, false, false),
  KeyPair(Key::Ans, false, false, KEY_NEG, true, false),
  KeyPair(Key::EXE, false, false, KEY_EXE, false, false, true),
  KeyPair(Key::OnOff, false, false, KEY_ACON, true, false),

  // Cut
  // Not assigned
  // Copy
  KeyPair(Key::Var, true, false, KEY_8, true, false),
  // Paste
  KeyPair(Key::Toolbox, true, false, KEY_9, true, false),
  // Clear
  KeyPair(Key::Backspace, true, false, KEY_ACON, false, false),
  // [
  KeyPair(Key::Exp, true, false, KEY_ADD, true, false),
  // ]
  KeyPair(Key::Ln, true, false, KEY_SUB, true, false),
  // {
  KeyPair(Key::Log, true, false, KEY_MUL, true, false),
  // }
  KeyPair(Key::Imaginary, true, false, KEY_DIV, true, false),
  // _
  KeyPair(Key::Comma, true, false, KEY_NEG, false, false),
  // ->
  KeyPair(Key::Power, true, false, KEY_STORE, false, false),
  // asin
  KeyPair(Key::Sine, true, false, KEY_SIN, true, false),
  // acos
  KeyPair(Key::Cosine, true, false, KEY_COS, true, false),
  // atan
  KeyPair(Key::Tangent, true, false, KEY_TAN, true, false),
  // =
  KeyPair(Key::Pi, true, false, KEY_DOT, true, false),
  // <
  KeyPair(Key::Sqrt, true, false, KEY_F1, false, false),
  // >
  KeyPair(Key::Square, true, false, KEY_F2, false, false),

  // :
  KeyPair(Key::XNT, false, true, KEY_F3, false, false),
  // ;
  KeyPair(Key::Var, false, true, KEY_F4, false, false),
  // "
  KeyPair(Key::Toolbox, false, true, KEY_EXP, false, true),
  // %
  KeyPair(Key::Backspace, false, true, KEY_F5, false, false),
  // A
  KeyPair(Key::Exp, false, true, KEY_XOT, false, true),
  // B
  KeyPair(Key::Ln, false, true, KEY_LOG, false, true),
  // C
  KeyPair(Key::Log, false, true, KEY_LN, false, true),
  // D
  KeyPair(Key::Imaginary, false, true, KEY_SIN, false, true),
  // E
  KeyPair(Key::Comma, false, true, KEY_COS, false, true),
  // F
  KeyPair(Key::Power, false, true, KEY_TAN, false, true),
  // G
  KeyPair(Key::Sine, false, true, KEY_FRAC, false, true),
  // H
  KeyPair(Key::Cosine, false, true, KEY_FD, false, true),
  // I
  KeyPair(Key::Tangent, false, true, KEY_LEFTP, false, true),
  // J
  KeyPair(Key::Pi, false, true, KEY_RIGHTP, false, true),
  // K
  KeyPair(Key::Sqrt, false, true, KEY_COMMA, false, true),
  // L
  KeyPair(Key::Square, false, true, KEY_ARROW, false, true),
  // M
  KeyPair(Key::Seven, false, true, KEY_7, false, true),
  // N
  KeyPair(Key::Eight, false, true, KEY_8, false, true),
  // O
  KeyPair(Key::Nine, false, true, KEY_9, false, true),
  // P
  KeyPair(Key::LeftParenthesis, false, true, KEY_4, false, true),
  // Q
  KeyPair(Key::RightParenthesis, false, true, KEY_5, false, true),
  // R
  KeyPair(Key::Four, false, true, KEY_6, false, true),
  // S
  KeyPair(Key::Five, false, true, KEY_TIMES, false, true),
  // T
  KeyPair(Key::Six, false, true, KEY_DIV, false, true),
  // U
  KeyPair(Key::Multiplication, false, true, KEY_1, false, true),
  // V
  KeyPair(Key::Division, false, true, KEY_2, false, true),
  // W
  KeyPair(Key::One, false, true, KEY_3, false, true),
  // X
  KeyPair(Key::Two, false, true, KEY_PLUS, false, true),
  // Y
  KeyPair(Key::Three, false, true, KEY_MINUS, false, true),
  // Z
  KeyPair(Key::Plus, false, true, KEY_0, false, true),
  // Space
  KeyPair(Key::Minus, false, true, KEY_DOT, false, true),
  // ?
  KeyPair(Key::Zero, false, true, KEY_F6, true, false),
  // !
  KeyPair(Key::Dot, false, true, KEY_F6, false, false),

  // Brightness control shortcut in Upsilon
  KeyPair(Key::Plus, true, false, KEY_UP, false, true),
  KeyPair(Key::Minus, true, false, KEY_DOWN, false, true),
};

constexpr int sNumberOfKeyPairs = sizeof(sKeyPairs)/sizeof(KeyPair);

namespace Ion {
namespace Keyboard {

int menuHeldFor = 0;

State scan() {
  State state = 0;

  // Grab this opportunity to refresh the display if needed
  Simulator::Main::refresh();

  clearevents();
  if (keydown(KEY_MENU)) {
    state.setKey(Key::Home);
    menuHeldFor++;
    if (menuHeldFor > 30) {
      Simulator::FXCGMenuHandler::openMenu();
      dupdate();
      // Wait until EXE is released
      do {
        sleep_ms(10);
        clearevents();
      } while (keydown(KEY_EXE));
    }
  } else {
    menuHeldFor = 0;
  }

  for (int i = 0; i < sNumberOfKeyPairs; i++) {
    const KeyPair & keyPair = sKeyPairs[i];
    if (!keyPair.ignoreShiftAlpha() &&
       (keyPair.gintShift() != Events::isShiftActive() ||
        keyPair.gintAlpha() != Events::isAlphaActive())) {
      continue;
    }
    if (keydown(keyPair.gintKey())) {
      if (!keyPair.ignoreShiftAlpha()) {
        state.setSimulatedShift(keyPair.numworksShift() ? ModSimState::ForceOn : ModSimState::ForceOff);
        state.setSimulatedAlpha(keyPair.numworksAlpha() ? ModSimState::ForceOn : ModSimState::ForceOff);
      }
      state.setKey(keyPair.key());
    }
  }

  return state;
}

}
}

namespace Ion {
namespace Simulator {
namespace Keyboard {

}
}
}