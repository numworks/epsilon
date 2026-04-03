#pragma once

#include <omg/vector.h>
#include <poincare/numeric_solver/solution.h>

namespace Poincare::NumericSolver {

template <typename TFloat, size_t N>
class StaticSolutionQueue : public OMG::StaticVector<Solution<TFloat>, N> {
 public:
  Solution<TFloat> popFirst() {
    assert(this->size() > 0);
    Solution<TFloat> s = (*this)[0];
    this->removeAt(0);
    return s;
  }
};

}  // namespace Poincare::NumericSolver
