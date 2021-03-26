#ifndef ION_DEVICE_KERNEL_DRIVERS_TRAMPOLINE_H
#define ION_DEVICE_KERNEL_DRIVERS_TRAMPOLINE_H

#include <stdint.h>
#include <shared/drivers/trampoline.h>

namespace Ion {
namespace Device {
namespace Trampoline {

uint32_t addressOfFunction(int index);

}
}
}

#endif


