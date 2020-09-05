#include "keyboard.h"
#include "layout.h"
#include "window.h"

#include <ion/keyboard.h>
#include <SDL.h>

static Ion::Keyboard::State sKeyboardState;

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

namespace Ion {
namespace Keyboard {

State scan() {
  State state = sKeyboardState;

  if (Simulator::Window::isHeadless()) {
    return state;
  }
  // We need to tell SDL to get new state from the host OS
  SDL_PumpEvents();

  // Notify callbacks in case we need to do something
  Simulator::Keyboard::didScan();

  // Grab this opportunity to refresh the display if needed
  Simulator::Window::refresh();

#if !EPSILON_SDL_SCREEN_ONLY
  // Register a key for the mouse, if any
  Key k = Simulator::Layout::getHighlightedKey();
  if (SDL_GetMouseState(nullptr, nullptr) && SDL_BUTTON(SDL_BUTTON_LEFT)) {
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

namespace Ion {
namespace Simulator {
namespace Keyboard {

void keyDown(Ion::Keyboard::Key k) {
  sKeyboardState.setKey(k);
}

void keyUp(Ion::Keyboard::Key k) {
  sKeyboardState.clearKey(k);
}

bool scanHandlesSDLKey(SDL_Scancode key) {
  for (int i = 0; i < sNumberOfKeyPairs; i++) {
    if (key == sKeyPairs[i].SDLKey()) {
      return true;
    }
  }
  return false;
}

}
}
}
