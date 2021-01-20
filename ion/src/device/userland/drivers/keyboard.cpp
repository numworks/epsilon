#include <ion/keyboard.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Keyboard {

void SVC_ATTRIBUTES scanSVC(State * s) {
  SVC(SVC_KEYBOARD_SCAN);
}

State scan() {
  State res;
  scanSVC(&res);
  return res;
}


}
}
