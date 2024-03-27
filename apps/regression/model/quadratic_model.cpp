#include "quadratic_model.h"

#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <poincare/based_integer.h>
#include <poincare/decimal.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/number.h>
#include <poincare/power.h>
#include <poincare/print.h>
#include <poincare/symbol.h>

using namespace Poincare;
using namespace PoincareJ;
using namespace Shared;

namespace Regression {

Layout QuadraticModel::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("2"_l) ^ "+b·x+c"_l;
}

Expression QuadraticModel::privateExpression(double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  // a*x^2+b*x+c
  return AdditionOrSubtractionBuilder(
      AdditionOrSubtractionBuilder(
          Multiplication::Builder(Number::DecimalNumber(a),
                                  Power::Builder(Symbol::Builder(k_xSymbol),
                                                 BasedInteger::Builder(2))),
          Multiplication::Builder(Number::DecimalNumber(std::fabs(b)),
                                  Symbol::Builder(k_xSymbol)),
          b >= 0.0),
      Number::DecimalNumber(std::fabs(c)), c >= 0.0);
}

double QuadraticModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a * x * x + b * x + c;
}

double QuadraticModel::partialDerivate(double* modelCoefficients,
                                       int derivateCoefficientIndex,
                                       double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: x^2
    return x * x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: x
    return x;
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: 1
  return 1.0;
}

}  // namespace Regression
