#ifndef ION_DEVICE_SHARED_USB_HELPERS_USBFS_H
#define ION_DEVICE_SHARED_USB_HELPERS_USBFS_H

#include <stm32u083/usb.h>

namespace Ion {
namespace Device {
namespace USB {

enum class Status {
  Disabled = 0b00,
  Stall = 0b01,
  NAK = 0b10,
  Valid = 0b11,
};

/* The CHEPXR registers cannot be modified with a normal read-modify-write cycle
 * since some bits may be modified by the hardware in-between. Instead we build
 * an invariant value that should preserve the register when written and modify
 * some fields. */

class CHEP0R {
  using USBFS = STM32U083::USB;

 public:
  static void ClearVTRX() { InvariantCHEP0R().setVTRX(false).write(); }
  static void ClearVTTX() { InvariantCHEP0R().setVTTX(false).write(); }

  static void SetEPKIND(bool kind) {
    InvariantCHEP0R().setEPKIND(kind).write();
  }

  static Status GetSTATTX() {
    return static_cast<Status>(USBFS::CHEP0R::Read().getBits(4, 2));
  }

  static Status GetSTATRX() {
    return static_cast<Status>(USBFS::CHEP0R::Read().getBits(12, 2));
  }

  static void SetSTATTX(Status s) {
    InvariantCHEP0R().setSTATTX(BitsToToggle(GetSTATTX(), s)).write();
  }

  static void SetSTATRX(Status s) {
    InvariantCHEP0R().setSTATRX(BitsToToggle(GetSTATRX(), s)).write();
  }

 private:
  static USBFS::CHEP0R InvariantCHEP0R() {
    USBFS::CHEP0R old = USBFS::CHEP0R::Read();
    // Preserve RW fields, keep Toggle fields to 0 and set W0 fields to 1
    return USBFS::CHEP0R::Reset()
        .setUTYPE(old.getUTYPE())
        .setEPKIND(old.getEPKIND())
        .setEA(old.getEA())
        .setVTTX(true)
        .setVTRX(true);
  }

  static uint8_t BitsToToggle(Status current, Status wanted) {
    return static_cast<uint8_t>(current) ^ static_cast<uint8_t>(wanted);
  }
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif
