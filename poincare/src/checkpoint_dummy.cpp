#include <poincare/checkpoint.h>
#include <poincare/exception_checkpoint.h>

namespace Poincare {

TreeNode * Checkpoint::TopmostEndOfPool() {
  return nullptr;
}

bool ExceptionCheckpoint::setActive(bool interruption) {
  return false;
}

void ExceptionCheckpoint::rollbackException() {
}

void ExceptionCheckpoint::Raise() {
}

}
