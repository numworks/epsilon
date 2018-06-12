#ifndef SOLVER_EQUATION_STORE_H
#define SOLVER_EQUATION_STORE_H

#include "equation.h"
#include "../shared/expression_model_store.h"
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
    TooManyVariables = -2,
    NonLinearSystem = -3,
    RequireApproximateSolution = -4
  };
  EquationStore();
  ~EquationStore();
  Equation * modelAtIndex(int i) override {
    assert(i>=0 && i<m_numberOfModels);
    return &m_equations[i];
  }
  Equation * definedModelAtIndex(int i) override { return static_cast<Equation *>(Shared::ExpressionModelStore::definedModelAtIndex(i)); }
  int maxNumberOfModels() const override { return k_maxNumberOfEquations; }
  Type type() const {
    return m_type;
  }
  char variableAtIndex(size_t i) {
    assert(i < strlen(m_variables));
    return m_variables[i];
  }
  int numberOfSolutions() const {
    return m_numberOfSolutions;
  }
  Poincare::ExpressionLayout * exactSolutionLayoutAtIndex(int i, bool exactLayout);
  bool exactSolutionLayoutsAtIndexAreIdentical(int i) {
    assert(m_type != Type::Monovariable && i >= 0 && (i < m_numberOfSolutions || (i == m_numberOfSolutions && m_type == Type::PolynomialMonovariable)));
    return m_exactSolutionIdentity[i];
  }
  bool exactSolutionLayoutsAtIndexAreEqual(int i) {
    assert(m_type != Type::Monovariable && i >= 0 && (i < m_numberOfSolutions || (i == m_numberOfSolutions && m_type == Type::PolynomialMonovariable)));
    return m_exactSolutionEquality[i];
  }
  double intervalBound(int index) const;
  void setIntervalBound(int index, double value);
  double approximateSolutionAtIndex(int i);
  void tidy() override;
  void approximateSolve(Poincare::Context * context);
  bool haveMoreApproximationSolutions(Poincare::Context * context);
  Error exactSolve(Poincare::Context * context);
  static constexpr int k_maxNumberOfExactSolutions = Poincare::Expression::k_maxNumberOfVariables > Poincare::Expression::k_maxPolynomialDegree + 1? Poincare::Expression::k_maxNumberOfVariables : Poincare::Expression::k_maxPolynomialDegree + 1;
  static constexpr int k_maxNumberOfApproximateSolutions = 10;
  static constexpr int k_maxNumberOfSolutions = k_maxNumberOfExactSolutions > k_maxNumberOfApproximateSolutions ? k_maxNumberOfExactSolutions : k_maxNumberOfApproximateSolutions;
private:
  static constexpr double k_precision = 0.01;
  static constexpr int k_maxNumberOfEquations = Poincare::Expression::k_maxNumberOfVariables; // Enable the same number of equations as the number of unknown variables
  Equation * emptyModel() override;
  Equation * nullModel() override {
    return emptyModel();
  }
  void setModelAtIndex(Shared::ExpressionModel * f, int i) override;
  Error resolveLinearSystem(Poincare::Expression * solutions[k_maxNumberOfExactSolutions], Poincare::Expression * coefficients[k_maxNumberOfEquations][Poincare::Expression::k_maxNumberOfVariables], Poincare::Expression * constants[k_maxNumberOfEquations], Poincare::Context * context);
  Error oneDimensialPolynomialSolve(Poincare::Expression * solutions[k_maxNumberOfExactSolutions], Poincare::Expression * polynomialCoefficients[Poincare::Expression::k_maxNumberOfPolynomialCoefficients], int degree, Poincare::Context * context);
  void tidySolution();

  Equation m_equations[k_maxNumberOfEquations];
  Type m_type;
  char m_variables[Poincare::Expression::k_maxNumberOfVariables+1];
  int m_numberOfSolutions;
  Poincare::ExpressionLayout * m_exactSolutionExactLayouts[k_maxNumberOfApproximateSolutions];
  Poincare::ExpressionLayout * m_exactSolutionApproximateLayouts[k_maxNumberOfExactSolutions];
  bool m_exactSolutionIdentity[k_maxNumberOfExactSolutions];
  bool m_exactSolutionEquality[k_maxNumberOfExactSolutions];
  double m_intervalApproximateSolutions[2];
  double m_approximateSolutions[k_maxNumberOfApproximateSolutions];
};

}

#endif
