#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <poincare/expression.h>

namespace Poincare {

class Trigonometry  {
public:
  enum class Function {
    Cosine = 0,
    Sine = 1,
  };
  static float characteristicXRange(const Expression * e, Context & context, Expression::AngleUnit angleUnit);
  static Expression * shallowReduceDirectFunction(Expression * e, Context& context, Expression::AngleUnit angleUnit);
  static Expression * shallowReduceInverseFunction(Expression * e, Context& context, Expression::AngleUnit angleUnit);
  static bool ExpressionIsEquivalentToTangent(const Expression * e);
  constexpr static int k_numberOfEntries = 37;
  static Expression * table(const Expression * e, Expression::Type type, Context & context, Expression::AngleUnit angleUnit); // , Function f, bool inverse
};

}

#endif
