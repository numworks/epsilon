#include "calculator.h"

namespace Ion {
namespace USB {

void DFU(bool exitWithKeyboard, bool unlocked, int level) {
  Ion::Device::USB::Calculator::PollAndReset(exitWithKeyboard, unlocked, level);
}

}
}
