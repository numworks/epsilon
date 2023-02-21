#ifndef ION_SIMULATOR_KEYBOARD_H
#define ION_SIMULATOR_KEYBOARD_H

#include <SDL.h>
#include <ion/keyboard.h>

namespace Ion {
namespace Simulator {
namespace Keyboard {

void keyDown(Ion::Keyboard::Key k);
void keyUp(Ion::Keyboard::Key k);
bool scanHandlesSDLKey(SDL_Scancode key);
void didScan();

}  // namespace Keyboard
}  // namespace Simulator
}  // namespace Ion

#endif
