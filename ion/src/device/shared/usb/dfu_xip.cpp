#include "calculator.h"

namespace Ion {
namespace USB {

void DFU() {
  Ion::Device::USB::Calculator::PollAndReset(true);
}

}
}
