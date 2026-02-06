#include <poincare/numeric_solver_jr/expression_solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/numeric_solver/solver_tests.h>

using namespace Poincare::Internal;

namespace Poincare::NumericSolver {

template <typename T>
ExpressionSolver<T>::ExpressionSolver(T xStart, T xEnd, const Internal::Tree* e,
                                      const SymbolContext& context,
                                      Solver<T>::BracketTest test,
                                      Solver<T>::HoneResult hone)
    : Solver<T>(xStart, xEnd, nullptr, e, test, hone,
                SolverTests<T>::IsDiscontinuousInBracket) {
  if (e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isRandomized(); })) {
    this->m_evaluator = [](T, const void*) { return static_cast<T>(NAN); };
    return;
  }

  /* TODO_PCJ: Either ensure expression is projected, or pass approximation
   * context(ComplexFormat, AngleUnit) and replace defined symbols. */
  this->m_evaluator = [](T x, const void* aux) {
    const Internal::Tree* e = reinterpret_cast<const Internal::Tree*>(aux);
    return Approximation::To<T>(e, x,
                                Approximation::Parameters{.optimize = true});
  };
}

}  // namespace Poincare::NumericSolver
