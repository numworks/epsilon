#ifndef SOLVER_SYSTEM_OF_EQUATIONS_H
#define SOLVER_SYSTEM_OF_EQUATIONS_H

#include <apps/shared/interactive_curve_view_range.h>
#include <poincare/old/context_with_parent.h>
#include <poincare/old/symbol_abstract.h>
#include <poincare/range.h>
#include <poincare/src/expression/equation_solver.h>

#include "equation.h"
#include "equation_store.h"
#include "solution.h"

namespace Solver {

/* SystemOfEquations provides an interface to solve the system described by
 * EquationStore. The two main methods are:
 * - exactSolve, which identify and compute exact solutions of linear systems,
 *   and polynomial equations of degree 2 or 3.
 * - approximateSolve, which computes numerical solutions for one equation of
 *   one variable, using an implementation of Brent's algorithm.
 *
 * FIXME Preliminary analysis of the system (e.g. identifiying variables...) is
 * only done when calling exactSolve. This works well for now as Solver will
 * only call approximateSolve after first attempting to exactSolve, but might be
 * a problem later. */

class SystemOfEquations {
 public:
  using Type = Poincare::Internal::EquationSolver::Type;

  using Error = Poincare::Internal::EquationSolver::Error;

  SystemOfEquations(EquationStore* store) : m_store(store) {}

  constexpr static int k_maxNumberOfExactSolutions =
      std::max(Poincare::Expression::k_maxNumberOfVariables,
               Poincare::Expression::k_maxPolynomialDegree + 1);
  constexpr static int k_maxNumberOfApproximateSolutions = 10;
  constexpr static int k_maxNumberOfSolutions =
      std::max(k_maxNumberOfExactSolutions, k_maxNumberOfApproximateSolutions);

  // System analysis
  Type type() const { return m_type; }
  int degree() const { return m_degree; }
  const char* variable(size_t index) const {
    // Variable is now displayed in the equation solution
    assert(index < m_numberOfSolvingVariables &&
           m_solverContext.variables[index][0] != '\0');
    return m_solverContext.variables[index];
  }
  size_t numberOfUserVariables() const { return m_numberOfUserVariables; }
  const char* userVariable(size_t index) const {
    assert(index < m_numberOfUserVariables &&
           m_userVariables[index][0] != '\0');
    return m_userVariables[index];
  }
  bool overrideUserVariables() const { return m_overrideUserVariables; }

  // Approximate range
  Poincare::Range1D<double> approximateSolvingRange() const {
    return m_approximateSolvingRange;
  }
  bool autoApproximateSolvingRange() const {
    return m_autoApproximateSolvingRange;
  }
  void setApproximateSolvingRange(
      Poincare::Range1D<double> approximateSolvingRange);
  void autoComputeApproximateSolvingRange(Poincare::Context* context);

  // Solving methods
  Error exactSolve(Poincare::Context* context);
  void approximateSolve(Poincare::Context* context);

  // Solutions getters
  size_t numberOfSolutions() const { return m_numberOfSolutions; }
  const Solution* solution(size_t index) const {
    assert(index < m_numberOfSolutions);
    return m_solutions + index;
  }
  bool hasMoreSolutions() const { return m_hasMoreSolutions; }

  void tidy(Poincare::PoolObject* treePoolCursor = nullptr);

 private:
  constexpr static char k_parameterPrefix = 't';

  class ContextWithoutT : public Poincare::ContextWithParent {
   public:
    using Poincare::ContextWithParent::ContextWithParent;

   private:
    const Poincare::UserExpression protectedExpressionForSymbolAbstract(
        const Poincare::SymbolAbstract& symbol, bool clone,
        Poincare::ContextWithParent* lastDescendantContext) override;
  };

  Poincare::SystemExpression equationStandardFormForApproximateSolve(
      Poincare::Context* context);
  Error privateExactSolve(Poincare::Context* context);
  Error simplifyAndFindVariables(
      Poincare::Context* context,
      Poincare::SystemExpression* simplifiedEquations);
  Error solveLinearSystem(Poincare::Context* context,
                          Poincare::SystemExpression* simplifiedEquations);
  Error solvePolynomial(Poincare::Context* context,
                        Poincare::SystemExpression* simplifiedEquations);
  uint32_t tagParametersUsedAsVariables() const;
  void tagVariableIfParameter(const char* name, uint32_t* tags) const;

  enum class SolutionType : uint8_t {
    Exact,
    Approximate,
    Formal,
  };
  Error registerSolution(Poincare::UserExpression e, Poincare::Context* context,
                         SolutionType type);
  void registerSolution(double f);

  Solution m_solutions[k_maxNumberOfSolutions];
  size_t m_numberOfSolvingVariables;
  size_t m_numberOfUserVariables;
  size_t m_numberOfSolutions;
  EquationStore* m_store;
  int m_degree;
  Poincare::Range1D<double> m_approximateSolvingRange;
  char m_variables[Poincare::Expression::k_maxNumberOfVariables]
                  [Poincare::SymbolAbstractNode::k_maxNameSize];
  char m_userVariables[Poincare::Expression::k_maxNumberOfVariables]
                      [Poincare::SymbolAbstractNode::k_maxNameSize];
  Type m_type;
  Poincare::Preferences::ComplexFormat m_complexFormat;
  bool m_overrideUserVariables;
  bool m_hasMoreSolutions;
  bool m_autoApproximateSolvingRange;
  Poincare::Internal::EquationSolver::Context m_solverContext;
};

}  // namespace Solver

#endif
