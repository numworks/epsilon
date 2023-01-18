#include <poincare/inv_method.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>

namespace Poincare {

Expression InverseMethod::shallowReduce(Expression * x, const Distribution * distribution, Expression * parameters, ReductionContext reductionContext, Expression * expression) const {
  Expression a = x[0];
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
        Expression p = parameters[1];
        if (p.type() != ExpressionNode::Type::Rational) {
          return *expression;
        }
        if (static_cast<Rational &>(p).isOne()) {
          Expression result = Rational::Builder(0);
          expression->replaceWithInPlace(result);
          return result;
        }
        return expression->replaceWithUndefinedInPlace();
      }
      // n if a == 1
      // TODO: false if p == 0 ?
      Expression n = parameters[0];
      expression->replaceWithInPlace(n);
      return n;
    }

    if (distribution->hasType(Distribution::Type::Geometric)) {
      if (is0) {
        return expression->replaceWithUndefinedInPlace();
      }

      // is1
      Expression p = parameters[0];
      if (p.type() != ExpressionNode::Type::Rational) {
        return *expression;
      }
      if (static_cast<Rational &>(p).isOne()) {
        Expression result = Rational::Builder(1);
        expression->replaceWithInPlace(result);
        return result;
      }
      Expression result = Infinity::Builder(false);
      expression->replaceWithInPlace(result);
      return result;
    }

    if (distribution->hasType(Distribution::Type::Normal) || distribution->hasType(Distribution::Type::Student)) {
      // Normal and Student (all distributions with real line support)
      Expression result = Infinity::Builder(is0);
      expression->replaceWithInPlace(result);
      return result;
    }
  }

  // expectedValue if a == 0.5 and continuous and symmetrical
  if (rationalA.isHalf()) {
    if (distribution->hasType(Distribution::Type::Normal)) {
      Expression mu = parameters[0];
      expression->replaceWithInPlace(mu);
      return mu;
    }
    if (distribution->hasType(Distribution::Type::Student)) {
      Expression zero = Rational::Builder(0);
      expression->replaceWithInPlace(zero);
      return zero;
    }
  }

  return *expression;
}

}
