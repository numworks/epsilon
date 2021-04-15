#ifndef ION_DEVICE_USERLAND_DRIVERS_SVCALL_H
#define ION_DEVICE_USERLAND_DRIVERS_SVCALL_H

#include <shared/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace SVCall {

//#define SVC_ATTRIBUTES __attribute__((optimize("-O0"),noinline,externally_visible))
#define SVC_ATTRIBUTES __attribute__((noinline,externally_visible))

}
}
}

#endif
