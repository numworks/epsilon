#ifndef ION_DEVICE_KERNEL_CHECKPOINT_H
#define ION_DEVICE_KERNEL_CHECKPOINT_H

#include <stdint.h>
#include <ion/circuit_breaker.h>

namespace Ion {
namespace Device {
namespace CircuitBreaker {

void initInterruptions();
void shutdownInterruptions();
void init();
void shutdown();
bool hasCheckpoint(Ion::CircuitBreaker::CheckpointType type);
void loadCheckpoint(Ion::CircuitBreaker::CheckpointType type);
bool setCheckpoint(Ion::CircuitBreaker::CheckpointType type);
Ion::CircuitBreaker::Status status();
void unsetCheckpoint(Ion::CircuitBreaker::CheckpointType type);
void lock();
void unlock(bool runLockedCheckpoint = true);
void forceUnlock();

}
}
}

#endif
