#include <ion/exam_mode.h>
#include <ion/external_apps.h>
#include <shared/drivers/board_shared.h>
#include <shared/drivers/usb.h>

#include "board.h"
#include "svcall.h"

namespace Ion {
namespace Device {
namespace USB {

void SVC_ATTRIBUTES willExecuteDFU() {
  SVC_RETURNING_VOID(SVC_USB_WILL_EXECUTE_DFU)

  // Keep useful information about the currently running slot
  slotInfo()->updateUserlandHeader();
}

void SVC_ATTRIBUTES didExecuteDFU() {
  Ion::ExternalApps::updateClearanceLevel(Ion::ExamMode::get().isActive());
  SVC_RETURNING_VOID(SVC_USB_DID_EXECUTE_DFU)
}

SlotInfo* slotInfo() {
  static SlotInfo __attribute__((used)) __attribute__((section(".slot_info")))
  slotInformation;
  return &slotInformation;
}

void SlotInfo::updateUserlandHeader() {
  m_userlandHeaderAddress = Board::userlandHeader();
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
