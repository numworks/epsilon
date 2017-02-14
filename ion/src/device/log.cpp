#include <ion/log.h>
#include "regs/itm.h"

// We're printing using SWO.
// This is achieved by writing to the ITM register, which is sent through the
// Cortex Debug bus

void Ion::Log::print(const char * message) {
  char character = 0;
  while ((character = *message++) != 0) {
    if (ITM.TER()->get(0)) {
      ITM.STIM(0)->set(character);
    }
  }
}
