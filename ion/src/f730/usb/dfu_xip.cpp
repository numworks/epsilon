#include "calculator.h"
#include "../device.h"

namespace Ion {
namespace USB {

void DFU() {
  Ion::USB::Device::Calculator::PollAndReset(true);
}

}
}
