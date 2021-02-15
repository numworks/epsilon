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

void unsetCustomCheckpoint();
bool hasCheckpoint(Checkpoint c);
bool clearCheckpointFlag(Checkpoint c);
void setCheckpoint(Checkpoint c);
void loadCheckpoint(Checkpoint c);

}
}
}

#endif
