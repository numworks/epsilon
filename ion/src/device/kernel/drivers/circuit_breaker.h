#ifndef ION_DEVICE_KERNEL_CHECKPOINT_H
#define ION_DEVICE_KERNEL_CHECKPOINT_H

#include <stdint.h>
#include <ion/circuit_breaker.h>

namespace Ion {
namespace Device {
namespace CircuitBreaker {

enum class Checkpoint {
  Home,
  Custom
};

Ion::CircuitBreaker::Status status();
void storeCustomCheckpoint(Ion::CircuitBreaker::CheckpointBuffer buffer);
void resetCustomCheckpoint(Ion::CircuitBreaker::CheckpointBuffer * buffer);
bool hasCheckpoint(Checkpoint c);
bool setCheckpoint(Checkpoint c, bool overridePreviousCheckpoint);
void loadCheckpoint(Checkpoint c);

}
}
}

#endif
