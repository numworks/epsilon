#pragma once

#include <poincare/symbol_context.h>
#include <poincare/tree.h>

#include "solver.h"

/* TODO_PCJ: Change signatures to preparedFunctions instead of Trees. */
namespace Poincare::NumericSolver {

class ExpressionSolver : public Solver<double> {
 public:
  ExpressionSolver(double xStart, double xEnd, const Internal::Tree* e,
                   const SymbolContext& context,
                   Solver<double>::BracketTest test,
                   Solver<double>::HoneResult hone);

  // Specialized builders for common use cases
  static ExpressionSolver BuildMinimumSolver(double xStart, double xEnd,
                                             const Internal::Tree* e,
                                             const SymbolContext& context);
  static ExpressionSolver BuildMaximumSolver(double xStart, double xEnd,
                                             const Internal::Tree* e,
                                             const SymbolContext& context);

 private:
  const SymbolContext& m_context;
};

}  // namespace Poincare::NumericSolver
