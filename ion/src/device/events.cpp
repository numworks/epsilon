#include <ion.h>

namespace Ion {
namespace Events {

Event getEvent(int * timeout) {
  return Keyboard::getEvent(timeout);
}

}
}
