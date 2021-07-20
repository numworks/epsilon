#include <shared/drivers/board_unprivileged.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initFPU() {
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  CORTEX.CPACR()->setAccess(10, CORTEX::CPACR::Access::Full);
  CORTEX.CPACR()->setAccess(11, CORTEX::CPACR::Access::Full);
  // FIXME: The pipeline should be flushed at this point
}

void setDefaultGPIO() {
  // Put all inputs as Analog Input, No pull-up nor pull-down
  // Except for the SWD port (PB3, PA13, PA14)
  GPIOA.MODER()->set(0xEBFFFFFF);
  GPIOA.PUPDR()->set(0x24000000);
  GPIOB.MODER()->set(0xFFFFFFBF);
  GPIOB.PUPDR()->set(0x00000000);
  for (int g=2; g<5; g++) {
    GPIO(g).MODER()->set(0xFFFFFFFF); // All to "Analog"
    GPIO(g).PUPDR()->set(0x00000000); // All to "None"
  }
}

void initCompensationCell() {
  /* The output speed of some GPIO pins is set to high, in which case,
   * the compensation cell should be enabled. */
  SYSCFG.CMPCR()->setCMP_PD(true);
  while (!SYSCFG.CMPCR()->getREADY()) {
  }
}

void shutdownCompensationCell() {
  SYSCFG.CMPCR()->setCMP_PD(false);
}

}
}
}
