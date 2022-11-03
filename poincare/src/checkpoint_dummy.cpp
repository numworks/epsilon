#include <poincare/checkpoint.h>
#include <poincare/exception_checkpoint.h>

namespace Poincare {

TreeNode * Checkpoint::TopmostEndOfPoolBeforeCheckpoint() {
  return nullptr;
}

bool ExceptionCheckpoint::setActive(bool interruption) {
  return false;
}

void ExceptionCheckpoint::rollback() {
}

void ExceptionCheckpoint::Raise() {
}

}
