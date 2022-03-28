#include <ion.h>
#include <ion/usb.h>

namespace Ion {
namespace USB {

bool isPlugged() {
  return false;
}

bool isEnumerated() {
  return false;
}

void clearEnumerationInterrupt() {
}

void DFU(bool, bool, int) {
}

void enable() {
}

void disable() {
}

}
}
