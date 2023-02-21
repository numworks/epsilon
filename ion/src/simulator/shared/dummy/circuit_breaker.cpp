#include <assert.h>
#include <ion/circuit_breaker.h>

namespace Ion {
namespace CircuitBreaker {

Status status() { return Status::Set; }

bool hasCheckpoint(CheckpointType) { return false; }

void unsetCheckpoint(CheckpointType) {}

void loadCheckpoint(CheckpointType) {}

void lock() {}

void unlock() {}

Status statusAfterSetjmp(int, CheckpointType) { return Status::Ignored; }

jmp_buf sDummy;
jmp_buf* jmpbufForType(CheckpointType) { return &sDummy; }

}  // namespace CircuitBreaker
}  // namespace Ion
