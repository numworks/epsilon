#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

namespace Ion {
namespace CircuitBreaker {

bool hasCheckpoint();
void loadCheckpoint();
bool setCheckpoint();
void resetCheckpoint();

}
}

#endif
