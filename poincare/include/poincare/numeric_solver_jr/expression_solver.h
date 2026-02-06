#pragma once

#include <poincare/symbol_context.h>
#include <poincare/tree.h>

#include "solver.h"

/* TODO_PCJ: Change signatures to preparedFunctions instead of Trees. */
namespace Poincare::NumericSolver {

template <typename T>
class ExpressionSolver : public Solver<T> {
 public:
  ExpressionSolver(T xStart, T xEnd, const Internal::Tree* e,
                   const SymbolContext& context, Solver<T>::BracketTest test,
                   Solver<T>::HoneResult hone);

  // Specialized builders for common use cases
  static ExpressionSolver BuildMinimumSolver(T xStart, T xEnd,
                                             const Internal::Tree* e,
                                             const SymbolContext& context);
  static ExpressionSolver BuildMaximumSolver(T xStart, T xEnd,
                                             const Internal::Tree* e,
                                             const SymbolContext& context);

 private:
  const SymbolContext& m_context;
};

}  // namespace Poincare::NumericSolver
