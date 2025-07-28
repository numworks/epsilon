#include <emscripten.h>

#include "../shared/keyboard.h"

namespace Ion {
namespace Simulator {
namespace Keyboard {

void didScan() {
  /* The following call to emscripten_sleep gives the JS VM a chance to do a run
   * loop iteration. This in turns gives the browser an opportunity to call the
   * IonEventsEmscriptenPushKey function, therefore modifying the sKeyboardState
   * global variable before it is returned by this Ion::Keyboard::scan.
   * On Emterpreter-async, emscripten_sleep is actually a wrapper around the JS
   * function setTimeout, which can be called with a value of zero. Doing so
   * puts the callback at the end of the queue of callbacks to be processed. */
  emscripten_sleep(0);
}

}  // namespace Keyboard
}  // namespace Simulator
}  // namespace Ion
