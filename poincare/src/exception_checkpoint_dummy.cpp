#include <poincare/exception_checkpoint.h>

namespace Poincare {


bool ExceptionCheckpoint::setActive(bool interruption) {
  return false;
}

void ExceptionCheckpoint::rollback() {
}

void ExceptionCheckpoint::Raise() {
}

}
