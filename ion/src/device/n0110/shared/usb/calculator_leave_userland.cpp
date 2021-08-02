#include <ion/external_apps.h>
#include <shared/drivers/config/board.h>
#include <shared/drivers/userland_header.h>
#include <shared/usb/calculator.h>
#include <userland/drivers/board.h>
#include <userland/drivers/reset.h>

extern "C" {
extern char _storage_flash_start;
extern char _storage_flash_end;
}

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  if (leaveAddress >= reinterpret_cast<uint32_t>(&_storage_flash_start) && leaveAddress < reinterpret_cast<uint32_t>(&_storage_flash_end)) {
    Ion::ExternalApps::setVisible();
    return;
  }
  Board::switchExecutableSlot(leaveAddress);
  /* The jump can't be done from switchExecutableSlot since we need to
   * terminate the interruption procedure before jumping. */
  Reset::jump(leaveAddress + sizeof(UserlandHeader));
}

}
}
}
