#include <ion/keyboard.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Keyboard {

State SVC_ATTRIBUTES scan() {
  SVC_RETURNING_STASH_ADDRESS_IN_R0(SVC_KEYBOARD_SCAN, State)
}


}
}
