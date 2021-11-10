#include <ion/usb.h>

namespace Ion {
namespace USB {

bool s_plugged = false;

bool isPlugged() {
  return s_plugged;
}

bool isEnumerated() {
  // Unused on simulator
  return false;
}

void clearEnumerationInterrupt() {
}

void DFU() {
}

/* To be able to fully emulate plugged states and events on simulator, we take
 * advantage of enable and disable methods, which are conveniently called
 * in AppsContainer::processEvent when processing USBPlug events. */

// Called when a USBPlugged event is received while being "plugged"
void enable() {
  // "Unplug" the simulator.
  s_plugged = false;
}

// Called when a USBPlugged event is received while being "unplugged"
void disable() {
  // "Plug" the simulator.
  s_plugged = true;
}

}
}
