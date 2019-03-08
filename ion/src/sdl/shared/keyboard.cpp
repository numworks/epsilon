#include "main.h"
#include "layout.h"

#include <ion/keyboard.h>
#include <SDL.h>

namespace Ion {
namespace Keyboard {

State scan() {
  // We need to tell SDL to get new state from the host OS
  SDL_PumpEvents();

  // Grab this opportunity to refresh the display if needed
  SDL::Main::refresh();

  // Start with a "clean" state
  State state;

  // Register a key for the mouse, if any
  SDL_Point p;
  Uint32 mouseState = SDL_GetMouseState(&p.x, &p.y);
  if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    Key k = SDL::Layout::keyAt(&p);
    state.setKey(k);
  }

#if 0
  // SDL exposes the first finger as a mouse!
  // Register a key for each finger, if any
  int numberOfTouchDevices = SDL_GetNumTouchDevices();
  for (int i=0; i<numberOfTouchDevices; i++) {
    SDL_TouchID touchDevice = SDL_GetTouchDevice(touchDevice);
    int numberOfFingers = SDL_GetNumTouchFingers(i);
    for (int j=0; j<numberOfFingers; j++) {
      SDL_Finger * finger = SDL_GetTouchFinger(touchDevice, j);
      // Todo: Convert the finger's coordinate into pixel coordinates
      //SDL_Point p =
      //Key k= SDL::Layout::keyAt(&p);
      //state.setKey(k);
    }
  }
#endif

  return state;
}

}
}
