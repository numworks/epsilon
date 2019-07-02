#include <ion/events.h>
#include <ion/usb.h>
#include <assert.h>

namespace Ion {
namespace Events {

bool sLastUSBPlugged = false;
bool sLastUSBEnumerated = false;

Event getPlatformEvent() {
  // First, check if the USB plugged status has changed
  bool usbPlugged = USB::isPlugged();
  if (usbPlugged != sLastUSBPlugged) {
    sLastUSBPlugged = usbPlugged;
    return USBPlug;
  }

  // Second, check if the USB device has been connected to an USB host
  bool usbEnumerated = USB::isEnumerated();
  if (usbEnumerated != sLastUSBEnumerated) {
    sLastUSBEnumerated = usbEnumerated;
    if (usbEnumerated) {
      return USBEnumeration;
    }
  }

  return None;
}

}
}
