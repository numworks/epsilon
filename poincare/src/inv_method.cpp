#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/inv_method.h>
#include <poincare/rational.h>

namespace Poincare {

OExpression InverseMethod::shallowReduce(OExpression *x,
                                         const Distribution *distribution,
                                         OExpression *parameters,
                                         ReductionContext reductionContext,
                                         OExpression *expression) const {
  OExpression a = x[0];
  // Check a
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
    // TODO: for all distributions with finite support
    if (distribution->hasType(Distribution::Type::Binomial)) {
      if (is0) {
        OExpression p = parameters[1];
        if (p.type() != ExpressionNode::Type::Rational) {
          return *expression;
        }
        if (static_cast<Rational &>(p).isOne()) {
          OExpression result = Rational::Builder(0);
          expression->replaceWithInPlace(result);
          return result;
        }
        return expression->replaceWithUndefinedInPlace();
      }
      // n if a == 1 (TODO: false if p == 0 ?)
      OExpression n = parameters[0];
      expression->replaceWithInPlace(n);
      return n;
    }

    if (distribution->hasType(Distribution::Type::Geometric)) {
      if (is0) {
        return expression->replaceWithUndefinedInPlace();
      }

      // is1
      OExpression p = parameters[0];
      if (p.type() != ExpressionNode::Type::Rational) {
        return *expression;
      }
      if (static_cast<Rational &>(p).isOne()) {
        OExpression result = Rational::Builder(1);
        expression->replaceWithInPlace(result);
        return result;
      }
      OExpression result = Infinity::Builder(false);
      expression->replaceWithInPlace(result);
      return result;
    }

    if (distribution->hasType(Distribution::Type::Normal) ||
        distribution->hasType(Distribution::Type::Student)) {
      // Normal and Student (all distributions with real line support)
      OExpression result = Infinity::Builder(is0);
      expression->replaceWithInPlace(result);
      return result;
    }
  }

  // expectedValue if a == 0.5 and continuous and symmetrical
  if (rationalA.isHalf()) {
    if (distribution->hasType(Distribution::Type::Normal)) {
      OExpression mu = parameters[0];
      expression->replaceWithInPlace(mu);
      return mu;
    }
    if (distribution->hasType(Distribution::Type::Student)) {
      OExpression zero = Rational::Builder(0);
      expression->replaceWithInPlace(zero);
      return zero;
    }
  }

  return *expression;
}

}  // namespace Poincare
