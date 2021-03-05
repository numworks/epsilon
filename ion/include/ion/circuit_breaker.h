#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

#include <stdint.h>

namespace Ion {
namespace CircuitBreaker {

enum class CheckpointType {
  Home, // Checkpoint where the code jumps on Home Event
  User, // Checkpoint where the code jumps on Back Event if the device is stalling
  System // Checkpoint used by system to try different paths at run-time
};

enum class Status {
  Busy, // Waiting after a pendSV
  Interrupted, // An interruption was encountered and the execution jumped to the checkpoint
  Set, // The checkpoint has just been set
  Ignored // The checkpoint was ignored because it's already set at a deeper checkpoint
};

Status status();
bool hasCheckpoint(CheckpointType type);
void unsetCheckpoint(CheckpointType type);
void loadCheckpoint(CheckpointType type);
Status setCheckpoint(CheckpointType type);

}
}

#endif
