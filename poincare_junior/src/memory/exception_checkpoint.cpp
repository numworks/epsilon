#include "exception_checkpoint.h"

#include <stdlib.h>

namespace PoincareJ {

ExceptionCheckpoint* ExceptionCheckpoint::s_topmostExceptionCheckpoint;

ExceptionCheckpoint::~ExceptionCheckpoint() {
  assert(s_topmostExceptionCheckpoint == this ||
         s_topmostExceptionCheckpoint == m_parent);
  s_topmostExceptionCheckpoint = m_parent;
}

void ExceptionCheckpoint::rollback(ExceptionType type) {
  assert(type != ExceptionType::None);
  // Next Raise will be handled by parent.
  s_topmostExceptionCheckpoint = m_parent;
  Checkpoint::rollback();
  longjmp(m_jumpBuffer, static_cast<int>(type));
}

void ExceptionCheckpoint::Raise(ExceptionType type) {
  assert(type != ExceptionType::None);
  // Can't raise if there are no active ExceptionCheckpoints.
  if (s_topmostExceptionCheckpoint == nullptr) {
    abort();
  }
  s_topmostExceptionCheckpoint->rollback(type);
  abort();
}

}  // namespace PoincareJ

extern "C" {
void ExceptionCheckpointRaise() {
  PoincareJ::ExceptionCheckpoint::Raise(PoincareJ::ExceptionType::Other);
}
}
