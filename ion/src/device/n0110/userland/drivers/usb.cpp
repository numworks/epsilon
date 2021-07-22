#include <ion/usb.h>
#include <userland/drivers/svcall.h>
#include <shared/drivers/usb.h>

namespace Ion {
namespace USB {

bool SVC_ATTRIBUTES isPlugged() {
  SVC_RETURNING_R0(SVC_USB_IS_PLUGGED, bool)
}

}
}

namespace Ion {
namespace Device {
namespace USB {

bool SVC_ATTRIBUTES shouldInterruptDFU() {
  SVC_RETURNING_R0(SVC_USB_SHOULD_INTERRUPT, bool)
}

}
}
}

