#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

namespace Ion {
namespace CircuitBreaker {

bool hasCheckpoint();
void loadCheckpoint();
void setCheckpoint();
void resetCheckpoint();

}
}

#endif
