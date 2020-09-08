#include "calculator.h"

namespace Ion {
namespace USB {

void DFU(bool exitWithKeyboard) {
  Ion::Device::USB::Calculator::PollAndReset(exitWithKeyboard);
}

}
}
