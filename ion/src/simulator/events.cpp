#include <ion.h>
#include "init.h"

namespace Ion {
namespace Events {

Event getEvent(int * timeout) {
  if (shouldTerminate) {
    return Termination;
  }
  return Keyboard::getEvent(timeout);
}

}
}
