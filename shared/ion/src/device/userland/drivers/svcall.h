#ifndef ION_DEVICE_USERLAND_DRIVERS_SVCALL_H
#define ION_DEVICE_USERLAND_DRIVERS_SVCALL_H

#include <shared/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace SVCall {

#define SVC_ATTRIBUTES __attribute__((noinline, externally_visible))

}  // namespace SVCall
}  // namespace Device
}  // namespace Ion

#endif
