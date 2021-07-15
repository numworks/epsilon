#ifndef ION_DEVICE_KERNEL_BOOT_RT0_H
#define ION_DEVICE_KERNEL_BOOT_RT0_H

#include <shared/boot/rt0.h>

namespace Ion {
namespace Device {
namespace Init {

void prologue();

}
}
}

void jump_to_main();

#endif

