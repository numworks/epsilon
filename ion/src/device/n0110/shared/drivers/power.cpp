#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/trampoline.h>

namespace Ion {
namespace Device {
namespace Power {

void standby() {
  Power::waitUntilOnOffKeyReleased();
  Power::standbyConfiguration();
  Board::shutdownPeripherals();
  Board::shutdownPeripheralsClocks();
  bootloaderSuspend();
}

TRAMPOLINE_INTERFACE(TRAMPOLINE_SUSPEND, bootloaderSuspend, (), void, void)

}
}
}
