
#pragma once

#include <poincare/prepared_function.h>

#include "expression_solver.h"

namespace Poincare::NumericSolver {

/* IntersectionSolver is only usable if the Pool is available for now, because
 * it needs to memoize the difference of the two functions on the pool.
 * It could be made independent of the pool, but the difference would need to
 * be managed externally.
 */
class IntersectionSolver : public RootSolver {
 public:
  IntersectionSolver(double xStart, double xEnd, const PreparedFunction& f,
                     const PreparedFunction& g, bool areAlongSameAxis,
                     const SymbolContext& context);
  IntersectionSolver(double xStart, double xEnd, const PreparedFunction& f,
                     const PreparedFunction& g, const SymbolContext& context)
      : IntersectionSolver(xStart, xEnd, f, g, true, context) {}

  Solution<double> next() override;

 private:
  PreparedFunction m_f;
  PreparedFunction m_g;
  PreparedFunction m_diff;
  bool m_alongSameAxis;
};

}  // namespace Poincare::NumericSolver
