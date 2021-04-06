#include "board.h"
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t SVC_ATTRIBUTES switchExecutableSlot() {
  SVC(SVC_BOARD_SWITCH_EXECUTABLE_SLOT);
}

}
}
}
