#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

#include <stdint.h>

namespace Ion {
namespace CircuitBreaker {

bool busy();

// Custom checkpoint

constexpr int k_checkpointBufferSize = 340;
typedef uint8_t CheckpointBuffer[k_checkpointBufferSize];

bool clearCustomCheckpointFlag();
void storeCustomCheckpoint(CheckpointBuffer buffer);
void resetCustomCheckpoint(CheckpointBuffer buffer = nullptr);
void loadCustomCheckpoint();
void setCustomCheckpoint(bool overridePreviousCheckoint = false);

// Home checkpoint
bool clearHomeCheckpointFlag();
void setHomeCheckpoint();

}
}

#endif
