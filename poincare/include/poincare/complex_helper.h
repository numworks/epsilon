#ifndef POINCARE_COMPLEX_HELPER_H
#define POINCARE_COMPLEX_HELPER_H

#include <poincare/expression.h>

namespace Poincare {

class ComplexHelper {
public:
  static Expression realPartOfComplexFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit) {
    return cartesianPartOfComplexFunction(e, context, angleUnit, true);
  }
  static Expression imaginaryPartOfComplexFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit) {
    return cartesianPartOfComplexFunction(e, context, angleUnit, false);
  }
  static Expression realPartRealFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit) {
    return Expression(e).clone();
  }
  static Expression imaginaryPartRealFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit);
  // static Expression realPartMatrix(const Expression e, Context & context, Preferences::AngleUnit angleUnit);

  static Expression realPartFromPolarParts(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit) { return complexCartesianPartFromPolarParts(node, context, angleUnit, true); }
  static Expression imaginaryPartFromPolarParts(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit) { return complexCartesianPartFromPolarParts(node, context, angleUnit, false); }

private:
 static Expression cartesianPartOfComplexFunction(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit, bool real);
  static Expression complexCartesianPartFromPolarParts(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit, bool isReal);
};

}

#endif
