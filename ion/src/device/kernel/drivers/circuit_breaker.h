#ifndef ION_DEVICE_KERNEL_CHECKPOINT_H
#define ION_DEVICE_KERNEL_CHECKPOINT_H

#include <stdint.h>
#include <ion/circuit_breaker.h>

namespace Ion {
namespace Device {
namespace CircuitBreaker {

bool hasCheckpoint(Ion::CircuitBreaker::CheckpointType type);
void loadCheckpoint(Ion::CircuitBreaker::CheckpointType type);
bool setCheckpoint(Ion::CircuitBreaker::CheckpointType type);
Ion::CircuitBreaker::Status status();
void unsetCheckpoint(Ion::CircuitBreaker::CheckpointType type);

}
}
}

#endif
