#include <config/clocks.h>
#include <drivers/board.h>
#include <drivers/cache.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void setFrequencyWithoutSystick(Frequency f) {
  RCC::CFGR::AHBPrescaler prescaler = f == Frequency::Low ? Clocks::Config::AHBLowFrequencyPrescalerReg : RCC::CFGR::AHBPrescaler::SysClk;
  RCC.CFGR()->setHPRE(prescaler);
}

}
}
}
