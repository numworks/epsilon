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

void DFU(bool, void*) {
}

void enable() {
}

void disable() {
}

}
}
