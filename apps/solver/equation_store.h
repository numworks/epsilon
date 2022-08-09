#ifndef SOLVER_EQUATION_STORE_H
#define SOLVER_EQUATION_STORE_H

#include "equation.h"
#include <apps/shared/expression_model_store.h>
#include <poincare/symbol_abstract.h>
#include <stdint.h>

namespace Solver {

class EquationStore : public Shared::ExpressionModelStore {
public:
  enum class Type {
    LinearSystem,
    PolynomialMonovariable,
    Monovariable,
  };
  enum class Error : int16_t {
    NoError = 0,
    EquationUndefined = -1,
    EquationNonreal = -2,
    TooManyVariables = -3,
    NonLinearSystem = -4,
    RequireApproximateSolution = -5,
  };
  EquationStore();

  /* ExpressionModelStore */
  Shared::ExpiringPointer<Equation> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<Equation>(static_cast<Equation *>(privateModelForRecord(record))); }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;

  /* EquationStore */
  Type type() const { return m_type; }
  int degree() const { return m_degree; }
  const char * variableAtIndex(size_t i) {
    assert(i < Poincare::Expression::k_maxNumberOfVariables && m_variables[i][0] != 0);
    return m_variables[i];
  }
  const char * userVariableAtIndex(size_t i) {
    assert(i < Poincare::Expression::k_maxNumberOfVariables && m_userVariables[i][0] != 0);
    return m_userVariables[i];
  }
  int numberOfUserVariables() const { return m_numberOfUserVariables; }
  bool userVariablesUsed() const { return m_userVariablesUsed; }
  int numberOfSolutions() const { return m_numberOfSolutions; }

  /* Exact resolution */
  Error exactSolve(Poincare::Context * context, bool * replaceFunctionsButNotSymbols);
  /* The exact solutions are displayed in a table with 2 layouts: an exact
   * Layout and an approximate layout. For example, 'sqrt(2)' and '1.414213'.
   * The boolean exactLayout indicates if we want the exact layout or the
   * approximate one. */
  Poincare::Layout exactSolutionLayoutAtIndex(int i, bool exactLayout);
  /* Exact layout and approximate layout of an exact solution can be:
   * - identical: for instance, 5 and 5
   * - equal: for instance 1/2 and 0.5
   * - non-equal: for instance 1/3 and 0.333.
   */
  bool exactSolutionLayoutsAtIndexAreIdentical(int i) {
    assert(m_type != Type::Monovariable && i >= 0 && (i < m_numberOfSolutions || (i == m_numberOfSolutions && m_type == Type::PolynomialMonovariable)));
    return m_exactSolutionIdentity[i];
  }
  bool exactSolutionLayoutsAtIndexAreEqual(int i) {
    assert(m_type != Type::Monovariable && i >= 0 && (i < m_numberOfSolutions || (i == m_numberOfSolutions && m_type == Type::PolynomialMonovariable)));
    return m_exactSolutionEquality[i];
  }

  /* Approximate resolution */
  double intervalBound(int index) const;
  void setIntervalBound(int index, double value);
  double approximateSolutionAtIndex(int i) {
    assert(m_type == Type::Monovariable && i >= 0 && i < m_numberOfSolutions);
    return m_approximateSolutions[i];
  }
  void approximateSolve(Poincare::Context * context, bool shouldReplaceFuncionsButNotSymbols);
  bool haveMoreApproximationSolutions() { return m_hasMoreThanMaxNumberOfApproximateSolution; }

  void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) override;

  constexpr static int k_maxNumberOfExactSolutions = Poincare::Expression::k_maxNumberOfVariables > Poincare::Expression::k_maxPolynomialDegree + 1? Poincare::Expression::k_maxNumberOfVariables : Poincare::Expression::k_maxPolynomialDegree + 1;
  constexpr static int k_maxNumberOfApproximateSolutions = 10;
  bool m_hasMoreThanMaxNumberOfApproximateSolution;
  constexpr static int k_maxNumberOfSolutions = k_maxNumberOfExactSolutions > k_maxNumberOfApproximateSolutions ? k_maxNumberOfExactSolutions : k_maxNumberOfApproximateSolutions;
private:
  constexpr static double k_precision = 0.01;
  // Enable the same number of equations as the number of unknown variables
  constexpr static int k_maxNumberOfEquations = Poincare::Expression::k_maxNumberOfVariables;

  // ExpressionModelStore
  int maxNumberOfMemoizedModels() const override { return k_maxNumberOfEquations; }
  const char * modelExtension() const override { return Ion::Storage::eqExtension; }
  /* We don't really use model memoization as the number of Equation is limited
   * and we keep enough Equations to store them all. */
  Shared::ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;

  Error privateExactSolve(Poincare::Context * context, bool replaceFunctionsButNotSymbols);
  Error resolveLinearSystem(Poincare::Expression solutions[k_maxNumberOfExactSolutions], Poincare::Expression solutionApproximations[k_maxNumberOfExactSolutions], Poincare::Expression coefficients[k_maxNumberOfEquations][Poincare::Expression::k_maxNumberOfVariables], Poincare::Expression constants[k_maxNumberOfEquations], Poincare::Context * context);
  Error oneDimensionalPolynomialSolve(Poincare::Expression solutions[k_maxNumberOfExactSolutions], Poincare::Expression solutionApproximations[k_maxNumberOfExactSolutions], Poincare::Expression polynomialCoefficients[Poincare::Expression::k_maxNumberOfPolynomialCoefficients], Poincare::Context * context);
  void tidySolution(char * treePoolCursor);
  bool isExplicitlyComplex(Poincare::Context * context);
  Poincare::Preferences::ComplexFormat updatedComplexFormat(Poincare::Context * context);

  mutable Equation m_equations[k_maxNumberOfEquations];
  Poincare::Layout m_exactSolutionExactLayouts[k_maxNumberOfApproximateSolutions];
  Poincare::Layout m_exactSolutionApproximateLayouts[k_maxNumberOfExactSolutions];
  char m_variables[Poincare::Expression::k_maxNumberOfVariables][Poincare::SymbolAbstract::k_maxNameSize];
  char m_userVariables[Poincare::Expression::k_maxNumberOfVariables][Poincare::SymbolAbstract::k_maxNameSize];
  double m_intervalApproximateSolutions[2];
  double m_approximateSolutions[k_maxNumberOfApproximateSolutions];
  int m_degree;
  int m_numberOfSolutions;
  int m_numberOfUserVariables;
  Type m_type;
  bool m_userVariablesUsed;
  bool m_exactSolutionIdentity[k_maxNumberOfExactSolutions];
  bool m_exactSolutionEquality[k_maxNumberOfExactSolutions];
};

}

#endif
