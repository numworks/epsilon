#include "calculator.h"
#include <userland/drivers/board.h>
#include <userland/drivers/reset.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  uint32_t addressToJumpTo = Board::switchExecutableSlot();
  if (addressToJumpTo) {
    Reset::jump(Board::switchExecutableSlot());
  }
}

}
}
}
