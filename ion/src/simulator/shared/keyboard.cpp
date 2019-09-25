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

State scan() {
  // We need to tell SDL to get new state from the host OS
  SDL_PumpEvents();

  // Notify callbacks in case we need to do something
  IonSimulatorCallbackDidScanKeyboard();

  // Grab this opportunity to refresh the display if needed
  SDL::Main::refresh();

#if EPSILON_SDL_SCREEN_ONLY
  // In this case, keyboard states will be sent over another channel
  return sKeyboardState;
#else
  // Start with a "clean" state
  State state;

  // Register a key for the mouse, if any
  SDL_Point p;
  Uint32 mouseState = SDL_GetMouseState(&p.x, &p.y);
  if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    Key k = SDL::Layout::keyAt(&p);
    state.setKey(k);
  }

  return state;
#endif
}

}
}
