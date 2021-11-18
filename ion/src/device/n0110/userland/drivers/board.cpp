#include <shared/drivers/board_unprivileged.h>
#include <shared/drivers/config/board.h>

extern "C" {
extern char _data_section_start_flash;
}

namespace Ion {
namespace Device {
namespace Board {

bool isRunningSlotA() {
  return reinterpret_cast<uint32_t>(&_data_section_start_flash) < Board::Config::SlotBStartAddress;
}

}
}
}
