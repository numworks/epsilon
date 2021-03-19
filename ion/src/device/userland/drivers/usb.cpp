#include <ion/usb.h>
#include <userland/drivers/svcall.h>
#include <shared/drivers/usb.h>

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

}
}

namespace Ion {
namespace Device {
namespace USB {

void willExecuteDFU() {
  SVC(SVC_USB_WILL_EXECUTE_DFU);
}

void didExecuteDFU() {
  SVC(SVC_USB_DID_EXECUTE_DFU);
}

void SVC_ATTRIBUTES shouldInterruptDFUSVC(bool * res) {
  SVC(SVC_USB_SHOULD_INTERRUPT);
}

bool shouldInterruptDFU() {
  bool res;
  shouldInterruptDFUSVC(&res);
  return res;
}

}
}
}
