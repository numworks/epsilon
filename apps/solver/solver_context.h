#ifndef SOLVER_CONTEXT_H
#define SOLVER_CONTEXT_H

#include <poincare/context_with_parent.h>

namespace Solver {

class SolverContext : public Poincare::ContextWithParent {
 public:
  SolverContext(Context* parentContext)
      : Poincare::ContextWithParent(parentContext) {}
  bool canRemoveUnderscoreToUnits() const override { return false; }
};

}  // namespace Solver
#endif