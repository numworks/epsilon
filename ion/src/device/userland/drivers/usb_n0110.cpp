#include <ion/usb.h>
#include <ion/authentication.h>
#include <shared/drivers/board_unprivileged.h>
#include <shared/drivers/usb.h>
#include <userland/drivers/config/usb.h>
#include <userland/drivers/svcall.h>

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

const char * stringDescriptor() {
  if (Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks || Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorksAndThirdPartyApps) {
    return Board::isRunningSlotA() ? Config::InterfaceFlashStringDescriptorAuthenticatedSlotA : Config::InterfaceFlashStringDescriptorAuthenticatedSlotB;
  } else {
    return Board::isRunningSlotA() ? Config::InterfaceFlashStringDescriptorThirdPartySlotA : Config::InterfaceFlashStringDescriptorThirdPartySlotB;
  }
}

}
}
}

