#include <ion.h>
#include <regs/regs.h>

namespace Ion {

using namespace Device::Regs;

uint32_t random() {
  bool initialRNGEngineState = RCC.AHB2ENR()->getRNGEN();
  RCC.AHB2ENR()->setRNGEN(true);

  RNG.CR()->setRNGEN(true);

  while (RNG.SR()->getDRDY() == 0) {
  }
  uint32_t result = RNG.DR()->get();

  RNG.CR()->setRNGEN(false);
  RCC.AHB2ENR()->setRNGEN(initialRNGEngineState);

  return result;
}

}
