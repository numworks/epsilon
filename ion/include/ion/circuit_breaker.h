#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

namespace Ion {
namespace CircuitBreaker {

// Custom checkpoint
bool clearCustomCheckpointFlag();
void resetCustomCheckpoint();
void loadCustomCheckpoint();
void setCustomCheckpoint();

// Home checkpoint
bool clearHomeCheckpointFlag();
void setHomeCheckpoint();

}
}

#endif
