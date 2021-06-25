#include <ion/usb.h>
#include <userland/drivers/board.h>
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

void SVC_ATTRIBUTES willExecuteDFU() {
  SVC_RETURNING_VOID(SVC_USB_WILL_EXECUTE_DFU)

  // Keep usefull information about the currently running slot
  slotInfo()->update();
}

void SVC_ATTRIBUTES didExecuteDFU() {
  SVC_RETURNING_VOID(SVC_USB_DID_EXECUTE_DFU)
}

bool SVC_ATTRIBUTES shouldInterruptDFU() {
  SVC_RETURNING_R0(SVC_USB_SHOULD_INTERRUPT, bool)
}

SlotInfo * slotInfo() {
  static SlotInfo __attribute__((used)) __attribute__((section(".slot_info"))) slotInformation;
  return &slotInformation;
}

void SlotInfo::update() {
  m_kernelHeaderAddress =  Board::kernelHeader();
  m_userlandHeaderAddress =  Board::userlandHeader();
}

}
}
}
