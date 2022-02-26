#include <bootloader/boot.h>
#include <bootloader/slot.h>
#include <ion.h>
#include <bootloader/interface.h>

#include <assert.h>

namespace Bootloader {

BootMode Boot::mode() {
  // We use the exam mode driver as storage for the boot mode
  uint8_t mode = Ion::ExamMode::FetchExamMode();

  if (mode > 3)
    return Unknown;
  
  return (BootMode) mode;
}

void Boot::setMode(BootMode mode) {
  BootMode currentMode = Boot::mode();
  if (currentMode == mode)
    return;

  assert(mode != BootMode::Unknown);
  int8_t deltaMode = (int8_t)mode - (int8_t)currentMode;
  deltaMode = deltaMode < 0 ? deltaMode + 4 : deltaMode;
  assert(deltaMode > 0);
  Ion::ExamMode::IncrementExamMode(deltaMode);
}

[[ noreturn ]] void Boot::boot() {
  assert(mode() != BootMode::Unknown);

  if (mode() == BootMode::SlotA)
    Slot::A().boot();
  else if (mode() == BootMode::SlotB)
    Slot::B().boot();
  
  for(;;);
}

[[ noreturn ]] void Boot::bootloader() {
  Bootloader::Interface::draw();
  for(;;) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated());
    Ion::USB::DFU(false);
  }
}

}
