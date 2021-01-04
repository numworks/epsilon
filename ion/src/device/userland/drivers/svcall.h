#ifndef ION_DEVICE_USERLAND_DRIVERS_SVCALL_H
#define ION_DEVICE_USERLAND_DRIVERS_SVCALL_H

#include <shared/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace SVCall {

#define SVC(code) asm volatile ("svc %[immediate]"::[immediate] "I" (code))
#define SVC_ATTRIBUTES __attribute__((noinline,optimize("O0")))

}
}
}

#endif
