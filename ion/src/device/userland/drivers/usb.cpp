#include <ion/usb.h>
#include <userland/drivers/svcall.h>
#include <shared/drivers/usb.h>

namespace Ion {
namespace USB {

bool SVC_ATTRIBUTES isPlugged() {
  SVC(SVC_USB_IS_PLUGGED);
}

}
}

namespace Ion {
namespace Device {
namespace USB {

void SVC_ATTRIBUTES willExecuteDFU() {
  SVC(SVC_USB_WILL_EXECUTE_DFU);
}

void SVC_ATTRIBUTES didExecuteDFU() {
  SVC(SVC_USB_DID_EXECUTE_DFU);
}

bool SVC_ATTRIBUTES shouldInterruptDFU() {
  SVC(SVC_USB_SHOULD_INTERRUPT);
}

}
}
}
