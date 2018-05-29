#ifndef SOLVER_EQUATION_STORE_H
#define SOLVER_EQUATION_STORE_H

#include "equation.h"
#include "../shared/expression_model_store.h"
#include <stdint.h>

namespace Solver {

class EquationStore : public Shared::ExpressionModelStore {
public:
  enum class Form {
    LinearSystem,
    PolynomialMonovariable,
    Monovariable,
  };
  enum class Error : int16_t {
    NoError = 0,
    TooManyVariables = -1,
    NonLinearSystem = -2,
    RequireApproximateSolution = -3
  };
  EquationStore();
  ~EquationStore();
  Equation * modelAtIndex(int i) override {
    assert(i>=0 && i<m_numberOfModels);
    return &m_equations[i];
  }
  int maxNumberOfModels() const override { return k_maxNumberOfEquations; }
  void tidy() override;
  Error exactSolve(Poincare::Context * context);
private:
  static constexpr int k_maxNumberOfEquations = Poincare::Expression::k_maxNumberOfVariables; // Enable the same number of equations as the number of unknown variables
  static constexpr int k_maxNumberOfExactSolutions = Poincare::Expression::k_maxNumberOfVariables > Poincare::Expression::k_maxPolynomialDegree ? Poincare::Expression::k_maxNumberOfVariables : Poincare::Expression::k_maxPolynomialDegree;
  static constexpr int k_maxNumberOfApproximateSolutions = 10;
  Equation * emptyModel() override;
  Equation * nullModel() override {
    return emptyModel();
  }
  void setModelAtIndex(Shared::ExpressionModel * f, int i) override;
  Error resolveLinearSystem(Poincare::Expression * coefficients[k_maxNumberOfEquations][Poincare::Expression::k_maxNumberOfVariables], Poincare::Expression * constants[k_maxNumberOfEquations], Poincare::Context * context);
  Error oneDimensialPolynomialSolve(Poincare::Expression * polynomialCoefficients[Poincare::Expression::k_maxNumberOfPolynomialCoefficients], int degree, Poincare::Context * context);
  void tidySolution();

  Equation m_equations[k_maxNumberOfEquations];
  Form m_form;
  int m_numberOfSolutions;
  Poincare::Expression * m_exactSolutions[k_maxNumberOfExactSolutions];
  double m_approximateSolutions[k_maxNumberOfApproximateSolutions];
};

}

#endif
