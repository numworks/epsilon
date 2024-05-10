#include <poincare/old/exception_checkpoint.h>
#include <poincare/old/pool_checkpoint.h>

namespace Poincare {

PoolCheckpoint* PoolCheckpoint::s_topmost = nullptr;

bool ExceptionCheckpoint::setActive(bool interruption) { return false; }

void ExceptionCheckpoint::rollbackException() {}

void ExceptionCheckpoint::Raise() {}

}  // namespace Poincare
