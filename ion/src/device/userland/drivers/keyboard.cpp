#include <ion/keyboard.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Keyboard {

State SVC_ATTRIBUTES scan() {
  SVC(SVC_KEYBOARD_SCAN);
}


}
}
