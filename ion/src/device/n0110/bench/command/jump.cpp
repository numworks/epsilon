#include "command.h"
#include <ion/src/device/shared/drivers/reset.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

// Input must an address with format 0x20000000
void Jump(const char * input) {
  if (input == nullptr || input[0] != '0' || input[1] != 'x') {
    reply(sSyntaxError);
    return;
  }
  constexpr int addressOffset = 2;
  for (int i = 0; i < 8; i++) {
    if (!isHex(input[addressOffset+i])) {
      reply(sSyntaxError);
      return;
    }
  }
  uint32_t address = hexNumber(input+addressOffset, 8);
  reply(sOK);
  Ion::Device::Reset::jump(address);
}

}
}
}
}
