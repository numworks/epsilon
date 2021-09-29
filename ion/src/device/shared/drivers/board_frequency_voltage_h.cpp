#include <config/clocks.h>
#include <drivers/board.h>
#include <drivers/cache.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

#warning Frequency calibration: Cf comment in include/N0120/config/clocks.h

void setLowVoltageScaling() {
#warning This line doesn\'t seem to be effective
  /*setFrequencyWithoutSystick(Frequency::Low);
  // Select HSI as sysclk source
  RCC.CFGR()->setSW(RCC::CFGR::SW::HSI);
  while(RCC.CFGR()->getSWS() != RCC::CFGR::SW::HSI) {}
  // Disable PLL
  RCC.CR()->setPLL1ON(false);
  RCC.CR()->setPLL2ON(false);
  while (RCC.CR()->getPLL1RDY() || RCC.CR()->getPLL2RDY()) {}
  // Decrease VOS
  PWR.D3CR()->setVOS(PWR::D3CR::VOS::Scale3);
  while (!PWR.D3CR()->getVOSRDY()) {}*/
}

void setHighVoltageScaling() {
  /*// Increase VOS
  PWR.D3CR()->setVOS(PWR::D3CR::VOS::Scale0);
  // Ensure VOS activation before changing the frequency
  while (!PWR.D3CR()->getVOSRDY()) {}
  // Enable PLL
  RCC.CR()->setPLL1ON(true);
  RCC.CR()->setPLL2ON(true);
  while (!RCC.CR()->getPLL1RDY() || !RCC.CR()->getPLL2RDY()) {}
  // Select PLL as sysclk source
  RCC.CFGR()->setSW(RCC::CFGR::SW::PLL1);
  while(RCC.CFGR()->getSWS() != RCC::CFGR::SW::PLL1) {}
  setFrequencyWithoutSystick(Frequency::High);*/
}

void setFrequencyWithoutSystick(Frequency f) {
  //RCC::Prescaler4Bits prescaler = f == Frequency::Low ? RCC::Prescaler4Bits::DivideBy512 : RCC::Prescaler4Bits::NoDivide;
  //RCC.D1CFGR()->setD1CPRE(prescaler);
}

}
}
}
