#include <drivers/reset.h>
#include <drivers/board.h>
#include <drivers/cache.h>
#include <drivers/config/internal_flash.h>

namespace Ion {
namespace Device {
namespace Reset {

void coreWhilePlugged() {
  /* Mimic a hardware reset:
   * - disable cache
   * - shutdown all clocks and peripherals.
   */
  Cache::disable();
  Board::shutdown();

  /* We don't perform a core reset because at this point in time the USB cable
   * might be plugged in. Doing a full core reset would result in the device
   * entering the ST DFU bootloader. By performing a jump-reset, we mimic the
   * core reset without entering ST bootloader.*/
  jump(InternalFlash::Config::StartAddress, true);
}

}
}
}
