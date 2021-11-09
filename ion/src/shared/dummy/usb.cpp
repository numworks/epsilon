#include <ion/usb.h>

namespace Ion {
namespace USB {

bool plugged = false;

bool isPlugged() {
  return plugged;
}

bool isEnumerated() {
  // Unused on simulator
  return false;
}

void clearEnumerationInterrupt() {
}

void DFU() {
}

void enable() {
  // A USBPlugged event on an already "plugged" simulator will "unplug" it.
  plugged = false;
}

void disable() {
  // A USBPlugged event on an "uplugged" simulator will "plug" it.
  plugged = true;
}

}
}
