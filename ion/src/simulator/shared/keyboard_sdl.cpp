#include "main.h"
#include "platform.h"
#include "layout.h"

#include <ion/keyboard.h>
#include <SDL.h>

#if EPSILON_SDL_SCREEN_ONLY
static Ion::Keyboard::State sKeyboardState;

void IonSimulatorKeyboardKeyDown(int keyNumber) {
  Ion::Keyboard::Key key = static_cast<Ion::Keyboard::Key>(keyNumber);
  sKeyboardState.setKey(key);
}

void IonSimulatorKeyboardKeyUp(int keyNumber) {
  Ion::Keyboard::Key key = static_cast<Ion::Keyboard::Key>(keyNumber);
  sKeyboardState.clearKey(key);
}
#endif

namespace Ion {
namespace Keyboard {

class KeySDLKeyPair {
public:
  constexpr KeySDLKeyPair(Ion::Keyboard::Key key, SDL_Scancode SDLKey) :
    m_key(key),
    m_SDLKey(SDLKey)
  {}
  Ion::Keyboard::Key key() const { return m_key; }
  SDL_Scancode SDLKey() const { return m_SDLKey; }
private:
  Ion::Keyboard::Key m_key;
  SDL_Scancode m_SDLKey;
};

constexpr static KeySDLKeyPair sKeyPairs[] = {
  KeySDLKeyPair(Ion::Keyboard::Key::Down,      SDL_SCANCODE_DOWN),
  KeySDLKeyPair(Ion::Keyboard::Key::Up,        SDL_SCANCODE_UP),
  KeySDLKeyPair(Ion::Keyboard::Key::Left,      SDL_SCANCODE_LEFT),
  KeySDLKeyPair(Ion::Keyboard::Key::Right,     SDL_SCANCODE_RIGHT),
  KeySDLKeyPair(Ion::Keyboard::Key::Shift,     SDL_SCANCODE_LSHIFT),
  KeySDLKeyPair(Ion::Keyboard::Key::Shift,     SDL_SCANCODE_RSHIFT),
  KeySDLKeyPair(Ion::Keyboard::Key::EXE,       SDL_SCANCODE_RETURN),
  KeySDLKeyPair(Ion::Keyboard::Key::Back,      SDL_SCANCODE_ESCAPE),
  KeySDLKeyPair(Ion::Keyboard::Key::Toolbox,   SDL_SCANCODE_TAB),
  KeySDLKeyPair(Ion::Keyboard::Key::Backspace, SDL_SCANCODE_BACKSPACE)
};

constexpr int sNumberOfKeyPairs = sizeof(sKeyPairs)/sizeof(KeySDLKeyPair);

State scan() {
  // We need to tell SDL to get new state from the host OS
  SDL_PumpEvents();

  // Notify callbacks in case we need to do something
  IonSimulatorCallbackDidScanKeyboard();

  // Grab this opportunity to refresh the display if needed
  Simulator::Main::refresh();

  // Start with a "clean" state
  State state;
#if EPSILON_SDL_SCREEN_ONLY
  // In this case, keyboard states are sent over another channel.
  state = sKeyboardState;
#else
  // Register a key for the mouse, if any
  SDL_Point p;
  Uint32 mouseState = SDL_GetMouseState(&p.x, &p.y);
  if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    Key k = Simulator::Layout::keyAt(&p);
    state.setKey(k);
  }
#endif

  // Catch the physical keyboard events
  const uint8_t * SDLstate = SDL_GetKeyboardState(NULL);
  for (int i = 0; i < sNumberOfKeyPairs; i++) {
    KeySDLKeyPair pair = sKeyPairs[i];
    if (SDLstate[pair.SDLKey()]) {
      state.setKey(pair.key());
    }
  }
  return state;
}

}
}

bool IonSimulatorSDLKeyDetectedByScan(SDL_Scancode key) {
  for (int i = 0; i < Ion::Keyboard::sNumberOfKeyPairs; i++) {
    if (key == Ion::Keyboard::sKeyPairs[i].SDLKey()) {
      return true;
    }
  }
  return false;
}
