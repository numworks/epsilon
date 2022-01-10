#include <ion/keyboard.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Keyboard {

uint64_t SVC_ATTRIBUTES privateScan() {
  SVC_RETURNING_R0R1(SVC_KEYBOARD_SCAN, State)
}

uint64_t SVC_ATTRIBUTES privatePopState() {
  SVC_RETURNING_R0R1(SVC_KEYBOARD_POP_STATE, State)
}

State scan() {
  return State(privateScan());
}

State popState() {
  return State(privatePopState());
}

}
}
