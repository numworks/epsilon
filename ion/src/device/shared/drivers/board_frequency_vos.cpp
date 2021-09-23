#include <config/clocks.h>
#include <drivers/board.h>
#include <drivers/cache.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

static void setLowVoltageScaling() {
  // Select HSI as sysclk source
  RCC.CFGR()->setSW(RCC::CFGR::SW::HSI);
  while(RCC.CFGR()->getSWS() != RCC::CFGR::SW::HSI) {}
  // Disable PLL
  RCC.CR()->setPLL1ON(false);
  RCC.CR()->setPLL2ON(false);
  while (RCC.CR()->getPLL1RDY() || RCC.CR()->getPLL2RDY()) {}
  // Decrease VOS
  PWR.D3CR()->setVOS(PWR::D3CR::VOS::Scale3);
  while (!PWR.D3CR()->getVOSRDY()) {}
}

static void setHighVoltageScaling() {
  // Increase VOS
  PWR.D3CR()->setVOS(PWR::D3CR::VOS::Scale0);
  // Ensure VOS activation before changing the frequency
  while (!PWR.D3CR()->getVOSRDY()) {}
  // Enable PLL
  RCC.CR()->setPLL1ON(true);
  RCC.CR()->setPLL2ON(true);
  while (!RCC.CR()->getPLL1RDY() || !RCC.CR()->getPLL2RDY()) {}
  /* Select PLL as sysclk source */
  RCC.CFGR()->setSW(RCC::CFGR::SW::PLL1);
  while(RCC.CFGR()->getSWS() != RCC::CFGR::SW::PLL1) {}
}

void setFrequencyWithoutSystick(Frequency f) {
  if (f == Frequency::Low) {
    setLowVoltageScaling();
  } else {
    assert(f == Frequency::High);
    setHighVoltageScaling();
  }
}

}
}
}
