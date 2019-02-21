#include <poincare/trigonometry.h>
#include <poincare/addition.h>
#include <poincare/constant.h>
#include <poincare/decimal.h>
#include <poincare/derivative.h>
#include <poincare/float.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/preferences.h>
#include <poincare/rational.h>
#include <poincare/sign_function.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/trigonometry_cheat_table.h>
#include <poincare/undefined.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include <float.h>

namespace Poincare {

float Trigonometry::characteristicXRange(const Expression & e, Context & context, Preferences::AngleUnit angleUnit) {
  assert(e.numberOfChildren() == 1);
  const char x[] = {Symbol::SpecialSymbols::UnknownX, 0};
  int d = e.childAtIndex(0).polynomialDegree(context, x);
  if (d < 0 || d > 1) {
    // child(0) is not linear so we cannot easily find an interesting range
    return e.childAtIndex(0).characteristicXRange(context, angleUnit);
  }
  // The expression e is x-independent
  if (d == 0) {
    return 0.0f;
  }
  // e has the form cos/sin/tan(ax+b) so it is periodic of period 2*Pi/a
  assert(d == 1);
  /* To compute a, the slope of the expression child(0), we compute the
   * derivative of child(0) for any x value. */
  Poincare::Derivative derivative = Poincare::Derivative::Builder(e.childAtIndex(0).clone(), Symbol::Builder(x, 1), Float<float>::Builder(1.0f));
  float a = derivative.node()->approximate(float(), context, Preferences::ComplexFormat::Real, angleUnit).toScalar();
  float pi = angleUnit == Preferences::AngleUnit::Radian ? M_PI : 180.0f;
  return 2.0f*pi/std::fabs(a);
}

bool Trigonometry::isDirectTrigonometryFunction(const Expression & e) {
  return e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine || e.type() == ExpressionNode::Type::Tangent;
}

bool Trigonometry::isInverseTrigonometryFunction(const Expression & e) {
  return e.type() == ExpressionNode::Type::ArcCosine || e.type() == ExpressionNode::Type::ArcSine || e.type() == ExpressionNode::Type::ArcTangent;
}

bool Trigonometry::AreInverseFunctions(const Expression & directFunction, const Expression & inverseFunction) {
  if (!isDirectTrigonometryFunction(directFunction)) {
    return false;
  }
  ExpressionNode::Type correspondingType;
  switch (directFunction.type()) {
    case ExpressionNode::Type::Cosine:
      correspondingType = ExpressionNode::Type::ArcCosine;
      break;
    case ExpressionNode::Type::Sine:
      correspondingType = ExpressionNode::Type::ArcSine;
      break;
    default:
      assert(directFunction.type() == ExpressionNode::Type::Tangent);
      correspondingType = ExpressionNode::Type::ArcTangent;
      break;
  }
  return inverseFunction.type() == correspondingType;
}

bool Trigonometry::ExpressionIsEquivalentToTangent(const Expression & e) {
  // We look for (cos^-1 * sin)
  assert(ExpressionNode::Type::Power < ExpressionNode::Type::Sine);
  if (e.type() == ExpressionNode::Type::Multiplication
      && e.childAtIndex(1).type() == ExpressionNode::Type::Sine
      && e.childAtIndex(0).type() == ExpressionNode::Type::Power
      && e.childAtIndex(0).childAtIndex(0).type() == ExpressionNode::Type::Cosine
      && e.childAtIndex(0).childAtIndex(1).type() == ExpressionNode::Type::Rational
      && e.childAtIndex(0).childAtIndex(1).convert<Rational>().isMinusOne()) {
    return true;
  }
  return false;
}

Expression Trigonometry::shallowReduceDirectFunction(Expression & e, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  assert(isDirectTrigonometryFunction(e));

  // Step 1. Try finding an easy standard calculation reduction
  Expression lookup = TrigonometryCheatTable::Table()->simplify(e.childAtIndex(0), e.type(), context, complexFormat, angleUnit, target);
  if (!lookup.isUninitialized()) {
    e.replaceWithInPlace(lookup);
    return lookup;
  }

  // Step 2. Look for an expression of type "cos(arccos(x))", return x
  if (AreInverseFunctions(e, e.childAtIndex(0))) {
    Expression result = e.childAtIndex(0).childAtIndex(0);
    e.replaceWithInPlace(result);
    return result;
  }

  // Step 3. Look for an expression of type "cos(arcsin(x))" or "sin(arccos(x)), return sqrt(1-x^2)
  // These equalities are true on complexes
  if ((e.type() == ExpressionNode::Type::Cosine && e.childAtIndex(0).type() == ExpressionNode::Type::ArcSine) || (e.type() == ExpressionNode::Type::Sine && e.childAtIndex(0).type() == ExpressionNode::Type::ArcCosine)) {
    Expression sqrt =
      Power::Builder(
        Addition::Builder(
          Rational::Builder(1),
          Multiplication::Builder(
            Rational::Builder(-1),
            Power::Builder(e.childAtIndex(0).childAtIndex(0), Rational::Builder(2))
          )
        ),
        Rational::Builder(1,2)
      );
    // reduce x^2
    sqrt.childAtIndex(0).childAtIndex(1).childAtIndex(1).shallowReduce(context, complexFormat, angleUnit, target);
    // reduce -1*x^2
    sqrt.childAtIndex(0).childAtIndex(1).shallowReduce(context, complexFormat, angleUnit, target);
    // reduce 1-1*x^2
    sqrt.childAtIndex(0).shallowReduce(context, complexFormat, angleUnit, target);
    e.replaceWithInPlace(sqrt);
    // reduce sqrt(1+(-1)*x^2)
    return sqrt.shallowReduce(context, complexFormat, angleUnit, target);
  }

  // Step 4. Look for an expression of type "cos(arctan(x))" or "sin(arctan(x))"
  // cos(arctan(x)) --> 1/sqrt(1+x^2)
  // sin(arctan(x)) --> x/sqrt(1+x^2)
  // These equalities are true on complexes
  if ((e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine) && e.childAtIndex(0).type() == ExpressionNode::Type::ArcTangent) {
    Expression x = e.childAtIndex(0).childAtIndex(0);
    // Build 1/sqrt(1+x^2)
    Expression res =
      Power::Builder(
        Addition::Builder(
          Rational::Builder(1),
          Power::Builder(
            e.type() == ExpressionNode::Type::Cosine ? x : x.clone(),
            Rational::Builder(2))
        ),
        Rational::Builder(-1,2)
      );

    // reduce x^2
    res.childAtIndex(0).childAtIndex(1).shallowReduce(context, complexFormat, angleUnit, target);
    // reduce 1+*x^2
    res.childAtIndex(0).shallowReduce(context, complexFormat, angleUnit, target);
    if (e.type() == ExpressionNode::Type::Sine) {
      res = Multiplication::Builder(x, res);
      // reduce (1+x^2)^(-1/2)
      res.childAtIndex(0).shallowReduce(context, complexFormat, angleUnit, target);
    }
    e.replaceWithInPlace(res);
    // reduce (1+x^2)^(-1/2) or x*(1+x^2)^(-1/2)
    return res.shallowReduce(context, complexFormat, angleUnit, target);
  }

  // Step 5. Look for an expression of type "cos(-a)", return "+/-cos(a)"
  Expression positiveArg = e.childAtIndex(0).makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
  if (!positiveArg.isUninitialized()) {
    // The argument was of form cos(-a)
    if (e.type() == ExpressionNode::Type::Cosine) {
      // cos(-a) = cos(a)
      return e.shallowReduce(context, complexFormat, angleUnit, target);
    } else {
      // sin(-a) = -sin(a) or tan(-a) = -tan(a)
      Multiplication m = Multiplication::Builder(Rational::Builder(-1));
      e.replaceWithInPlace(m);
      m.addChildAtIndexInPlace(e, 1, 1);
      e.shallowReduce(context, complexFormat, angleUnit, target);
      return m.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }

  /* Step 6. Look for an expression of type "cos(p/q * Pi)" in radians or
   * "cos(p/q)" in degrees, put the argument in [0, Pi/2[ or [0, 90[ and
   * multiply the cos/sin/tan by -1 if needed.
   * We know thanks to Step 3 that p/q > 0. */
  if ((angleUnit == Preferences::AngleUnit::Radian
        && e.childAtIndex(0).type() == ExpressionNode::Type::Multiplication
        && e.childAtIndex(0).numberOfChildren() == 2
        && e.childAtIndex(0).childAtIndex(1).type() == ExpressionNode::Type::Constant
        && e.childAtIndex(0).childAtIndex(1).convert<Constant>().isPi()
        && e.childAtIndex(0).childAtIndex(0).type() == ExpressionNode::Type::Rational)
      || (angleUnit == Preferences::AngleUnit::Degree
        && e.childAtIndex(0).type() == ExpressionNode::Type::Rational))
  {
    Rational r = angleUnit == Preferences::AngleUnit::Radian ? e.childAtIndex(0).childAtIndex(0).convert<Rational>() : e.childAtIndex(0).convert<Rational>();
    /* Step 4.1. In radians:
     * We first check if p/q * Pi is already in the right quadrant:
     * p/q * Pi < Pi/2 => p/q < 2 => 2p < q */
    Integer dividand = angleUnit == Preferences::AngleUnit::Radian ? Integer::Addition(r.unsignedIntegerNumerator(), r.unsignedIntegerNumerator()) : r.unsignedIntegerNumerator();
    Integer divisor = angleUnit == Preferences::AngleUnit::Radian ? r.integerDenominator() : Integer::Multiplication(r.integerDenominator(), Integer(90));
    if (divisor.isLowerThan(dividand)) {
      /* Step 4.2. p/q * Pi is not in the wanted trigonometrical quadrant.
       * We could subtract n*Pi to p/q with n an integer.
       * Given p/q = (q'*q+r')/q, we have
       * (p/q * Pi - q'*Pi) < Pi/2 => r'/q < 1/2 => 2*r'<q
       * (q' is the theoretical n).*/
      int unaryCoefficient = 1; // store 1 or -1 for the final result.
      Integer piDivisor = angleUnit == Preferences::AngleUnit::Radian ? r.integerDenominator() : Integer::Multiplication(r.integerDenominator(), Integer(180));
      IntegerDivision div = Integer::Division(r.unsignedIntegerNumerator(), piDivisor);
      dividand = angleUnit == Preferences::AngleUnit::Radian ? Integer::Addition(div.remainder, div.remainder) : div.remainder;
      if (divisor.isLowerThan(dividand)) {
        /* Step 4.3. r'/q * Pi is not in the wanted trigonometrical quadrant,
         * and because r'<q (as r' is the remainder of an euclidian division
         * by q), we know that r'/q*Pi is in [Pi/2; Pi[.
         * So we can take the new angle Pi - r'/q*Pi, which changes cosinus or
         * tangent, but not sinus. The new rational is 1-r'/q = (q-r')/q. */
        div.remainder = Integer::Subtraction(piDivisor, div.remainder);
        if (e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Tangent) {
          unaryCoefficient *= -1;
        }
      }
      if (div.remainder.isOverflow()) {
        return e;
      }
      // Step 4.5. Build the new result.
      Integer rDenominator = r.integerDenominator();
      Expression newR = Rational::Builder(div.remainder, rDenominator);
      Expression rationalParent = angleUnit == Preferences::AngleUnit::Radian ? e.childAtIndex(0) : e;
      rationalParent.replaceChildAtIndexInPlace(0, newR);
      newR.shallowReduce(context, complexFormat, angleUnit, target);
      if (angleUnit == Preferences::AngleUnit::Radian) {
        e.childAtIndex(0).shallowReduce(context, complexFormat, angleUnit, target);
      }
      if (Integer::Division(div.quotient, Integer(2)).remainder.isOne() && e.type() != ExpressionNode::Type::Tangent) {
        /* Step 4.6. If we subtracted an odd number of Pi in 4.2, we need to
         * multiply the result by -1 (because cos((2k+1)Pi + x) = -cos(x) */
        unaryCoefficient *= -1;
      }
      Expression simplifiedCosine = e.shallowReduce(context, complexFormat, angleUnit, target); // recursive
      Multiplication m = Multiplication::Builder(Rational::Builder(unaryCoefficient));
      simplifiedCosine.replaceWithInPlace(m);
      m.addChildAtIndexInPlace(simplifiedCosine, 1, 1);
      return m.shallowReduce(context, complexFormat, angleUnit, target);
    }
    assert(r.sign() == ExpressionNode::Sign::Positive);
  }
  return e;
}

Expression Trigonometry::shallowReduceInverseFunction(Expression & e, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  assert(isInverseTrigonometryFunction(e));
  float pi = angleUnit == Preferences::AngleUnit::Radian ? M_PI : 180;

  // Step 1. Look for an expression of type "arccos(cos(x))", return x
  if (AreInverseFunctions(e.childAtIndex(0), e)) {
    float trigoOp = e.childAtIndex(0).childAtIndex(0).node()->approximate(float(), context, complexFormat, angleUnit).toScalar();
    if ((e.type() == ExpressionNode::Type::ArcCosine && trigoOp >= 0.0f && trigoOp <= pi) ||
        (e.type() == ExpressionNode::Type::ArcSine && trigoOp >= -pi/2.0f && trigoOp <= pi/2.0f) ||
        (e.type() == ExpressionNode::Type::ArcTangent && trigoOp >= -pi/2.0f && trigoOp <= pi/2.0f)) {
      Expression result = e.childAtIndex(0).childAtIndex(0);
      e.replaceWithInPlace(result);
      return result;
    }
  }

  // Step 2. Special case for arctan(sin(x)/cos(x))
  if (e.type() == ExpressionNode::Type::ArcTangent && ExpressionIsEquivalentToTangent(e.childAtIndex(0))) {
    float trigoOp = e.childAtIndex(0).childAtIndex(1).childAtIndex(0).node()->approximate(float(), context, complexFormat, angleUnit).toScalar();
    if (trigoOp >= -pi/2.0f && trigoOp <= pi/2.0f) {
      Expression result = e.childAtIndex(0).childAtIndex(1).childAtIndex(0);
      e.replaceWithInPlace(result);
      return result;
    }
  }

  // Step 3. Look for an expression of type "arctan(1/x), return sign(x)*Pi/2-arctan(x)
  if (e.type() == ExpressionNode::Type::ArcTangent && e.childAtIndex(0).type() == ExpressionNode::Type::Power && e.childAtIndex(0).childAtIndex(1).type() == ExpressionNode::Type::Rational && e.childAtIndex(0).childAtIndex(1).convert<Rational>().isMinusOne()) {
    Expression x = e.childAtIndex(0).childAtIndex(0);
    /* This equality is not true if x = 0. We apply it under certain conditions:
     * - the reduction target is the user
     * - x is numeral (which means that x != 0 otherwise 0^(-1) would have been
     *   reduced to undef) */
    if (target == ExpressionNode::ReductionTarget::User || x.isNumber()) {
      Expression sign = SignFunction::Builder(x.clone());
      Multiplication m0 = Multiplication::Builder(Rational::Builder(1,2), sign, Constant::Builder(Ion::Charset::SmallPi));
      sign.shallowReduce(context, complexFormat, angleUnit, target);
      e.replaceChildAtIndexInPlace(0, x);
      Addition a = Addition::Builder(m0);
      e.replaceWithInPlace(a);
      Multiplication m1 = Multiplication::Builder(Rational::Builder(-1), e);
      e.shallowReduce(context, complexFormat, angleUnit, target);
      a.addChildAtIndexInPlace(m1, 1, 1);
      return a.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }

  // Step 4. Try finding an easy standard calculation reduction
  Expression lookup = TrigonometryCheatTable::Table()->simplify(e.childAtIndex(0), e.type(), context, complexFormat, angleUnit, target);
  if (!lookup.isUninitialized()) {
    e.replaceWithInPlace(lookup);
    return lookup;
  }

  /* We do not apply some rules if:
   * - the parent node is a cosine, a sine or a tangent. In this case there is a simplication of
   *   form f(g(x)) with f cos, sin or tan and g acos, asin or atan.
   * - the reduction is being BottomUp. In this case, we do not yet have any
   *   information on the parent which could later be a cosine, a sine or a tangent.
   */
  Expression p = e.parent();
  bool letArcFunctionAtRoot = !p.isUninitialized() && isDirectTrigonometryFunction(p);
  /* Step 5. Handle opposite argument: arccos(-x) = Pi-arcos(x),
   * arcsin(-x) = -arcsin(x), arctan(-x)= -arctan(x) *
   */
  if (!letArcFunctionAtRoot) {
    Expression positiveArg = e.childAtIndex(0).makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
    if (!positiveArg.isUninitialized()) {
      // The argument was made positive
      // acos(-x) = pi-acos(x)
      if (e.type() == ExpressionNode::Type::ArcCosine) {
        Expression pi = angleUnit == Preferences::AngleUnit::Radian ? static_cast<Expression>(Constant::Builder(Ion::Charset::SmallPi)) : static_cast<Expression>(Rational::Builder(180));
        Subtraction s = Subtraction::Builder();
        e.replaceWithInPlace(s);
        s.replaceChildAtIndexInPlace(0, pi);
        s.replaceChildAtIndexInPlace(1, e);
        e.shallowReduce(context, complexFormat, angleUnit, target);
        return s.shallowReduce(context, complexFormat, angleUnit, target);
      } else {
        // asin(-x) = -asin(x) or atan(-x) = -atan(x)
        Multiplication m = Multiplication::Builder(Rational::Builder(-1));
        e.replaceWithInPlace(m);
        m.addChildAtIndexInPlace(e, 1, 1);
        e.shallowReduce(context, complexFormat, angleUnit, target);
        return m.shallowReduce(context, complexFormat, angleUnit, target);
      }
    }
  }

  return e;
}

template <typename T>
std::complex<T> Trigonometry::ConvertToRadian(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    return c * std::complex<T>(M_PI/180.0);
  }
  return c;
}

template <typename T>
std::complex<T> Trigonometry::ConvertRadianToAngleUnit(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    return c * std::complex<T>(180/M_PI);
  }
  return c;
}

template<typename T>
T Trigonometry::RoundToMeaningfulDigits(T result, T input) {
  /* Cheat: openbsd trigonometric functions are numerical implementation and
   * thus are approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to
   * avoid weird results as acos(1) = 6E-17 or cos(Pi/2) = 4E-17, we round
   * the result to its 1E-6 or 1E-14 precision when its ratio with the
   * argument (pi/2 in the exemple) is smaller than epsilon. This way, we
   * have sin(pi) ~ 0 and sin(1E-15)=1E-15.
   * We can't do that for all evaluation as the user can operate on values as
   * small as 1E-308 (in double) and most results still be correct. */
  if (input == 0.0 || std::fabs(result/input) <= Expression::Epsilon<T>()) {
     T precision = 10*Expression::Epsilon<T>();
     result = std::round(result/precision)*precision;
  }
  return result;
}

template <typename T>
std::complex<T> Trigonometry::RoundToMeaningfulDigits(const std::complex<T> result, const std::complex<T> input) {
  return std::complex<T>(RoundToMeaningfulDigits(result.real(), std::abs(input)), RoundToMeaningfulDigits(result.imag(), std::abs(input)));
}

template std::complex<float> Trigonometry::ConvertToRadian<float>(std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertToRadian<double>(std::complex<double>, Preferences::AngleUnit);
template std::complex<float> Trigonometry::ConvertRadianToAngleUnit<float>(std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertRadianToAngleUnit<double>(std::complex<double>, Preferences::AngleUnit);
template std::complex<float> Trigonometry::RoundToMeaningfulDigits<float>(std::complex<float>, std::complex<float>);
template std::complex<double> Trigonometry::RoundToMeaningfulDigits<double>(std::complex<double>, std::complex<double>);

}
