#include <regs/cortex.h>

namespace Ion {
namespace Reset {

void core() {
  /* We don't want to use a SVC on n0100 to Reset::core because the DFU driver
   * needs it and the svc_handler lives in the internal flash which can be
   * corrupted (being reflashed) when the DFU emits the SVC. We can't write the
   * CORTEX register to generate a core reset from the userland or DFU code
   * since this register can be accessed by privileged mode only. We use the
   * following hack: aborting will reset the device. */
  Ion::Device::Regs::CORTEX.AIRCR()->requestReset();
  while (true) {
    asm("nop");
  }
}

}
}
