#include <shared/drivers/board.h>
#include <shared/drivers/config/board.h>

extern "C" {
extern char _storage_flash_start;
}

namespace Ion {
namespace Device {
namespace Board {

bool isRunningSlotA() {
  return reinterpret_cast<uint32_t>(&_storage_flash_start) < ExternalFlash::Config::StartAddress + ExternalFlash::Config::TotalSize/2;
}

}
}
}
