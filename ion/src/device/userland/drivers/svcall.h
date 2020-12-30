#ifndef ION_DEVICE_USERLAND_DRIVERS_SVCALL_H
#define ION_DEVICE_USERLAND_DRIVERS_SVCALL_H

#include <shared/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace SVCall {

#define svc(code) asm volatile ("svc %[immediate]"::[immediate] "I" (code))

void svcall(unsigned int svcNumber, int * argcPointer = nullptr, void * argv[] = nullptr);

void setSvcallArgs(int argc, void * argv[]);

}
}
}

#endif
