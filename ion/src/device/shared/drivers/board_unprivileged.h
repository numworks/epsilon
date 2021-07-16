#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_UNPRIVILEGED_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_UNPRIVILEGED_H

#include <drivers/kernel_header.h>
#include <drivers/userland_header.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t userlandStart();
KernelHeader * kernelHeader();
UserlandHeader * userlandHeader();

}
}
}

#endif

