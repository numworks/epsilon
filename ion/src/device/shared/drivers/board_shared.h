#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_SHARED_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_SHARED_H

#include <shared/drivers/kernel_header.h>
#include <shared/drivers/userland_header.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader();
UserlandHeader * userlandHeader();
uint32_t userlandStart();
uint32_t userlandEnd();
uint32_t writableSRAMEndAddress();
uint32_t writableSRAMStartAddress();
bool isRunningSlotA();

}
}
}

#endif
