#ifndef SOLVER_SYSTEM_OF_EQUATIONS_H
#define SOLVER_SYSTEM_OF_EQUATIONS_H

#include <apps/shared/interactive_curve_view_range.h>
#include <poincare/equation_solver/equation_solver_pool.h>
#include <poincare/old/context_with_parent.h>
#include <poincare/range.h>

#include "equation_store.h"
#include "solution.h"

namespace Solver {

/* SystemOfEquations provides an interface to solve the system described by
 * EquationStore. The two main methods are:
 * - exactSolve, which identifies and computes exact solutions of linear
 *   systems, and polynomial equations of degree 2 or 3.
 * - approximateSolve, which computes numerical solutions for one equation of
 *   one variable, using an implementation of Brent's algorithm.
 *
 * FIXME Preliminary analysis of the system (e.g. identifiying variables...) is
 * only done when calling exactSolve. This works well for now as Solver will
 * only call approximateSolve after first attempting to exactSolve, but might be
 * a problem later. */

class SystemOfEquations {
 public:
  using SolvingMethod = Poincare::EquationSolver::SolvingMethod;
  using SolutionType = Poincare::EquationSolver::SolutionType;

  using Error = Poincare::EquationSolver::Error;

  SystemOfEquations(EquationStore* store) : m_store(store) {}

  constexpr static int k_maxNumberOfVariables =
      Poincare::EquationSolver::k_maxNumberOfVariables;
  constexpr static int k_maxNumberOfExactSolutions =
      Poincare::EquationSolver::k_maxNumberOfExactSolutions;
  constexpr static int k_maxNumberOfApproximateSolutions = 10;
  constexpr static int k_maxNumberOfSolutions =
      std::max(k_maxNumberOfExactSolutions, k_maxNumberOfApproximateSolutions);

  // System analysis
  SolvingMethod solvingMethod() const {
    return m_solutionMetadata.solvingMethod;
  }
  int degree() const { return m_solutionMetadata.degree; }
  bool incompleteSolutions() const {
    return m_solutionMetadata.incompleteSolutions;
  }

  const char* unknownVariable(size_t index) const {
    return m_equationMetadata.unknownVariables[index];
  }
  size_t numberOfDefinedVariables() const {
    return m_equationMetadata.definedVariables.size();
  }
  const char* definedVariable(size_t index) const {
    return m_equationMetadata.definedVariables[index];
  }
  bool overrideDefinedVariables() const {
    return m_equationMetadata.overrideDefinedVariables;
  }

  // Approximate range
  Poincare::Range1D<double> approximateSolvingRange() const {
    return m_approximateSolvingRange;
  }
  void setApproximateSolvingRange(
      Poincare::Range1D<double> approximateSolvingRange);

  void resetSolvingRanges() {
    m_approximateSolvingRange = k_fallbackRange;
    m_memoizedAutoSolvingRange = Poincare::Range1D<double>();
  }

  bool isUsingAutoSolvingRange() const { return m_isUsingAutoSolvingRange; }
  void useAutoSolvingRange(bool useAuto = true) {
    m_isUsingAutoSolvingRange = useAuto;
  }
  Poincare::Range1D<double> memoizedAutoSolvingRange() const {
    return m_memoizedAutoSolvingRange;
  }

  // Solving methods
  Error exactSolve(Poincare::Context* context);
  void approximateSolve(Poincare::Context* context);

  /* Cancel intermediate results of setApproximateSolvingRange and
   * approximateSolve */
  void cancelApproximateSolve();

  // Solutions getters
  size_t numberOfSolutions() const { return m_numberOfSolutions; }
  const Solution* solution(size_t index) const {
    assert(index < m_numberOfSolutions);
    return m_solutions + index;
  }
  SolutionType solutionType() const { return m_solutionMetadata.solutionType; }

  bool wasInterrupted() const { return m_wasInterrupted; }
  void tidy(Poincare::PoolObject* treePoolCursor = nullptr);

 private:
  constexpr static char k_parameterPrefix = 't';
  constexpr static Poincare::Range1D<double> k_fallbackRange =
      Poincare::Range1D<double>(-10.0, 10.0);

  class ContextWithoutT : public Poincare::ContextWithParent {
   public:
    using Poincare::ContextWithParent::ContextWithParent;

   private:
    const Poincare::Internal::Tree* expressionForUserNamed(
        const Poincare::Internal::Tree* symbol) override;
  };

  Error registerExactSolution(Poincare::UserExpression exact,
                              Poincare::UserExpression approximate,
                              Poincare::Context* context);

  Solution m_solutions[k_maxNumberOfSolutions];
  Poincare::EquationSolver::EquationMetadata m_equationMetadata;
  Poincare::EquationSolver::SolutionMetadata m_solutionMetadata;
  size_t m_numberOfSolutions;
  EquationStore* m_store;
  Poincare::Range1D<double> m_approximateSolvingRange;
  Poincare::Range1D<double> m_memoizedAutoSolvingRange;
  bool m_isUsingAutoSolvingRange;
  bool m_wasInterrupted;
};

}  // namespace Solver

#endif
