#include "exception_checkpoint.h"

#include <stdlib.h>

namespace PoincareJ {

ExceptionCheckpoint* ExceptionCheckpoint::s_topmostExceptionCheckpoint;

ExceptionCheckpoint::ExceptionCheckpoint()
    : m_parent(s_topmostExceptionCheckpoint) {}

ExceptionCheckpoint::~ExceptionCheckpoint() {
  s_topmostExceptionCheckpoint = m_parent;
}

bool ExceptionCheckpoint::setActive(bool interruption) {
  if (!interruption) {
    s_topmostExceptionCheckpoint = this;
  }
  return !interruption;
}

void ExceptionCheckpoint::rollback() {
  Checkpoint::rollback();
  longjmp(m_jumpBuffer, 1);
}

void ExceptionCheckpoint::Raise() {
  if (s_topmostExceptionCheckpoint == nullptr) {
    abort();
  }
  s_topmostExceptionCheckpoint->rollback();
  abort();
}

}  // namespace PoincareJ

extern "C" {
void ExceptionCheckpointRaise() { PoincareJ::ExceptionCheckpoint::Raise(); }
}
