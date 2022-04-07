#include <bootloader/recovery.h>
#include <ion.h>
#include <ion/src/device/n0110/drivers/power.h>
#include <ion/src/device/shared/drivers/reset.h>
#include <ion/src/device/shared/drivers/board.h>
#include <assert.h>

#include <bootloader/interface.h>
#include <bootloader/slot.h>
#include <bootloader/usb_data.h>

constexpr static uint32_t MagicStorage = 0xEE0BDDBA;

void Bootloader::Recovery::crash_handler(const char *error) {
  Ion::Device::Board::shutdownPeripherals(true);
  Ion::Device::Board::initPeripherals(false);
  Ion::Timing::msleep(100);
  Ion::Backlight::init();
  Ion::Backlight::setBrightness(180);

  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorWhite);
  Interface::drawCrash(error);

  while (true) {

  }
}

bool Bootloader::Recovery::has_crashed() {
  bool isA = Bootloader::Slot::A().kernelHeader()->isValid() && Bootloader::Slot::A().userlandHeader()->isValid();
  bool isB = Bootloader::Slot::B().kernelHeader()->isValid() && Bootloader::Slot::B().userlandHeader()->isValid();

  bool isACrashed = false;
  bool isBCrashed = false;

  if (isA) {
    const uint32_t * storage = (uint32_t *)Bootloader::Slot::A().userlandHeader()->storageAddress();
    isACrashed = *storage == MagicStorage;
  }

  if (isB) {
    const uint32_t * storage = (uint32_t *)Bootloader::Slot::B().userlandHeader()->storageAddress();
    isBCrashed = *storage == MagicStorage;
  }

  return (isACrashed || isBCrashed);
}

Bootloader::Recovery::CrashedSlot Bootloader::Recovery::getSlotConcerned() {
  bool isA = Bootloader::Slot::A().kernelHeader()->isValid() && Bootloader::Slot::A().userlandHeader()->isValid();
  bool isB = Bootloader::Slot::B().kernelHeader()->isValid() && Bootloader::Slot::B().userlandHeader()->isValid();

  bool isACrashed = false;
  bool isBCrashed = false;

  if (isA) {
    const uint32_t * storage = (uint32_t *)Bootloader::Slot::A().userlandHeader()->storageAddress();
    isACrashed = *storage == MagicStorage;
  }

  if (isB) {
    const uint32_t * storage = (uint32_t *)Bootloader::Slot::B().userlandHeader()->storageAddress();
    isBCrashed = *storage == MagicStorage;
  }

  assert(isACrashed || isBCrashed);

  if (isACrashed) {
    return CrashedSlot(Bootloader::Slot::A().userlandHeader()->storageSize(), Bootloader::Slot::A().userlandHeader()->storageAddress());
  } else {
    return CrashedSlot(Bootloader::Slot::B().userlandHeader()->storageSize(), Bootloader::Slot::B().userlandHeader()->storageAddress());
  }
}

void Bootloader::Recovery::recover_data() {
  Ion::Device::Board::initPeripherals(false);
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorWhite);
  Ion::Backlight::init();
  Interface::drawRecovery();

  uint64_t scan = 0;

  USBData udata = USBData::Recovery((uint32_t)getSlotConcerned().getStorageAddress(), (uint32_t)getSlotConcerned().getStorageSize());

  while (scan != Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
    Ion::USB::enable();
    do {
      scan = Ion::Keyboard::scan();
      if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
        Ion::USB::disable();
      }
    } while (!Ion::USB::isEnumerated() && scan != Ion::Keyboard::State(Ion::Keyboard::Key::Back));

    if (scan != Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
      Ion::USB::DFU(true, &udata);
    }
  }

  // Invalidate storage header
  *(uint32_t *)(getSlotConcerned().getStorageAddress()) = (uint32_t)0x0;
  
}

void Bootloader::Recovery::debug() {
  bool isA = Bootloader::Slot::A().kernelHeader()->isValid() && Bootloader::Slot::A().userlandHeader()->isValid();
  bool isB = Bootloader::Slot::B().kernelHeader()->isValid() && Bootloader::Slot::B().userlandHeader()->isValid();

  if (isA) {  
    const uint32_t * storage = (uint32_t *)Bootloader::Slot::A().userlandHeader()->storageAddress();
    *(uint32_t *)(0x20000070) = *storage;
    if (*storage == MagicStorage) {
      *(uint32_t *)(0x20000070 + (sizeof(uint32_t))) = 0xDEADBEEF;
    }
  }

  if (isB) {
    const uint32_t * storage = (uint32_t *)Bootloader::Slot::B().userlandHeader()->storageAddress();
    *(uint32_t *)(0x20000070 + 2*(sizeof(uint32_t))) = *storage;
    if (*storage == MagicStorage) {
      *(uint32_t *)(0x20000070 + 3*(sizeof(uint32_t))) = 0xDEADBEEF;
    }
  }

  if (has_crashed()) {
    *(uint32_t *)(0x20000070 + 4*(sizeof(uint32_t))) = 0xDEADBEEF;
  } else {
    *(uint32_t *)(0x20000070 + 4*(sizeof(uint32_t))) = 0xBEEFDEAD;
  }

}
