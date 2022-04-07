#include <ion/usb.h>

#include "common.h"

bool Ion::USB::isPlugged() {
  return Ion::Simulator::CommonDriver::isPlugged();
}

bool Ion::USB::isEnumerated() {
  return false;
}

void Ion::USB::clearEnumerationInterrupt() {
}

void Ion::USB::DFU(bool, void *) {
}

void Ion::USB::enable() {
}

void Ion::USB::disable() {
}
