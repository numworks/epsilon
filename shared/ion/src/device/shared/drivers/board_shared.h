#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_SHARED_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_SHARED_H

#include <shared/drivers/kernel_header.h>
#include <shared/drivers/userland_header.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

enum class Slot : bool {
  A,
  B,
};

KernelHeader* kernelHeader();
UserlandHeader* userlandHeader();
UserlandHeader* userlandHeader(Slot slot);
uint32_t userlandStart();
uint32_t userlandStart(Slot slot);
uint32_t userlandEnd(Slot slot);
uint32_t writableSRAMEndAddress();
uint32_t writableSRAMStartAddress();
bool isRunningSlotA();
uint32_t securityLevel();

}  // namespace Board
}  // namespace Device
}  // namespace Ion

#endif
