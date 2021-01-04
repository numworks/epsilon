#include <ion/usb.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace USB {

void SVC_ATTRIBUTES isPluggedSVC(bool * res) {
  SVC(SVC_USB_IS_PLUGGED);
}

bool isPlugged() {
  bool res;
  isPluggedSVC(&res);
  return res;
}

void SVC_ATTRIBUTES isEnumeratedSVC(bool * res) {
  SVC(SVC_USB_IS_ENUMERATED);
}

bool isEnumerated() {
  bool res;
  isEnumeratedSVC(&res);
  return res;
}

void clearEnumerationInterrupt() {
  SVC(SVC_USB_CLEAR_ENUMERATION_INTERRUPT);
}

void enable() {
  SVC(SVC_USB_ENABLE);
}

void disable() {
  SVC(SVC_USB_DISABLE);
}

void DFU() {
  SVC(SVC_USB_DFU);
}

}
}
