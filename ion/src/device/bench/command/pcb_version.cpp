#include "command.h"
#include <drivers/board.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void PCBVersion(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }

  Board::writePCBVersion(PCB_LATEST);
  if (Board::readPCBVersion() != PCB_LATEST) {
    reply(sKO);
    return;
  }
  reply(sOK);
}

}
}
}
}
