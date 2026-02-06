#include <poincare/numeric_solver_jr/expression_solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/numeric_solver/solver_tests.h>
#include <poincare/src/numeric_solver_jr/hone_functions.h>

using namespace Poincare::Internal;

namespace Poincare::NumericSolver {

ExpressionSolver::ExpressionSolver(double xStart, double xEnd,
                                   const Internal::Tree* e,
                                   const SymbolContext& context,
                                   Solver<double>::BracketTest test,
                                   Solver<double>::HoneResult hone)
    : Solver<double>(xStart, xEnd, nullptr, e, test, hone,
                     SolverTests<double>::IsDiscontinuousInBracket),
      m_context(context) {
  if (e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isRandomized(); })) {
    this->m_evaluator = [](double, const void*) {
      return static_cast<double>(NAN);
    };
    return;
  }

  /* TODO_PCJ: Either ensure expression is projected, or pass approximation
   * context(ComplexFormat, AngleUnit) and replace defined symbols. */
  this->m_evaluator = [](double x, const void* aux) {
    const Internal::Tree* e = reinterpret_cast<const Internal::Tree*>(aux);
    return Approximation::To<double>(
        e, x, Approximation::Parameters{.optimize = true});
  };
}

ExpressionSolver ExpressionSolver::BuildMinimumSolver(
    double xStart, double xEnd, const Internal::Tree* e,
    const SymbolContext& context) {
  return ExpressionSolver(xStart, xEnd, e, context,
                          SolverTests<double>::MinimumInBracket,
                          HoneFunctions::SafeBrentMinimum);
}

ExpressionSolver ExpressionSolver::BuildMaximumSolver(
    double xStart, double xEnd, const Internal::Tree* e,
    const SymbolContext& context) {
  return ExpressionSolver(xStart, xEnd, e, context,
                          SolverTests<double>::MaximumInBracket,
                          HoneFunctions::SafeBrentMaximum);
}

}  // namespace Poincare::NumericSolver
