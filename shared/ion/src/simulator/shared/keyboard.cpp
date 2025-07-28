#include "keyboard.h"

#include <SDL.h>
#include <ion/keyboard.h>
#include <ion/src/shared/keyboard.h>
#include <ion/src/shared/keyboard_queue.h>

#include <array>

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
    KeySDLKeyPair(Key::EXE, SDL_SCANCODE_KP_ENTER),
    KeySDLKeyPair(Key::Back, SDL_SCANCODE_ESCAPE),
    KeySDLKeyPair(Key::Toolbox, SDL_SCANCODE_TAB),
};

constexpr static KeySDLKeyPair sKeyPairsPythonAddon[] = {
    KeySDLKeyPair(Key::Alpha, SDL_SCANCODE_LALT),
    KeySDLKeyPair(Key::Alpha, SDL_SCANCODE_RALT),
    KeySDLKeyPair(Key::Backspace, SDL_SCANCODE_BACKSPACE),
    // KeySDLKeyPair(Key::XNT, SDL_SCANCODE_X),
    // KeySDLKeyPair(Key::XNT, SDL_SCANCODE_N),
    // KeySDLKeyPair(Key::XNT, SDL_SCANCODE_T),
    // KeySDLKeyPair(Key::Comma, SDL_SCANCODE_COMMA),
    // KeySDLKeyPair(Key::Square, SDL_SCANCODE_GRAVE),
    KeySDLKeyPair(Key::Zero, SDL_SCANCODE_0),
    KeySDLKeyPair(Key::Zero, SDL_SCANCODE_KP_0),
    KeySDLKeyPair(Key::One, SDL_SCANCODE_1),
    KeySDLKeyPair(Key::One, SDL_SCANCODE_KP_1),
    KeySDLKeyPair(Key::Two, SDL_SCANCODE_2),
    KeySDLKeyPair(Key::Two, SDL_SCANCODE_KP_2),
    KeySDLKeyPair(Key::Three, SDL_SCANCODE_3),
    KeySDLKeyPair(Key::Three, SDL_SCANCODE_KP_3),
    KeySDLKeyPair(Key::Four, SDL_SCANCODE_4),
    KeySDLKeyPair(Key::Four, SDL_SCANCODE_KP_4),
    KeySDLKeyPair(Key::Five, SDL_SCANCODE_5),
    KeySDLKeyPair(Key::Five, SDL_SCANCODE_KP_5),
    KeySDLKeyPair(Key::Six, SDL_SCANCODE_6),
    KeySDLKeyPair(Key::Six, SDL_SCANCODE_KP_6),
    KeySDLKeyPair(Key::Seven, SDL_SCANCODE_7),
    KeySDLKeyPair(Key::Seven, SDL_SCANCODE_KP_7),
    KeySDLKeyPair(Key::Eight, SDL_SCANCODE_8),
    KeySDLKeyPair(Key::Eight, SDL_SCANCODE_KP_8),
    KeySDLKeyPair(Key::Nine, SDL_SCANCODE_9),
    KeySDLKeyPair(Key::Nine, SDL_SCANCODE_KP_9),
    KeySDLKeyPair(Key::Multiplication, SDL_SCANCODE_KP_MULTIPLY),
    KeySDLKeyPair(Key::Division, SDL_SCANCODE_SLASH),
    KeySDLKeyPair(Key::Division, SDL_SCANCODE_KP_DIVIDE),
    KeySDLKeyPair(Key::Plus, SDL_SCANCODE_KP_PLUS),
    KeySDLKeyPair(Key::Minus, SDL_SCANCODE_MINUS),
    KeySDLKeyPair(Key::Minus, SDL_SCANCODE_KP_MINUS),
    KeySDLKeyPair(Key::Dot, SDL_SCANCODE_KP_PERIOD),
    KeySDLKeyPair(Key::Dot, SDL_SCANCODE_PERIOD),
};

constexpr int sNumberOfKeyPairs = std::size(sKeyPairs);
constexpr int sNumberOfKeyPairsPython = std::size(sKeyPairsPythonAddon);

namespace Ion {
namespace Keyboard {

State scanForInterruptionAndPopState() {
  // We get eventual user interruptions via the simulator keyboard
  scan();
  return popState();
}

/* forPython allows gathering more keyboard information when the requester is
 * python. It is useful because most simulator keys are mapped via ExternalText
 * while not on Python. But Python does not use this event detection system
 * and so misses a lot of relevent keypresses via `ion.keydown` */
State scan(bool forPython) {
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
  if (forPython) {
    for (int i = 0; i < sNumberOfKeyPairsPython; i++) {
      KeySDLKeyPair pair = sKeyPairsPythonAddon[i];
      if (SDLstate[pair.SDLKey()]) {
        state.setKey(pair.key());
      }
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
