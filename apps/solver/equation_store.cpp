#include "equation_store.h"
#include "../shared/poincare_helpers.h"
#include <limits.h>

#include <poincare/constant.h>
#include <poincare/symbol.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
#include <poincare/addition.h>
#include <poincare/subtraction.h>
#include <poincare/multiplication.h>
#include <poincare/division.h>
#include <poincare/square_root.h>
#include <poincare/power.h>
#include <poincare/undefined.h>

using namespace Poincare;
using namespace Shared;

namespace Solver {

EquationStore::EquationStore() :
  m_type(Type::LinearSystem),
  m_numberOfSolutions(0),
  m_exactSolutionExactLayouts{},
  m_exactSolutionApproximateLayouts{}
{
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

Poincare::Layout EquationStore::exactSolutionLayoutAtIndex(int i, bool exactLayout) {
  assert(m_type != Type::Monovariable && i >= 0 && (i < m_numberOfSolutions));
  if (exactLayout) {
    return m_exactSolutionExactLayouts[i];
  } else {
    return m_exactSolutionApproximateLayouts[i];
  }
}

double EquationStore::intervalBound(int index) const {
  assert(m_type == Type::Monovariable && index >= 0 && index < 2);
  return m_intervalApproximateSolutions[index];
}

void EquationStore::setIntervalBound(int index, double value) {
  assert(m_type == Type::Monovariable && index >= 0 && index < 2);
  m_intervalApproximateSolutions[index] = value;
  if (m_intervalApproximateSolutions[0] > m_intervalApproximateSolutions[1]) {
    if (index == 0) {
      m_intervalApproximateSolutions[1] = m_intervalApproximateSolutions[0]+1;
    } else {
      m_intervalApproximateSolutions[0] = m_intervalApproximateSolutions[1]-1;
    }
  }
}

double EquationStore::approximateSolutionAtIndex(int i) {
  assert(m_type == Type::Monovariable && i >= 0 && i < m_numberOfSolutions);
  return m_approximateSolutions[i];
}

bool EquationStore::haveMoreApproximationSolutions(Context * context) {
  if (m_numberOfSolutions < k_maxNumberOfEquations) {
    return false;
  }
  double step = (m_intervalApproximateSolutions[1]-m_intervalApproximateSolutions[0])*k_precision;
  return !std::isnan(PoincareHelpers::NextRoot(definedModelAtIndex(0)->standardForm(context), m_variables[0], m_approximateSolutions[m_numberOfSolutions-1], step, m_intervalApproximateSolutions[1], *context));
}

void EquationStore::approximateSolve(Poincare::Context * context) {
  assert(m_variables[0][0] != 0 && m_variables[1][0] == 0);
  assert(m_type == Type::Monovariable);
  m_numberOfSolutions = 0;
  double start = m_intervalApproximateSolutions[0];
  double step = (m_intervalApproximateSolutions[1]-m_intervalApproximateSolutions[0])*k_precision;
  for (int i = 0; i < k_maxNumberOfApproximateSolutions; i++) {
    m_approximateSolutions[i] = PoincareHelpers::NextRoot(definedModelAtIndex(0)->standardForm(context), m_variables[0], start, step, m_intervalApproximateSolutions[1], *context);
    if (std::isnan(m_approximateSolutions[i])) {
      break;
    } else {
      start = m_approximateSolutions[i];
      m_numberOfSolutions++;
    }
  }
}

EquationStore::Error EquationStore::exactSolve(Poincare::Context * context) {
  tidySolution();

  // Step 0. Get unknown variables
  m_variables[0][0] = 0;
  int numberOfVariables = 0;
  for (int i = 0; i < numberOfDefinedModels(); i++) {
    const Expression e = definedModelAtIndex(i)->standardForm(context);
    if (e.isUninitialized() || e.type() == ExpressionNode::Type::Undefined) {
      return Error::EquationUndefined;
    }
    if (e.type() == ExpressionNode::Type::Unreal) {
      return Error::EquationUnreal;
    }
    numberOfVariables = e.getVariables(*context, [](const char * symbol) { return true; }, (char *)m_variables, Poincare::SymbolAbstract::k_maxNameSize);
    if (numberOfVariables == -1) {
      return Error::TooManyVariables;
    }
    /* The equation has been parsed so there should be no
     * Error::VariableNameTooLong*/
    assert(numberOfVariables >= 0);
  }

  // Step 1. Linear System?

  /* Create matrix coefficients and vector constants as:
   *   coefficients * (x y z ...) = constants */
  Expression coefficients[k_maxNumberOfEquations][Expression::k_maxNumberOfVariables];
  Expression constants[k_maxNumberOfEquations];
  bool isLinear = true; // Invalid the linear system if one equation is non-linear
  Preferences * preferences = Preferences::sharedPreferences();
  for (int i = 0; i < numberOfDefinedModels(); i++) {
    isLinear = isLinear && definedModelAtIndex(i)->standardForm(context).getLinearCoefficients((char *)m_variables, Poincare::SymbolAbstract::k_maxNameSize, coefficients[i], &constants[i], *context,  updatedComplexFormat(), preferences->angleUnit());
    if (!isLinear) {
    // TODO: should we clean pool allocated memory if the system is not linear
#if 0
      for (int j = 0; j < i; j++) {
        for (int k = 0; k < numberOfVariables; k++) {
          coefficients[j][k] = Expression();
        }
        constants[j] = Expression();
      }
#endif
      if (numberOfDefinedModels() > 1 || numberOfVariables > 1) {
        return Error::NonLinearSystem;
      } else {
        break;
      }
    }
  }

  // Initialize result
  Expression exactSolutions[k_maxNumberOfExactSolutions];
  Expression exactSolutionsApproximations[k_maxNumberOfExactSolutions];
  EquationStore::Error error;

  if (isLinear) {
    m_type = Type::LinearSystem;
    error = resolveLinearSystem(exactSolutions, exactSolutionsApproximations, coefficients, constants, context);
  } else {
    // Step 2. Polynomial & Monovariable?
    assert(numberOfVariables == 1 && numberOfDefinedModels() == 1);
    Expression polynomialCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
    int degree = definedModelAtIndex(0)->standardForm(context).getPolynomialReducedCoefficients(m_variables[0], polynomialCoefficients, *context, updatedComplexFormat(), preferences->angleUnit());
    if (degree == 2) {
      // Polynomial degree <= 2
      m_type = Type::PolynomialMonovariable;
      error = oneDimensialPolynomialSolve(exactSolutions, exactSolutionsApproximations, polynomialCoefficients, degree, context);
    } else {
      // Step 3. Monovariable non-polynomial or polynomial with degree > 2
      m_type = Type::Monovariable;
      m_intervalApproximateSolutions[0] = -10;
      m_intervalApproximateSolutions[1] = 10;
      return Error::RequireApproximateSolution;
    }
  }
  // Create the results' layouts
  int solutionIndex = 0;
  int initialNumberOfSolutions = m_numberOfSolutions <= k_maxNumberOfExactSolutions ? m_numberOfSolutions : -1;
  // We iterate through the solutions and the potential delta
  for (int i = 0; i < initialNumberOfSolutions; i++) {
    if (!exactSolutions[i].isUninitialized()) {
      assert(!exactSolutionsApproximations[i].isUninitialized());
      if (exactSolutionsApproximations[i].type() == ExpressionNode::Type::Unreal) {
        // Discard unreal solutions.
        m_numberOfSolutions--;
        continue;
      }
      m_exactSolutionExactLayouts[solutionIndex] = PoincareHelpers::CreateLayout(exactSolutions[i]);
      m_exactSolutionApproximateLayouts[solutionIndex] = PoincareHelpers::CreateLayout(exactSolutionsApproximations[i]);
      // Check for identity between exact and approximate layouts
      char exactBuffer[Shared::ExpressionModel::k_expressionBufferSize];
      char approximateBuffer[Shared::ExpressionModel::k_expressionBufferSize];
      m_exactSolutionExactLayouts[solutionIndex].serializeForParsing(exactBuffer, Shared::ExpressionModel::k_expressionBufferSize);
      m_exactSolutionApproximateLayouts[solutionIndex].serializeForParsing(approximateBuffer, Shared::ExpressionModel::k_expressionBufferSize);
      m_exactSolutionIdentity[solutionIndex] = strcmp(exactBuffer, approximateBuffer) == 0;
      if (!m_exactSolutionIdentity[solutionIndex]) {
        char buffer[Shared::ExpressionModel::k_expressionBufferSize];
        m_exactSolutionEquality[solutionIndex] = exactSolutions[i].isEqualToItsApproximationLayout(exactSolutionsApproximations[i], buffer, Shared::ExpressionModel::k_expressionBufferSize, preferences->complexFormat(), preferences->angleUnit(), preferences->displayMode(), preferences->numberOfSignificantDigits(), *context);
      }
      solutionIndex++;
    }
  }
  return error;
}

EquationStore::Error EquationStore::resolveLinearSystem(Expression exactSolutions[k_maxNumberOfExactSolutions], Expression exactSolutionsApproximations[k_maxNumberOfExactSolutions], Expression coefficients[k_maxNumberOfEquations][Expression::k_maxNumberOfVariables], Expression constants[k_maxNumberOfEquations], Context * context) {
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  // n unknown variables
  int n = 0;
  while (m_variables[n][0] != 0) { n++; }
  int m = numberOfDefinedModels(); // m equations
  /* Create the matrix (A | b) for the equation Ax=b */
  Matrix Ab = Matrix::Builder();
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      Ab.addChildAtIndexInPlace(coefficients[i][j], Ab.numberOfChildren(), Ab.numberOfChildren());
    }
    Ab.addChildAtIndexInPlace(constants[i], Ab.numberOfChildren(), Ab.numberOfChildren());
  }
  Ab.setDimensions(m, n+1);

  // Compute the rank of (A | b)
  int rankAb = Ab.rank(*context, updatedComplexFormat(), angleUnit, true);

  // Initialize the number of solutions
  m_numberOfSolutions = INT_MAX;
  /* If the matrix has one null row except the last column, the system is
   * inconsistent (equivalent to 0 = x with x non-null */
  for (int j = m-1; j >= 0; j--) {
    bool rowWithNullCoefficients = true;
    for (int i = 0; i < n; i++) {
      if (!Ab.matrixChild(j, i).isRationalZero()) {
        rowWithNullCoefficients = false;
        break;
      }
    }
    if (rowWithNullCoefficients && !Ab.matrixChild(j, n).isRationalZero()) {
      m_numberOfSolutions = 0;
    }
  }
  if (m_numberOfSolutions > 0) {
    // if rank(A | b) < n, the system has an infinite number of solutions
    if (rankAb == n && n > 0) {
      // Otherwise, the system has n solutions which correspond to the last column
      m_numberOfSolutions = n;
      for (int i = 0; i < m_numberOfSolutions; i++) {
        exactSolutions[i] = Ab.matrixChild(i,n);
        exactSolutions[i].simplifyAndApproximate(&exactSolutions[i], &exactSolutionsApproximations[i], *context, updatedComplexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit());
      }
    }
  }
  return Error::NoError;
}

EquationStore::Error EquationStore::oneDimensialPolynomialSolve(Expression exactSolutions[k_maxNumberOfExactSolutions], Expression exactSolutionsApproximations[k_maxNumberOfExactSolutions], Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients], int degree, Context * context) {
  /* Equation ax^2+bx+c = 0 */
  assert(degree == 2);
  // Compute delta = b*b-4ac
  Expression delta = Subtraction::Builder(Power::Builder(coefficients[1].clone(), Rational(2)), Multiplication::Builder(Rational(4), coefficients[0].clone(), coefficients[2].clone()));
  delta = delta.simplify(*context, updatedComplexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit());
  if (delta.isUninitialized()) {
    delta = Poincare::Undefined();
  }
  if (delta.isRationalZero()) {
    // if delta = 0, x0=x1= -b/(2a)
    exactSolutions[0] = Division::Builder(Opposite::Builder(coefficients[1]), Multiplication::Builder(Rational(2), coefficients[2]));
    m_numberOfSolutions = 2;
  } else {
    // x0 = (-b-sqrt(delta))/(2a)
    exactSolutions[0] = Division::Builder(Subtraction::Builder(Opposite::Builder(coefficients[1].clone()), SquareRoot::Builder(delta.clone())), Multiplication::Builder(Rational(2), coefficients[2].clone()));
    // x1 = (-b+sqrt(delta))/(2a)
    exactSolutions[1] = Division::Builder(Addition::Builder(Opposite::Builder(coefficients[1]), SquareRoot::Builder(delta.clone())), Multiplication::Builder(Rational(2), coefficients[2]));
    m_numberOfSolutions = 3;
  }
  exactSolutions[m_numberOfSolutions-1] = delta;
  for (int i = 0; i < m_numberOfSolutions; i++) {
    exactSolutions[i].simplifyAndApproximate(&exactSolutions[i], &exactSolutionsApproximations[i], *context, updatedComplexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit());
  }
  return Error::NoError;
#if 0
  if (degree == 3) {
    Expression * a = coefficients[3];
    Expression * b = coefficients[2];
    Expression * c = coefficients[1];
    Expression * d = coefficients[0];
    // Delta = b^2*c^2+18abcd-27a^2*d^2-4ac^3-4db^3
    Expression * mult0Operands[2] = {new Power::Builder(b->clone(), new Rational(2), false), new Power::Builder(c->clone(), new Rational(2), false)};
    Expression * mult1Operands[5] = {new Rational(18), a->clone(), b->clone(), c->clone(), d->clone()};
    Expression * mult2Operands[3] = {new Rational(-27), new Power::Builder(a->clone(), new Rational(2), false), new Power::Builder(d->clone(), new Rational(2), false)};
    Expression * mult3Operands[3] = {new Rational(-4), a->clone(), new Power::Builder(c->clone(), new Rational(3), false)};
    Expression * mult4Operands[3] = {new Rational(-4), d->clone(), new Power::Builder(b->clone(), new Rational(3), false)};
    Expression * add0Operands[5] = {new Multiplication::Builder(mult0Operands, 2, false), new Multiplication::Builder(mult1Operands, 5, false), new Multiplication::Builder(mult2Operands, 3, false), new Multiplication::Builder(mult3Operands, 3, false), new Multiplication::Builder(mult4Operands, 3, false)};
    Expression * delta = new Addition(add0Operands, 5, false);
    PoincareHelpers::Simplify(&delta, *context);
    // Delta0 = b^2-3ac
    Expression * mult5Operands[3] = {new Rational(3), a->clone(), c->clone()};
    Expression * delta0 = new Subtraction::Builder(new Power::Builder(b->clone(), new Rational(2), false), new Multiplication::Builder(mult5Operands, 3, false), false);
    Reduce(&delta0, *context);
    if (delta->isRationalZero()) {
      if (delta0->isRationalZero()) {
        // delta0 = 0 && delta = 0 --> x0 = -b/(3a)
        delete delta0;
        m_exactSolutions[0] = new Opposite::Builder(new Division::Builder(b, new Multiplication::Builder(new Rational(3), a, false), false), false);
        m_numberOfSolutions = 1;
        delete c;
        delete d;
      } else {
        // delta = 0 --> x0 = (9ad-bc)/(2delta0)
        //           --> x1 = (4abc-9a^2d-b^3)/(a*delta0)
        Expression * mult6Operands[3] = {new Rational(9), a, d};
        m_exactSolutions[0] = new Division::Builder(new Subtraction::Builder(new Multiplication::Builder(mult6Operands, 3, false), new Multiplication::Builder(b, c, false), false), new Multiplication::Builder(new Rational(2), delta0, false), false);
        Expression * mult7Operands[4] = {new Rational(4), a->clone(), b->clone(), c->clone()};
        Expression * mult8Operands[3] = {new Rational(-9), new Power::Builder(a->clone(), new Rational(2), false), d->clone()};
        Expression * add1Operands[3] = {new Multiplication::Builder(mult7Operands, 4, false), new Multiplication::Builder(mult8Operands,3, false), new Opposite::Builder(new Power::Builder(b->clone(), new Rational(3), false), false)};
        m_exactSolutions[1] = new Division::Builder(new Addition(add1Operands, 3, false), new Multiplication::Builder(a->clone(), delta0, false), false);
        m_numberOfSolutions = 2;
      }
    } else {
      // delta1 = 2b^3-9abc+27a^2*d
      Expression * mult9Operands[4] = {new Rational(-9), a, b, c};
      Expression * mult10Operands[3] = {new Rational(27), new Power::Builder(a->clone(), new Rational(2), false), d};
      Expression * add2Operands[3] = {new Multiplication::Builder(new Rational(2), new Power::Builder(b->clone(), new Rational(3), false), false), new Multiplication::Builder(mult9Operands, 4, false), new Multiplication::Builder(mult10Operands, 3, false)};
      Expression * delta1 = new Addition(add2Operands, 3, false);
      // C = Root((delta1+sqrt(-27a^2*delta))/2, 3)
      Expression * mult11Operands[3] = {new Rational(-27), new Power::Builder(a->clone(), new Rational(2), false), (*delta)->clone()};
      Expression * c = new Power::Builder(new Division::Builder(new Addition(delta1, new SquareRoot(new Multiplication::Builder(mult11Operands, 3, false), false), false), new Rational(2), false), new Rational(1,3), false);
      Expression * unary3roots[2] = {new Addition(new Rational(-1,2), new Division::Builder(new Multiplication::Builder(new SquareRoot(new Rational(3), false), new Constant(Ion::Charset::IComplex), false), new Rational(2), false), false), new Subtraction::Builder(new Rational(-1,2), new Division::Builder(new Multiplication::Builder(new SquareRoot(new Rational(3), false), new Constant(Ion::Charset::IComplex), false), new Rational(2), false), false)};
      // x_k = -1/(3a)*(b+C*z+delta0/(zC)) with z = unary cube root
      for (int k = 0; k < 3; k++) {
        Expression * ccopy = c;
        Expression * delta0copy = delta0;
        if (k < 2) {
          ccopy = new Multiplication::Builder(c->clone(), unary3roots[k], false);
          delta0copy = delta0->clone();
        }
        Expression * add3Operands[3] = {b->clone(), ccopy, new Division::Builder(delta0copy, ccopy->clone(), false)};
        m_exactSolutions[k] = new Multiplication::Builder(new Division::Builder(new Rational(-1), new Multiplication::Builder(new Rational(3), a->clone(), false), false), new Addition(add3Operands, 3, false), false);
      }
      m_numberOfSolutions = 3;
    }
    m_exactSolutions[m_numberOfSolutions] = delta;
  }
#endif
}

void EquationStore::tidySolution() {
  for (int i = 0; i < k_maxNumberOfExactSolutions; i++) {
    m_exactSolutionExactLayouts[i] = Layout();
    m_exactSolutionApproximateLayouts[i] = Layout();
  }
}

Preferences::ComplexFormat EquationStore::updatedComplexFormat() {
  Preferences::ComplexFormat complexFormat = Preferences::sharedPreferences()->complexFormat();
  if (complexFormat == Preferences::ComplexFormat::Real && isExplictlyComplex()) {
    return Preferences::ComplexFormat::Cartesian;
  }
  return complexFormat;
}

bool EquationStore::isExplictlyComplex() {
  for (int i = 0; i < numberOfDefinedModels(); i++) {
    if (definedModelAtIndex(i)->containsIComplex()) {
      return true;
    }
  }
  return false;
}

}
