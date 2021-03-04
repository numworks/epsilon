#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

#include <stdint.h>

namespace Ion {
namespace CircuitBreaker {

enum class Status {
  Busy, // Waiting after a pendSV
  Interrupted, // An interruption was encountered and the execution jumped to the checkpoint
  Set, // The checkpoint has just been set
  Ignored // The checkpoint was ignored because it's already set at a deeper checkpoint
};

Status status();

// Custom checkpoint

constexpr int k_checkpointBufferSize = 340;
typedef uint8_t CheckpointBuffer[k_checkpointBufferSize];

void storeCustomCheckpoint(CheckpointBuffer buffer);
void resetCustomCheckpoint(CheckpointBuffer * buffer = nullptr);
void loadCustomCheckpoint();
Status setCustomCheckpoint(bool overridePreviousCheckpoint = false);

// Home checkpoint
Status setHomeCheckpoint();

}
}

#endif
