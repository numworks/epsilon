#include <shared/drivers/board.h>
#include <config/board.h>

extern "C" {
extern char _external_apps_flash_start;
}

namespace Ion {
namespace Device {
namespace Board {

bool isRunningSlotA() {
  return Device::Config::SlotOrigin == Config::SlotAOrigin;
}

}
}
}
