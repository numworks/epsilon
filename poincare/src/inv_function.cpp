#include <poincare/inv_function.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>

namespace Poincare {

Expression InverseFunction::shallowReduce(Expression * x, Context * context, Expression * expression) {
  Expression a = x[0];
  // Check a
  if (a.deepIsMatrix(context)) {
    return expression->replaceWithUndefinedInPlace();
  }
  if (a.type() != ExpressionNode::Type::Rational) {
    return *expression;
  }

  // Special values

  // Undef if a < 0 or a > 1
  Rational rationalA = static_cast<Rational &>(a);
  if (rationalA.isNegative()) {
    return expression->replaceWithUndefinedInPlace();
  }
  Integer num = rationalA.unsignedIntegerNumerator();
  Integer den = rationalA.integerDenominator();
  if (den.isLowerThan(num)) {
    return expression->replaceWithUndefinedInPlace();
  }
  bool is0 = rationalA.isZero();
  bool is1 = !is0 && rationalA.isOne();
  if (is0 || is1) {
    Expression result = Infinity::Builder(is0);
    expression->replaceWithInPlace(result);
    return result;
  }

  return *expression;
}

}
