#include <drivers/svcall.h>
#include <ion/keyboard.h>

namespace Ion {
namespace Keyboard {

uint64_t SVC_ATTRIBUTES privateScan(){
    SVC_RETURNING_R0R1(SVC_KEYBOARD_SCAN, State)}

uint64_t SVC_ATTRIBUTES
    privatePopState(){SVC_RETURNING_R0R1(SVC_KEYBOARD_POP_STATE, State)}

State scan() {
  return State(privateScan());
}

State scanForInterruptionAndPopState() {
  /* There is no need to manually scan to get the interruptions, as they are
   * done in hardware. We only need to do it on simulators. */
  return popState();
}

State popState() { return State(privatePopState()); }

}  // namespace Keyboard
}  // namespace Ion
