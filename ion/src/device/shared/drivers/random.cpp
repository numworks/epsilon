#include <shared/drivers/random.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {

using namespace Device::Regs;

uint32_t random() {
  bool initialRNGEngineState = RCC.AHB2ENR()->getRNGEN();
  RCC.AHB2ENR()->setRNGEN(true);

  RNG.CR()->setRNGEN(true);
  while (RNG.SR()->getDRDY() == 0) {
  }
  RNG.CR()->setRNGEN(false);
  uint32_t result = RNG.DR()->get();

  RCC.AHB2ENR()->setRNGEN(initialRNGEngineState);

  return result;
}

}
}
