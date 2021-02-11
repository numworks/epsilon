#ifndef ION_DEVICE_KERNEL_CHECKPOINT_H
#define ION_DEVICE_KERNEL_CHECKPOINT_H

#include <stdint.h>
#include <ion/circuit_breaker.h>

namespace Ion {
namespace Device {
namespace CircuitBreaker {

bool hasCheckpoint(Ion::CircuitBreaker::Checkpoint c);
void setCheckpoint(Ion::CircuitBreaker::Checkpoint c, uint8_t * frameAddress, uint32_t excReturn);
void unsetCheckpoint(Ion::CircuitBreaker::Checkpoint c);
void loadCheckpoint(Ion::CircuitBreaker::Checkpoint c, uint8_t * frameAddress);

}
}
}

#endif
