#ifndef ION_DEVICE_SHARED_RESET_H
#define ION_DEVICE_SHARED_RESET_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Reset {

void core();
void coreWhilePlugged();

/* jump takes as parameter the address of the isr vector to jump to: the data at
 * jumpIsrVectorAddress must the stack_pointer address, and the data at
 * jumpIsrVectorAddress + 1 must be the reset method address. */
void jump(uint32_t jumpIsrVectorAddress);

}
}
}

#endif
