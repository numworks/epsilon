#include "reset.h"
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Reset {

void SVC_ATTRIBUTES core() {
  SVC_RETURNING_VOID(SVC_RESET_CORE)
}

}
}

namespace Ion {
namespace Device {
namespace Reset {

void jump(uint32_t jumpIsrVectorAddress) {
  uint32_t * stackPointerAddress = reinterpret_cast<uint32_t *>(jumpIsrVectorAddress);
  uint32_t * resetHandlerAddress = stackPointerAddress + 1;

  asm volatile (
      "msr PSP, %[stackPointer] ; bx %[resetHandler]"
      : :
      [stackPointer] "r" (*stackPointerAddress),
      [resetHandler] "r" (*resetHandlerAddress)
      );

}

}
}
}
