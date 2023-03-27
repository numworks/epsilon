#ifndef SOLVER_SYSTEM_OF_EQUATIONS_H
#define SOLVER_SYSTEM_OF_EQUATIONS_H

#include <poincare/symbol_abstract.h>

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
  enum class Type : uint8_t {
    LinearSystem,
    PolynomialMonovariable,
    GeneralMonovariable,
  };

  enum class Error : uint8_t {
    NoError = 0,
    EquationUndefined = 1,
    EquationNonreal = 2,
    TooManyVariables = 3,
    NonLinearSystem = 4,
    RequireApproximateSolution = 5,
  };

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
    assert(index < m_numberOfResolutionVariables &&
           m_variables[index][0] != '\0');
    return m_variables[index];
  }
  size_t numberOfUserVariables() const { return m_numberOfUserVariables; }
  const char* userVariable(size_t index) const {
    assert(index < m_numberOfUserVariables &&
           m_userVariables[index][0] != '\0');
    return m_userVariables[index];
  }
  bool overrideUserVariables() const { return m_overrideUserVariables; }

  // Resolution methods
  double approximateResolutionMinimum() const {
    return m_approximateResolutionMinimum;
  }
  double approximateResolutionMaximum() const {
    return m_approximateResolutionMaximum;
  }
  void setApproximateResolutionMinimum(double value) {
    m_approximateResolutionMinimum = value;
  }
  void setApproximateResolutionMaximum(double value) {
    m_approximateResolutionMaximum = value;
  }
  Error exactSolve(Poincare::Context* context);
  void approximateSolve(Poincare::Context* context);

  // Solutions getters
  size_t numberOfSolutions() const { return m_numberOfSolutions; }
  const Solution* solution(size_t index) const {
    assert(index < m_numberOfSolutions);
    return m_solutions + index;
  }
  bool hasMoreSolutions() const { return m_hasMoreSolutions; }

  void tidy(char* treePoolCursor = nullptr);

 private:
  constexpr static char k_parameterPrefix = 't';
  constexpr static double k_defaultApproximateSearchRange = 10.;

  Error privateExactSolve(Poincare::Context* context);
  Error simplifyAndFindVariables(Poincare::Context* context,
                                 Poincare::Expression* simplifiedEquations);
  Error solveLinearSystem(Poincare::Context* context,
                          Poincare::Expression* simplifiedEquations);
  Error solvePolynomial(Poincare::Context* context,
                        Poincare::Expression* simplifiedEquations);
  uint32_t tagParametersUsedAsVariables() const;
  void tagVariableIfParameter(const char* name, uint32_t* tags) const;

  enum class SolutionType : uint8_t {
    Exact,
    Approximate,
    Formal,
  };
  Error registerSolution(Poincare::Expression e, Poincare::Context* context,
                         SolutionType type);
  void registerSolution(double f);

  Solution m_solutions[k_maxNumberOfSolutions];
  double m_approximateResolutionMinimum;
  double m_approximateResolutionMaximum;
  size_t m_numberOfResolutionVariables;
  size_t m_numberOfUserVariables;
  size_t m_numberOfSolutions;
  EquationStore* m_store;
  int m_degree;
  char m_variables[Poincare::Expression::k_maxNumberOfVariables]
                  [Poincare::SymbolAbstractNode::k_maxNameSize];
  char m_userVariables[Poincare::Expression::k_maxNumberOfVariables]
                      [Poincare::SymbolAbstractNode::k_maxNameSize];
  Type m_type;
  Poincare::Preferences::ComplexFormat m_complexFormat;
  bool m_overrideUserVariables;
  bool m_hasMoreSolutions;
};

}  // namespace Solver

#endif
