#include <stdlib.h>
#include <unistd.h>


#include <ion/keyboard.h>
#include <ion/events.h>
#include <ion/power.h>

#include "layout_keyboard.h"
#include "main.h"
#include "k_csdk.h"


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
  KeyPair(Key::Toolbox, false, false, KEY_CTRL_CATALOG, false, false),
  KeyPair(Key::Backspace, false, false, KEY_DEL, false, false),
  KeyPair(Key::Exp, false, false, KEY_CHAR_EXPN, false, false),
  KeyPair(Key::Ln, false, false, KEY_LN, false, false),
  KeyPair(Key::Log, false, false, KEY_LOG, false, false),
  KeyPair(Key::Imaginary, false, false, KEY_CHAR_IMGNRY, false, false),
  KeyPair(Key::Comma, false, false, KEY_COMMA, false, false),
  KeyPair(Key::Power, false, false, '^', false, false),
  KeyPair(Key::Sine, false, false, KEY_SIN, false, false),
  KeyPair(Key::Cosine, false, false, KEY_COS, false, false),
  KeyPair(Key::Tangent, false, false, KEY_TAN, false, false),
  KeyPair(Key::Pi, false, false, KEY_CHAR_PI, false, false),
  KeyPair(Key::Sqrt, false, false, KEY_CHAR_ROOT, false, false),
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
  KeyPair(Key::EE, false, false, KEY_CHAR_EXP, false, false),
  KeyPair(Key::Ans, false, false, KEY_CHAR_ANS, true, false),
  KeyPair(Key::EXE, false, false, KEY_EXE, false, false, true),
  KeyPair(Key::OnOff, false, false, KEY_ACON, false, false),

  // Cut
  KeyPair(Key::XNT, true, false, KEY_CTRL_CUT, false, false),
  // Copy
  KeyPair(Key::Var, true, false, KEY_CTRL_CLIP, false, false),
  // Paste
  KeyPair(Key::Toolbox, true, false, KEY_CTRL_PASTE, false, false),
  // Clear
  KeyPair(Key::Backspace, true, false, KEY_ACON, false, false),
  // [
  KeyPair(Key::Exp, true, false, KEY_CHAR_CROCHETS, false, false),
  // ]
  KeyPair(Key::Ln, true, false, KEY_CTRL_F13, false, false),
  // {
  KeyPair(Key::Log, true, false, KEY_CHAR_ACCOLADES, false, false),
  // }
  KeyPair(Key::Imaginary, true, false, KEY_CTRL_F14, false, false),
  // _
  KeyPair(Key::Comma, true, false, KEY_NEG, false, false),
  // ->
  KeyPair(Key::Power, true, false, KEY_STORE, false, true),
  // asin
  KeyPair(Key::Sine, true, false, KEY_CHAR_ASIN, true, false),
  // acos
  KeyPair(Key::Cosine, true, false, KEY_CHAR_ACOS, true, false),
  // atan
  KeyPair(Key::Tangent, true, false, KEY_CHAR_ATAN, true, false),
  // =
  KeyPair(Key::Pi, true, false, KEY_CHAR_EQUAL, false, false),
  // <
  KeyPair(Key::Sqrt, true, false, '<', false, false),
  // >
  KeyPair(Key::Square, true, false, '>', false, false),

  // :
  KeyPair(Key::XNT, false, true, ':', false, false),
  // ;
  KeyPair(Key::Var, false, true, ';', false, false),
  // "
  KeyPair(Key::Toolbox, false, true, KEY_CHAR_DQUATE, false, true),
  // %
  KeyPair(Key::Backspace, false, true, '%', false, false),
  // A
  KeyPair(Key::Exp, false, true, 'a', false, false),
  // B
  KeyPair(Key::Ln, false, true, 'b', false, false),
  // C
  KeyPair(Key::Log, false, true, 'c', false, false),
  // D
  KeyPair(Key::Imaginary, false, true, 'd', false, false),
  // E
  KeyPair(Key::Comma, false, true, 'e', false, false),
  // F
  KeyPair(Key::Power, false, true, 'f', false, false),
  // G
  KeyPair(Key::Sine, false, true, 'g', false, false),
  // H
  KeyPair(Key::Cosine, false, true, 'h', false, false),
  // I
  KeyPair(Key::Tangent, false, true, 'i', false, false),
  // J
  KeyPair(Key::Pi, false, true, 'j', false, false),
  // K
  KeyPair(Key::Sqrt, false, true,'k', false, false),
  // L
  KeyPair(Key::Square, false, true, 'l', false, false),
  // M
  KeyPair(Key::Seven, false, true, 'm', false, false),
  // N
  KeyPair(Key::Eight, false, true, 'n', false, false),
  // O
  KeyPair(Key::Nine, false, true, 'o', false, false),
  // P
  KeyPair(Key::LeftParenthesis, false, true, 'p', false, false),
  // Q
  KeyPair(Key::RightParenthesis, false, true, 'q', false, false),
  // R
  KeyPair(Key::Four, false, true, 'r', false, false),
  // S
  KeyPair(Key::Five, false, true, 's', false, false),
  // T
  KeyPair(Key::Six, false, true, 't', false, false),
  // U
  KeyPair(Key::Multiplication, false, true, 'u', false, false),
  // V
  KeyPair(Key::Division, false, true, 'v', false, false),
  // W
  KeyPair(Key::One, false, true, 'w', false, false),
  // X
  KeyPair(Key::Two, false, true, 'x', false, false),
  // Y
  KeyPair(Key::Three, false, true, 'y', false, false),
  // Z
  KeyPair(Key::Plus, false, true, 'z', false, false),
  // Space
  KeyPair(Key::Minus, false, true, KEY_CHAR_SPACE, false, false),
  // ?
  KeyPair(Key::Zero, false, true, KEY_F6, true, false),
  // !
  KeyPair(Key::Dot, false, true, KEY_F6, false, false),

  // Brightness control shortcut in Upsilon
  KeyPair(Key::Plus, true, false, KEY_ADD, false, true),
  KeyPair(Key::Minus, true, false, KEY_SUB, false, true),

  
};

constexpr int sNumberOfKeyPairs = sizeof(sKeyPairs)/sizeof(KeyPair);

namespace Ion {
namespace Keyboard {

int menuHeldFor = 0;

State scan() {
  State state = 0;

  // Grab this opportunity to refresh the display if needed
  Simulator::Main::refresh();
  if (!any_key_pressed())
    return state;
  if (isKeyPressed(KEY_NSPIRE_HOME)){
    state.setKey(Key::Home); return state;
  }
  if (isKeyPressed(KEY_NSPIRE_ENTER)){
    state.setKey(Key::EXE);
    return state;
  }
  if (isKeyPressed(KEY_NSPIRE_LEFT)){
    state.setKey(Key::Left);
    return state;
  }
  if (isKeyPressed(KEY_NSPIRE_RIGHT)){
    state.setKey(Key::Right);
    return state;
  }
  if (isKeyPressed(KEY_NSPIRE_UP)){
    state.setKey(Key::Up);
    return state;
  }
  if (isKeyPressed(KEY_NSPIRE_DOWN)){
    state.setKey(Key::Down);
    return state;
  }
  int shiftstate;
  int scancode=nspire_scan(&shiftstate);
  if (scancode==0) {
    return state;
  }
  switch (scancode){
  case -2: case -1:
    return state;
  case KEY_CTRL_EXIT:
    state.setKey(Key::Back);
    return state;
  case KEY_CTRL_EXE:
    state.setKey(Key::EXE);
    return state;
  case KEY_CTRL_MENU:
    state.setKey(Key::Home);
    return state;
  }
  nspire_ctrl=nspire_shift=0;
  for (int i = 0; i < sNumberOfKeyPairs; i++) {
    const KeyPair & keyPair = sKeyPairs[i];
    if (scancode==keyPair.gintKey()) {
      state.setSimulatedShift(keyPair.numworksShift() ? ModSimState::ForceOn : ModSimState::ForceOff);
      state.setSimulatedAlpha(keyPair.numworksAlpha() ? ModSimState::ForceOn : ModSimState::ForceOff);
      state.setKey(keyPair.key());
      return state;
    }
  }
  if (isKeyPressed(KEY_NSPIRE_TENX)){
      exit(0);
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
