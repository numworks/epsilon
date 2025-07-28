#ifndef ION_CIRCUIT_BREAKER_H
#define ION_CIRCUIT_BREAKER_H

#include <setjmp.h>
#include <stdint.h>

namespace Ion {
namespace CircuitBreaker {

enum class CheckpointType : uint8_t {
  // Sorted by order of priority
  Home = 0,    // Checkpoint where the code jumps on Home Event
  Back = 1,    // Checkpoint where the code jumps on Back Event if the device is
               // stalling
  AnyKey = 2,  // Checkpoint where the code jumps on any keyboard Event if the
               // device is stalling
  NumberOfCheckpoints
};

enum class Status : uint8_t {
  Busy = 0,         // Waiting after a pendSV
  Interrupted = 1,  // An interruption was encountered and the execution jumped
                    // to the checkpoint
  Set = 2,          // The checkpoint has just been set
  Ignored = 3       // The checkpoint was ignored because it's already set at a
                    // deeper checkpoint
};

Status status();
bool hasCheckpoint(CheckpointType type);
void unsetCheckpoint(CheckpointType type);
void loadCheckpoint(CheckpointType type);
void lock();
void unlock();

#if PLATFORM_DEVICE
Status setCheckpoint(CheckpointType type);
#else
#define setCheckpoint(type) \
  statusAfterSetjmp(setjmp(*jmpbufForType(type)), (type))
Status statusAfterSetjmp(int jmpStatus, CheckpointType type);
jmp_buf* jmpbufForType(CheckpointType type);
#endif

}  // namespace CircuitBreaker
}  // namespace Ion

#endif
