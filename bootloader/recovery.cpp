#include <bootloader/recovery.h>
#include <ion.h>
#include <ion/src/device/n0110/drivers/power.h>
#include <ion/src/device/shared/drivers/reset.h>
#include <ion/src/device/shared/drivers/board.h>
#include <assert.h>

#include <bootloader/interface/static/interface.h>
#include <bootloader/slots/slot.h>
#include <bootloader/usb_data.h>
#include <bootloader/interface/menus/slot_recovery.h>
#include <bootloader/interface/menus/crash.h>

constexpr static uint32_t MagicStorage = 0xEE0BDDBA;

void Bootloader::Recovery::crash_handler(const char *error) {
  Ion::Device::Board::shutdownPeripherals(true);
  Ion::Device::Board::initPeripherals(false);
  Ion::Timing::msleep(100);
  Ion::Backlight::init();
  Ion::Backlight::setBrightness(180);

  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorWhite);
  CrashMenu menu(error);
  menu.open(true);
}

bool Bootloader::Recovery::hasCrashed() {
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

void Bootloader::Recovery::recoverData() {
  Ion::Device::Board::initPeripherals(false);
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorWhite);
  Ion::Backlight::init();

  USBData udata = USBData::Recovery((uint32_t)getSlotConcerned().getStorageAddress(), (uint32_t)getSlotConcerned().getStorageSize());
  
  SlotRecoveryMenu menu = SlotRecoveryMenu(&udata);
  menu.open();

  // Invalidate storage header
  *(uint32_t *)(getSlotConcerned().getStorageAddress()) = (uint32_t)0x0;
  
}
