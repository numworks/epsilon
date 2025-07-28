#include <ion/authentication.h>
#include <ion/usb.h>
#include <shared/drivers/board_shared.h>
#include <shared/usb/dfu_interfaces.h>

#include <new>

#include "svcall.h"
#include "usb.h"

namespace Ion {
namespace USB {

bool SVC_ATTRIBUTES isPlugged() { SVC_RETURNING_R0(SVC_USB_IS_PLUGGED, bool) }

}  // namespace USB
}  // namespace Ion

namespace Ion {
namespace Device {
namespace USB {

bool SVC_ATTRIBUTES shouldInterruptDFU(Keyboard::State exitKeys,
                                       bool bubbleUpEvents) {
  SVC_RETURNING_R0(SVC_USB_SHOULD_INTERRUPT, bool)
}

constexpr static StringDescriptor k_flashDescriptorAuthenticatedSlotA(
    Config::InterfaceFlashStringDescriptorAuthenticatedSlotA);
constexpr static StringDescriptor k_flashDescriptorAuthenticatedSlotB(
    Config::InterfaceFlashStringDescriptorAuthenticatedSlotB);
constexpr static StringDescriptor k_flashDescriptorThirdPartySlotA(
    Config::InterfaceFlashStringDescriptorThirdPartySlotA);
constexpr static StringDescriptor k_flashDescriptorThirdPartySlotB(
    Config::InterfaceFlashStringDescriptorThirdPartySlotB);
constexpr static StringDescriptor k_SRAMDescriptor(
    Config::InterfaceSRAMStringDescriptor);

const StringDescriptor* stringDescriptor(int index) {
  assert(index < 2);
  if (index == 1) {
    return &k_SRAMDescriptor;
  }

  if (Authentication::clearanceLevel() ==
          Ion::Authentication::ClearanceLevel::NumWorks ||
      Authentication::clearanceLevel() ==
          Ion::Authentication::ClearanceLevel::NumWorksAndThirdPartyApps) {
    return Board::isRunningSlotA() ? &k_flashDescriptorAuthenticatedSlotA
                                   : &k_flashDescriptorAuthenticatedSlotB;
  } else {
    return Board::isRunningSlotA() ? &k_flashDescriptorThirdPartySlotA
                                   : &k_flashDescriptorThirdPartySlotB;
  }
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
