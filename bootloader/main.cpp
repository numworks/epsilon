
#include <ion.h>
#include <assert.h>

#include <bootloader/boot.h>
#include <bootloader/interface.h>
#include <bootloader/slot.h>
#include <bootloader/slot_exam_mode.h>
#include <bootloader/recovery.h>
#include <ion/src/device/shared/drivers/flash.h>

__attribute__ ((noreturn)) void ion_main(int argc, const char * const argv[]) {
  // Clear the screen
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorWhite);
  // Initialize the backlight
  Ion::Backlight::init();

  // We check if there is a slot in exam_mode

  bool isSlotA = Bootloader::Slot::A().kernelHeader()->isValid();

  if (isSlotA) {
    Bootloader::ExamMode::ExamMode SlotAExamMode = (Bootloader::ExamMode::ExamMode)Bootloader::ExamMode::SlotsExamMode::FetchSlotAExamMode(Bootloader::Slot::A().kernelHeader()->isNewVersion());
    if (SlotAExamMode != Bootloader::ExamMode::ExamMode::Off && SlotAExamMode != Bootloader::ExamMode::ExamMode::Unknown) {
      // We boot the slot in exam_mode
      Bootloader::Slot::A().boot();
    } 
  }

  bool isSlotB = Bootloader::Slot::B().kernelHeader()->isValid();

  if (isSlotB) {
    Bootloader::ExamMode::ExamMode SlotBExamMode = (Bootloader::ExamMode::ExamMode)Bootloader::ExamMode::SlotsExamMode::FetchSlotBExamMode(Bootloader::Slot::B().kernelHeader()->isNewVersion());
    if (SlotBExamMode != Bootloader::ExamMode::ExamMode::Off && SlotBExamMode != Bootloader::ExamMode::ExamMode::Unknown && isSlotB) {
      // We boot the slot in exam_mode
      Bootloader::Slot::B().boot();
    }
    
  }

  if (Bootloader::Recovery::has_crashed()) {
    Bootloader::Recovery::recover_data();
  }

  Bootloader::Interface::drawLoading();

  // Boot the firmware
  Bootloader::Boot::boot();
}
