#include "reset.h"

namespace Ion {
namespace Device {
namespace Reset {

void jump(uint32_t jumpIsrVectorAddress) {
  uint32_t *stackPointerAddress =
      reinterpret_cast<uint32_t *>(jumpIsrVectorAddress);
  uint32_t *resetHandlerAddress = stackPointerAddress + 1;

  asm volatile(
      "msr PSP, %[stackPointer] ; bx %[resetHandler]"
      :
      : [stackPointer] "r"(*stackPointerAddress), [resetHandler] "r"(
                                                      *resetHandlerAddress));
}

}  // namespace Reset
}  // namespace Device
}  // namespace Ion
