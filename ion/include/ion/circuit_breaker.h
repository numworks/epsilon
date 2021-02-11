#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

namespace Ion {
namespace CircuitBreaker {

enum class Checkpoint {
  Home,
  Custom
};

bool hasCheckpoint(Checkpoint c);
void resetCheckpoint(Checkpoint c);
void loadCustomCheckpoint();
void loadHomeCheckpoint();
void setCustomCheckpoint();
void setHomeCheckpoint();

}
}

#endif
