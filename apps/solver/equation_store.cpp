#include "equation_store.h"
#include <limits.h>

using namespace Poincare;

namespace Solver {

EquationStore::EquationStore() :
  m_type(Type::LinearSystem),
  m_numberOfSolutions(0),
  m_exactSolutionExactLayouts{},
  m_exactSolutionApproximateLayouts{}
{
}

EquationStore::~EquationStore() {
  tidySolution();
}

Equation * EquationStore::emptyModel() {
  static Equation e;
  return &e;
}

void EquationStore::setModelAtIndex(Shared::ExpressionModel * e, int i) {
  m_equations[i] = *(static_cast<Equation *>(e));;
}

void EquationStore::tidy() {
  ExpressionModelStore::tidy();
  tidySolution();
}

Poincare::ExpressionLayout * EquationStore::exactSolutionLayoutAtIndex(int i, bool exactLayout) {
  assert(m_type != Type::Monovariable && i >= 0 && (i < m_numberOfSolutions || (i == m_numberOfSolutions && m_type == Type::PolynomialMonovariable)));
  if (exactLayout) {
    return m_exactSolutionExactLayouts[i];
  } else {
    return m_exactSolutionApproximateLayouts[i];
  }
}

double EquationStore::approximateSolutionAtIndex(int i) {
  assert(m_type == Type::Monovariable && i >= 0 && i < m_numberOfSolutions);
  return m_approximateSolutions[i];
}

EquationStore::Error EquationStore::exactSolve(Poincare::Context * context) {
  tidySolution();
  m_variables[0] = 0;
  int numberOfVariables = 0;
  for (int i = 0; i < numberOfModels(); i++) {
    numberOfVariables = m_equations[i].standardForm(context)->getVariables(m_variables);
    if (numberOfVariables < 0) {
      return Error::TooManyVariables;
    }
  }

  // 1-- Linear System
  /* Create matrix coefficients and vector constants as:
   * coefficients*(x y z ...) = constants */
  Expression * coefficients[k_maxNumberOfEquations][Expression::k_maxNumberOfVariables];
  Expression * constants[k_maxNumberOfEquations];
  bool success = true;
  for (int i = 0; i < numberOfModels(); i++) {
    success = success && m_equations[i].standardForm(context)->getLinearCoefficients(m_variables, coefficients[i], &constants[i], *context);
    if (!success) {
      for (int j = 0; j < i; j++) {
        for (int k = 0; k < numberOfVariables; k++) {
          delete coefficients[j][k];
        }
        delete constants[j];
      }
      if (numberOfModels() > 1 || numberOfVariables > 1) {
        return Error::NonLinearSystem;
      } else {
        break;
      }
    }
  }
  Expression * exactSolutions[k_maxNumberOfExactSolutions];
  for (int i = 0; i < k_maxNumberOfExactSolutions; i++) {
   exactSolutions[i] = nullptr;
  }
  EquationStore::Error error;
  if (success) {
    m_type = Type::LinearSystem;
    error = resolveLinearSystem(exactSolutions, coefficients, constants, context);
  } else {
    assert(numberOfVariables == 1 && numberOfModels() == 1);
    char x = m_variables[0];
    Expression * polynomialCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
    int degree = m_equations[0].standardForm(context)->getPolynomialCoefficients(x, polynomialCoefficients, *context);
    if (degree < 0) {
      m_type = Type::Monovariable;
      return Error::RequireApproximateSolution;
    } else {
      m_type = Type::PolynomialMonovariable;
      error = oneDimensialPolynomialSolve(exactSolutions, polynomialCoefficients, degree, context);
    }
  }
  for (int i = 0; i < k_maxNumberOfExactSolutions; i++) {
    if (exactSolutions[i]) {
      m_exactSolutionExactLayouts[i] = exactSolutions[i]->createLayout();
      Expression * approximate = exactSolutions[i]->approximate<double>(*context);
      m_exactSolutionApproximateLayouts[i] = approximate->createLayout();
      delete approximate;
      delete exactSolutions[i];
    }
  }
  return error;
}

EquationStore::Error EquationStore::resolveLinearSystem(Expression * exactSolutions[k_maxNumberOfExactSolutions], Expression * coefficients[k_maxNumberOfEquations][Expression::k_maxNumberOfVariables], Expression * constants[k_maxNumberOfEquations], Context * context) {
  Expression::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  int n = strlen(m_variables); // n unknown variables
  int m = numberOfModels(); // m equations
  const Expression ** operandsAb = new const Expression * [(n+1)*m];
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      operandsAb[i*(n+1)+j] = coefficients[i][j];
    }
    operandsAb[i*(n+1)+n] = constants[i];
  }
  Matrix * Ab = new Matrix(operandsAb, m, n+1, false);
  delete [] operandsAb;
  int rankAb = Ab->rank(*context, angleUnit, true);

  // Infinite number of solutions
  m_numberOfSolutions = INT_MAX;
  // Inconsistency?
  for (int j = m-1; j >= 0; j--) {
    bool rowWithNullCoefficients = true;
    for (int i = 0; i < n; i++) {
      if (!Ab->matrixOperand(j, i)->isRationalZero()) {
        rowWithNullCoefficients = false;
        break;
      }
    }
    if (rowWithNullCoefficients && !Ab->matrixOperand(j, n)->isRationalZero()) {
      m_numberOfSolutions = 0;
    }
  }
  if (m_numberOfSolutions > 0) {
    if (rankAb == n && n > 0) {
      m_numberOfSolutions = n;
      for (int i = 0; i < m_numberOfSolutions; i++) {
        Expression * sol = Ab->matrixOperand(i,n);
        exactSolutions[i] = sol;
        Ab->detachOperand(sol);
        Expression::Simplify(&exactSolutions[i], *context);
      }
    }
  }
  delete Ab;
  return Error::NoError;
}

EquationStore::Error EquationStore::oneDimensialPolynomialSolve(Expression * exactSolutions[k_maxNumberOfExactSolutions], Expression * coefficients[Expression::k_maxNumberOfPolynomialCoefficients], int degree, Context * context) {
  assert(degree == 2);
  Expression * deltaDenominator[3] = {new Rational(4), coefficients[0]->clone(), coefficients[2]->clone()};
  Expression * delta = new Subtraction(new Power(coefficients[1]->clone(), new Rational(2), false), new Multiplication(deltaDenominator, 3, false), false);
  Expression::Simplify(&delta, *context);
  if (delta->isRationalZero()) {
    exactSolutions[0] = new Division(new Opposite(coefficients[1], false), new Multiplication(new Rational(2), coefficients[2]), false);
    m_numberOfSolutions = 1;
  } else {
    exactSolutions[0] = new Division(new Subtraction(new Opposite(coefficients[1]->clone(), false), new SquareRoot(delta->clone(), false), false), new Multiplication(new Rational(2), coefficients[2]->clone()), false);
    exactSolutions[1] = new Division(new Addition(new Opposite(coefficients[1], false), new SquareRoot(delta->clone(), false), false), new Multiplication(new Rational(2), coefficients[2]), false);
    m_numberOfSolutions = 2;
  }
  exactSolutions[m_numberOfSolutions] = delta;
  delete coefficients[0];
  for (int i = 0; i < m_numberOfSolutions; i++) {
    Expression::Simplify(&exactSolutions[i], *context);
  }
  return Error::NoError;
#if 0
  if (degree == 3) {
    Expression * a = coefficients[3];
    Expression * b = coefficients[2];
    Expression * c = coefficients[1];
    Expression * d = coefficients[0];
    // Delta = b^2*c^2+18abcd-27a^2*d^2-4ac^3-4db^3
    Expression * mult0Operands[2] = {new Power(b->clone(), new Rational(2), false), new Power(c->clone(), new Rational(2), false)};
    Expression * mult1Operands[5] = {new Rational(18), a->clone(), b->clone(), c->clone(), d->clone()};
    Expression * mult2Operands[3] = {new Rational(-27), new Power(a->clone(), new Rational(2), false), new Power(d->clone(), new Rational(2), false)};
    Expression * mult3Operands[3] = {new Rational(-4), a->clone(), new Power(c->clone(), new Rational(3), false)};
    Expression * mult4Operands[3] = {new Rational(-4), d->clone(), new Power(b->clone(), new Rational(3), false)};
    Expression * add0Operands[5] = {new Multiplication(mult0Operands, 2, false), new Multiplication(mult1Operands, 5, false), new Multiplication(mult2Operands, 3, false), new Multiplication(mult3Operands, 3, false), new Multiplication(mult4Operands, 3, false)};
    Expression * delta = new Addition(add0Operands, 5, false);
    Simplify(&delta, *context);
    // Delta0 = b^2-3ac
    Expression * mult5Operands[3] = {new Rational(3), a->clone(), c->clone()};
    Expression * delta0 = new Subtraction(new Power(b->clone(), new Rational(2), false), new Multiplication(mult5Operands, 3, false), false);
    Reduce(&delta0, *context);
    if (delta->isRationalZero()) {
      if (delta0->isRationalZero()) {
        // delta0 = 0 && delta = 0 --> x0 = -b/(3a)
        delete delta0;
        m_exactSolutions[0] = new Opposite(new Division(b, new Multiplication(new Rational(3), a, false), false), false);
        m_numberOfSolutions = 1;
        delete c;
        delete d;
      } else {
        // delta = 0 --> x0 = (9ad-bc)/(2delta0)
        //           --> x1 = (4abc-9a^2d-b^3)/(a*delta0)
        Expression * mult6Operands[3] = {new Rational(9), a, d};
        m_exactSolutions[0] = new Division(new Subtraction(new Multiplication(mult6Operands, 3, false), new Multiplication(b, c, false), false), new Multiplication(new Rational(2), delta0, false), false);
        Expression * mult7Operands[4] = {new Rational(4), a->clone(), b->clone(), c->clone()};
        Expression * mult8Operands[3] = {new Rational(-9), new Power(a->clone(), new Rational(2), false), d->clone()};
        Expression * add1Operands[3] = {new Multiplication(mult7Operands, 4, false), new Multiplication(mult8Operands,3, false), new Opposite(new Power(b->clone(), new Rational(3), false), false)};
        m_exactSolutions[1] = new Division(new Addition(add1Operands, 3, false), new Multiplication(a->clone(), delta0, false), false);
        m_numberOfSolutions = 2;
      }
    } else {
      // delta1 = 2b^3-9abc+27a^2*d
      Expression * mult9Operands[4] = {new Rational(-9), a, b, c};
      Expression * mult10Operands[3] = {new Rational(27), new Power(a->clone(), new Rational(2), false), d};
      Expression * add2Operands[3] = {new Multiplication(new Rational(2), new Power(b->clone(), new Rational(3), false), false), new Multiplication(mult9Operands, 4, false), new Multiplication(mult10Operands, 3, false)};
      Expression * delta1 = new Addition(add2Operands, 3, false);
      // C = Root((delta1+sqrt(-27a^2*delta))/2, 3)
      Expression * mult11Operands[3] = {new Rational(-27), new Power(a->clone(), new Rational(2), false), (*delta)->clone()};
      Expression * c = new Power(new Division(new Addition(delta1, new SquareRoot(new Multiplication(mult11Operands, 3, false), false), false), new Rational(2), false), new Rational(1,3), false);
      Expression * unary3roots[2] = {new Addition(new Rational(-1,2), new Division(new Multiplication(new SquareRoot(new Rational(3), false), new Symbol(Ion::Charset::IComplex), false), new Rational(2), false), false), new Subtraction(new Rational(-1,2), new Division(new Multiplication(new SquareRoot(new Rational(3), false), new Symbol(Ion::Charset::IComplex), false), new Rational(2), false), false)};
      // x_k = -1/(3a)*(b+C*z+delta0/(zC)) with z = unary cube root
      for (int k = 0; k < 3; k++) {
        Expression * ccopy = c;
        Expression * delta0copy = delta0;
        if (k < 2) {
          ccopy = new Multiplication(c->clone(), unary3roots[k], false);
          delta0copy = delta0->clone();
        }
        Expression * add3Operands[3] = {b->clone(), ccopy, new Division(delta0copy, ccopy->clone(), false)};
        m_exactSolutions[k] = new Multiplication(new Division(new Rational(-1), new Multiplication(new Rational(3), a->clone(), false), false), new Addition(add3Operands, 3, false), false);
      }
      m_numberOfSolutions = 3;
    }
    m_exactSolutions[m_numberOfSolutions] = delta;
  }
#endif
}

void EquationStore::tidySolution() {
  for (int i = 0; i < k_maxNumberOfExactSolutions; i++) {
    if (m_exactSolutionExactLayouts[i]) {
      delete m_exactSolutionExactLayouts[i];
      m_exactSolutionExactLayouts[i] = nullptr;
    }
    if (m_exactSolutionApproximateLayouts[i]) {
      delete m_exactSolutionApproximateLayouts[i];
      m_exactSolutionApproximateLayouts[i] = nullptr;
    }
  }
}

}
