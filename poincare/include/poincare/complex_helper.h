#ifndef POINCARE_COMPLEX_HELPER_H
#define POINCARE_COMPLEX_HELPER_H

#include <poincare/expression.h>
#include <poincare/complex_cartesian.h>
#include <poincare/complex_polar.h>

namespace Poincare {

class ComplexHelper {
public:
  static ComplexCartesian complexCartesianComplexFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit);
  static ComplexCartesian complexCartesianRealFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit);
  // static Expression realPartMatrix(const Expression e, Context & context, Preferences::AngleUnit angleUnit);
  static ComplexCartesian complexCartesianFromComplexPolar(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit);
  static ComplexPolar complexPolarFromComplexCartesian(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit);
  static Expression complexSquareNormComplexCartesian(Expression real, Expression imag, Context & context, Preferences::AngleUnit angleUnit);
private:
  static Expression complexCartesianFromComplexPolarHelper(Expression norm, Expression trigo, Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
