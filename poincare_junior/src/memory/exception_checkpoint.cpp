#include "exception_checkpoint.h"

namespace PoincareJ {

ExceptionCheckpoint * ExceptionCheckpoint::s_topmostExceptionCheckpoint;

ExceptionCheckpoint::ExceptionCheckpoint() :
  m_parent(s_topmostExceptionCheckpoint)
{
}

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
  assert(s_topmostExceptionCheckpoint != nullptr);
  s_topmostExceptionCheckpoint->rollback();
  assert(false);
}

}

