#include <ion/circuit_breaker.h>
#include <assert.h>

namespace Ion {
namespace CircuitBreaker {

Status sStatus = Status::Interrupted;
constexpr static int k_numberOfCheckpointTypes = 3;
bool sCheckpointsSet[k_numberOfCheckpointTypes] = {false, false, false};
jmp_buf sBuffers[k_numberOfCheckpointTypes];
jmp_buf sDummyBuffer;

Status status() {
  return sStatus;
}

bool hasCheckpoint(CheckpointType type) {
  return sCheckpointsSet[static_cast<uint8_t>(type)];
}

void unsetCheckpoint(CheckpointType type) {
  sCheckpointsSet[static_cast<uint8_t>(type)] = false;
}

void loadCheckpoint(CheckpointType type) {
  assert(hasCheckpoint(type));
  sStatus = Status::Interrupted;
  longjmp(sBuffers[static_cast<uint8_t>(type)], 1);
}

Status statusAfterSetjmp(int jmpStatus, CheckpointType type) {
  if (jmpStatus == 0) {
    if (hasCheckpoint(type)) {
      return Status::Ignored;
    }
    sStatus = Status::Set;
    sCheckpointsSet[static_cast<uint8_t>(type)] = true;
  }
  assert((sStatus == Status::Interrupted) == (jmpStatus != 0));
  return sStatus;
}

jmp_buf * jmpbufForType(CheckpointType type) {
  return hasCheckpoint(type) ? &sDummyBuffer : sBuffers + static_cast<uint8_t>(type);
}

}
}
