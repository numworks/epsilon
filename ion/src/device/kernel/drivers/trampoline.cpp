#include <kernel/drivers/trampoline.h>
#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Trampoline {

uint32_t addressOfFunction(int index) {
  return Board::Config::BootloaderTrampolineAddress + sizeof(void *) * index;
}

}
}
}
