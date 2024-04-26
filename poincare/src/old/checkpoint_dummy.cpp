#include <poincare/old/checkpoint.h>
#include <poincare/old/exception_checkpoint.h>

namespace Poincare {

Checkpoint* Checkpoint::s_topmost = nullptr;

bool ExceptionCheckpoint::setActive(bool interruption) { return false; }

void ExceptionCheckpoint::rollbackException() {}

void ExceptionCheckpoint::Raise() {}

}  // namespace Poincare
