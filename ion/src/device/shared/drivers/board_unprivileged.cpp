#include <shared/drivers/board_shared.h>
#include <shared/drivers/userland_header.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t userlandStart() {
  return reinterpret_cast<uint32_t>(userlandHeader()) + sizeof(UserlandHeader);
}

}
}
}
