#include <ion.h>
#include "calculator.h"

namespace Ion {
namespace USB {

void DFU(bool exitWithKeyboard, void * data) {
  Ion::updateSlotInfo();
  Ion::Device::USB::Calculator::PollAndReset(exitWithKeyboard, data);
}

}
}
