#ifndef SOLVER_SYSTEM_H
#define SOLVER_SYSTEM_H

#include <poincare/symbol_abstract.h>

#include "equation.h"
#include "equation_store.h"
#include "solution.h"

namespace Solver {

class System {
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

  System(EquationStore* store) : m_store(store) {}

  constexpr static int k_maxNumberOfExactSolutions =
      std::max(Poincare::Expression::k_maxNumberOfVariables,
               Poincare::Expression::k_maxPolynomialDegree + 1);
  constexpr static int k_maxNumberOfApproximateSolutions = 10;
  constexpr static int k_maxNumberOfSolutions =
      std::max(k_maxNumberOfExactSolutions, k_maxNumberOfApproximateSolutions);

  // System analysis
  Type type() const { return m_type; }
  int degree() const { return m_degree; }
  const char* variable(int index) const {
    assert(index < m_numberOfResolutionVariables);
    return m_variables[index];
  }
  size_t numberOfUserVariables() const { return m_numberOfUserVariables; }
  const char* userVariable(int index) const {
    assert(index < m_numberOfUserVariables);
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
  const Solution* solution(int index) const {
    assert(index < m_numberOfSolutions);
    return m_solutions + index;
  }
  bool hasMoreSolutions() const { return m_hasMoreSolutions; }

  void tidy(char* treePoolCursor = nullptr);

 private:
  constexpr static char k_parameterPrefix = 't';

  Error privateExactSolve(Poincare::Context* context);
  Error simplifyAndFindVariables(Poincare::Context* context,
                                 Poincare::Expression* simplifiedEquations);
  Error solveLinearSystem(Poincare::Context* context,
                          Poincare::Expression* simplifiedEquations);
  Error solvePolynomial(Poincare::Context* context,
                        Poincare::Expression* simplifiedEquations);

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
