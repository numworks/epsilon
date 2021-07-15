#include "command.h"
#include <drivers/board.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void WritePCBVersion(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }

  /* When running the bench for a diagnostic, we must absolutely not write the
   * OTP, as N0110 built prior to the PCB revision would still have their OTP
   * blank and unlocked. */
#if IN_FACTORY
  Board::writePCBVersion(PCB_LATEST);
  /* Read directly from memory, as when IN_FACTORY is true, the method
   * pcbVersion always returns PCB_LATEST. */
  if (Board::readPCBVersionInMemory() != PCB_LATEST) {
    reply(sKO);
    return;
  }
  Board::lockPCBVersion();
#endif
  reply(sOK);
}

}
}
}
}
