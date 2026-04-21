#pragma once

#include <poincare/tree.h>

#include "policies/solver.h"

namespace Poincare::NumericSolver {

/* NOTE: TSolver is a CRTP descendant of PolicySolver
 * It must also define a function parentNext(). */
template <typename TSolver>
class RootSolverFormalHelper {
 protected:
  struct DetailedRoot {
    double x;
    const Internal::Tree* nullDescendant = nullptr;
  };
  constexpr static DetailedRoot k_nanRoot = {.x = static_cast<double>(NAN),
                                             .nullDescendant = nullptr};

  static Solution<double> SolutionFromDetailedRoot(DetailedRoot root) {
    return std::isfinite(root.x) ? Solution<double>{root.x, 0., Interest::Root}
                                 : k_noSolution<double>;
  }

  DetailedRoot nextDetailedRoot();

 private:
  TSolver& as() { return *static_cast<TSolver*>(this); }
  const TSolver& as() const { return *static_cast<const TSolver*>(this); }

  const Internal::Tree* getTree() const {
    return static_cast<const Internal::Tree*>(as().m_evaluator.data());
  }

  DetailedRoot nextRootSearchingInChild(int childIndex) const;
  using ExpressionTestAuxiliary = bool (*)(const Internal::Tree*);
  DetailedRoot nextRootSearchingInChildren(ExpressionTestAuxiliary test) const;
  DetailedRoot nextRootInMultiplication() const;
  DetailedRoot nextRootInAddition() const;
  DetailedRoot nextRootInDependency() const;

  DetailedRoot registerDetailedRoot(DetailedRoot root) {
    as().registerSolution(SolutionFromDetailedRoot(root));
    return root;
  }
};

}  // namespace Poincare::NumericSolver
