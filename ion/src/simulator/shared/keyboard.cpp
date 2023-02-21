#include "keyboard.h"

#include <SDL.h>
#include <ion/keyboard.h>
#include <ion/src/shared/keyboard.h>
#include <ion/src/shared/keyboard_queue.h>

#include "layout.h"
#include "window.h"

using namespace Ion::Keyboard;

class KeySDLKeyPair {
 public:
  constexpr KeySDLKeyPair(Key key, SDL_Scancode SDLKey)
      : m_key(key), m_SDLKey(SDLKey) {}
  Key key() const { return m_key; }
  SDL_Scancode SDLKey() const { return m_SDLKey; }

 private:
  Key m_key;
  SDL_Scancode m_SDLKey;
};

/* The sKeyPairs list indicates which keys on the host are bound to the
 * simulated calculator's own keyboard.
 * The state of those keys will be visible in Ion::Keyboard::state(), and as a
 * result will emit events from Ion::Events::sharedGetEvent.
 * It looks like a great idea, but in many scenarios that 1:1 mapping is not the
 * best solution. For example, you may not want to emit a Clear event following
 * a Shift-Backspace on the host. */

constexpr static KeySDLKeyPair sKeyPairs[] = {
    KeySDLKeyPair(Key::Down, SDL_SCANCODE_DOWN),
    KeySDLKeyPair(Key::Up, SDL_SCANCODE_UP),
    KeySDLKeyPair(Key::Left, SDL_SCANCODE_LEFT),
    KeySDLKeyPair(Key::Right, SDL_SCANCODE_RIGHT),
    KeySDLKeyPair(Key::Shift, SDL_SCANCODE_LSHIFT),
    KeySDLKeyPair(Key::Shift, SDL_SCANCODE_RSHIFT),
    KeySDLKeyPair(Key::EXE, SDL_SCANCODE_RETURN),
    KeySDLKeyPair(Key::Back, SDL_SCANCODE_ESCAPE),
    KeySDLKeyPair(Key::Toolbox, SDL_SCANCODE_TAB),
};

constexpr int sNumberOfKeyPairs = sizeof(sKeyPairs) / sizeof(KeySDLKeyPair);

namespace Ion {
namespace Keyboard {

State scanForInterruptionAndPopState() {
  // We get eventual user interruptions via the simulator keyboard
  scan();
  return popState();
}

State scan() {
  State state(0);

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
  if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    state.setKey(k);
  }
#endif

  // Catch the physical keyboard events
  const uint8_t* SDLstate = SDL_GetKeyboardState(NULL);
  for (int i = 0; i < sNumberOfKeyPairs; i++) {
    KeySDLKeyPair pair = sKeyPairs[i];
    if (SDLstate[pair.SDLKey()]) {
      state.setKey(pair.key());
    }
  }

  keyboardWasScanned(state);
  return state;
}

}  // namespace Keyboard
}  // namespace Ion

namespace Ion {
namespace Simulator {
namespace Keyboard {

void keyDown(Ion::Keyboard::Key k) { Queue::sharedQueue()->push(State(k)); }

void keyUp(Ion::Keyboard::Key k) { Queue::sharedQueue()->push(State(0)); }

bool scanHandlesSDLKey(SDL_Scancode key) {
  for (int i = 0; i < sNumberOfKeyPairs; i++) {
    if (key == sKeyPairs[i].SDLKey()) {
      return true;
    }
  }
  return false;
}

}  // namespace Keyboard
}  // namespace Simulator
}  // namespace Ion
