#include <ion/usb.h>

namespace Ion {
namespace USB {

bool s_plugged = false;

void togglePlug() { s_plugged = !s_plugged; }

bool isPlugged() { return s_plugged; }

bool isEnumerated() {
  // Unused on simulator
  return false;
}

void clearEnumerationInterrupt() {}

void enable() {}

void disable() {}

void DFU(DFUParameters) {}

}  // namespace USB
}  // namespace Ion
