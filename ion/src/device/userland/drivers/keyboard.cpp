#include <ion/keyboard.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Keyboard {

void SVC_ATTRIBUTES hasNextStateSVC(bool * res) {
  SVC(SVC_KEYBOARD_HAS_NEXT_STATE);
}

bool hasNextState() {
  bool res;
  hasNextStateSVC(&res);
  return res;
}


void SVC_ATTRIBUTES nextStateSVC(State * s) {
  SVC(SVC_KEYBOARD_NEXT_STATE);
}

State nextState() {
  State res;
  nextStateSVC(&res);
  return res;
}

}
}
