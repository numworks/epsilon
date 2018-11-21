#include <poincare/trigonometry.h>
#include <poincare/constant.h>
#include <poincare/symbol.h>
#include <poincare/preferences.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <poincare/derivative.h>
#include <poincare/decimal.h>
#include <poincare/float.h>
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
  Poincare::Derivative derivative = Poincare::Derivative::Builder(e.childAtIndex(0).clone(), Symbol(x, 1), Float<float>(1.0f));
  float a = derivative.approximateToScalar<float>(context, angleUnit);
  float pi = angleUnit == Preferences::AngleUnit::Radian ? M_PI : 180.0f;
  return 2.0f*pi/std::fabs(a);
}

Expression Trigonometry::shallowReduceDirectFunction(Expression & e, Context& context, Preferences::AngleUnit angleUnit) {
  assert(e.type() == ExpressionNode::Type::Sine
      || e.type() == ExpressionNode::Type::Cosine
      || e.type() == ExpressionNode::Type::Tangent);

  // Step 1. Try finding an easy standard calculation reduction
  Expression lookup = Trigonometry::table(e.childAtIndex(0), e.type(), context, angleUnit);
  if (!lookup.isUninitialized()) {
    e.replaceWithInPlace(lookup);
    return lookup;
  }

  // Step 2. Look for an expression of type "cos(arccos(x))", return x
  ExpressionNode::Type correspondingType = e.type() == ExpressionNode::Type::Cosine ? ExpressionNode::Type::ArcCosine :
    (e.type() == ExpressionNode::Type::Sine ? ExpressionNode::Type::ArcSine : ExpressionNode::Type::ArcTangent);
  if (e.childAtIndex(0).type() == correspondingType) {
    Expression result = e.childAtIndex(0).childAtIndex(0);
    e.replaceWithInPlace(result);
    return result;
  }

  // Step 3. Look for an expression of type "cos(-a)", return "+/-cos(a)"
  if (e.childAtIndex(0).sign() == ExpressionNode::Sign::Negative) {
    e.childAtIndex(0).setSign(ExpressionNode::Sign::Positive, context, angleUnit).shallowReduce(context, angleUnit);
    if (e.type() == ExpressionNode::Type::Cosine) {
      return e.shallowReduce(context, angleUnit);
    } else {
      Multiplication m(Rational(-1));
      e.replaceWithInPlace(m);
      m.addChildAtIndexInPlace(e, 1, 1);
      e.shallowReduce(context, angleUnit);
      return m.shallowReduce(context, angleUnit);
    }
  }

  /* Step 4. Look for an expression of type "cos(p/q * Pi)" in radians or
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
      if (div.remainder.isInfinity()) {
        return e;
      }
      // Step 4.5. Build the new result.
      Integer rDenominator = r.integerDenominator();
      Expression newR = Rational(div.remainder, rDenominator);
      Expression rationalParent = angleUnit == Preferences::AngleUnit::Radian ? e.childAtIndex(0) : e;
      rationalParent.replaceChildAtIndexInPlace(0, newR);
      newR.shallowReduce(context, angleUnit);
      if (angleUnit == Preferences::AngleUnit::Radian) {
        e.childAtIndex(0).shallowReduce(context, angleUnit);
      }
      if (Integer::Division(div.quotient, Integer(2)).remainder.isOne() && e.type() != ExpressionNode::Type::Tangent) {
        /* Step 4.6. If we subtracted an odd number of Pi in 4.2, we need to
         * multiply the result by -1 (because cos((2k+1)Pi + x) = -cos(x) */
        unaryCoefficient *= -1;
      }
      Expression simplifiedCosine = e.shallowReduce(context, angleUnit); // recursive
      Multiplication m = Multiplication(Rational(unaryCoefficient));
      simplifiedCosine.replaceWithInPlace(m);
      m.addChildAtIndexInPlace(simplifiedCosine, 1, 1);
      return m.shallowReduce(context, angleUnit);
    }
    assert(r.sign() == ExpressionNode::Sign::Positive);
  }
  return e;
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

Expression Trigonometry::shallowReduceInverseFunction(Expression & e, Context& context, Preferences::AngleUnit angleUnit) {
  assert(e.type() == ExpressionNode::Type::ArcCosine || e.type() == ExpressionNode::Type::ArcSine || e.type() == ExpressionNode::Type::ArcTangent);
  ExpressionNode::Type correspondingType = e.type() == ExpressionNode::Type::ArcCosine ? ExpressionNode::Type::Cosine : (e.type() == ExpressionNode::Type::ArcSine ? ExpressionNode::Type::Sine : ExpressionNode::Type::Tangent);
  float pi = angleUnit == Preferences::AngleUnit::Radian ? M_PI : 180;

  // Step 1. Look for an expression of type "arccos(cos(x))", return x
  if (e.childAtIndex(0).type() == correspondingType) {
    float trigoOp = e.childAtIndex(0).childAtIndex(0).approximateToScalar<float>(context, angleUnit);
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
    float trigoOp = e.childAtIndex(0).childAtIndex(1).childAtIndex(0).approximateToScalar<float>(context, angleUnit);
    if (trigoOp >= -pi/2.0f && trigoOp <= pi/2.0f) {
      Expression result = e.childAtIndex(0).childAtIndex(1).childAtIndex(0);
      e.replaceWithInPlace(result);
      return result;
    }
  }

  // Step 3. Try finding an easy standard calculation reduction
  Expression lookup = Trigonometry::table(e.childAtIndex(0), e.type(), context, angleUnit);
  if (!lookup.isUninitialized()) {
    e.replaceWithInPlace(lookup);
    return lookup;
  }

  /* Step 4. Handle negative arguments: arccos(-x) = Pi-arcos(x),
   * arcsin(-x) = -arcsin(x), arctan(-x)= -arctan(x) */
  if (e.childAtIndex(0).sign() == ExpressionNode::Sign::Negative
      || (e.childAtIndex(0).type() == ExpressionNode::Type::Multiplication
        && e.childAtIndex(0).childAtIndex(0).type() == ExpressionNode::Type::Rational
        && e.childAtIndex(0).childAtIndex(0).convert<Rational>().isMinusOne()))
  {
    Expression newArgument;
    if (e.childAtIndex(0).sign() == ExpressionNode::Sign::Negative) {
      newArgument = e.childAtIndex(0).setSign(ExpressionNode::Sign::Positive, context, angleUnit);
    } else {
      newArgument = e.childAtIndex(0);
      static_cast<Multiplication&>(newArgument).removeChildAtIndexInPlace(0);
    }
    newArgument = newArgument.shallowReduce(context, angleUnit);
    if (e.type() == ExpressionNode::Type::ArcCosine) {
      // Do the reduction after the if case, or it might change the result!
      Expression pi = angleUnit == Preferences::AngleUnit::Radian ? static_cast<Expression>(Constant(Ion::Charset::SmallPi)) : static_cast<Expression>(Rational(180));
      Subtraction s;
      e.replaceWithInPlace(s);
      s.replaceChildAtIndexInPlace(0, pi);
      s.replaceChildAtIndexInPlace(1, e);
      e.shallowReduce(context, angleUnit);
      return s.shallowReduce(context, angleUnit);
    } else {
      Multiplication m(Rational(-1));
      e.replaceWithInPlace(m);
      m.addChildAtIndexInPlace(e, 1, 1);
      e.shallowReduce(context, angleUnit);
      return m.shallowReduce(context, angleUnit);
    }
  }

  return e;
}

/* TODO: We use the cheat table to look for known simplifications (e.g.
 * cos(0)=1). To look for a simplification, we parse, simplify and compare all
 * known values to the input expression, which is slow. To make this faster, we
 * have several options:
 * - Compare double values instead of trees (a hash table)
 * - Store parsed Trees and compare buffers:
 *   - By parsing all expressions at initialization and storing them (takes a
 *     lot of RAM)
 *   - By having constexpr constructor of TreeNodes (not possible because
 *     TreeNode has a virtual destructor) and storing Trees in Flash memory
 * - Use a prefix Tree to search for a match (also called Trie) */

static_assert('\x8A' == Ion::Charset::SmallPi, "Unicode error");
constexpr const char * cheatTable[Trigonometry::k_numberOfEntries][5] =
// Angle in Radian | Angle in Degree | Cosine | Sine | Tangent
{{"-90",    "\x8A*(-2)^(-1)",    "",                                   "-1",                                 "undef"},
 {"-75",    "\x8A*(-5)*12^(-1)", "",                                   "(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)", "-(3^(1/2)+2)"},
 {"-72",    "\x8A*2*(-5)^(-1)",  "",                                   "-(5/8+5^(1/2)/8)^(1/2)",             "-(5+2*5^(1/2))^(1/2)"},
 {"-135/2", "\x8A*(-3)*8^(-1)",  "",                                   "-(2+2^(1/2))^(1/2)*2^(-1)",          "-1-2^(1/2)"},
 {"-60",    "\x8A*(-3)^(-1)",    "",                                   "-3^(1/2)*2^(-1)",                    "-3^(1/2)"},
 {"-54",    "\x8A*(-3)*10^(-1)", "",                                   "4^(-1)*(-1-5^(1/2))",                "-(1+2*5^(-1/2))^(1/2)"},
 {"-45",    "\x8A*(-4)^(-1)",    "",                                   "(-1)*(2^(-1/2))",                    "-1"},
 {"-36",    "\x8A*(-5)^(-1)",    "",                                   "-(5/8-5^(1/2)/8)^(1/2)",             "-(5-2*5^(1/2))^(1/2)"},
 {"-30",    "\x8A*(-6)^(-1)",    "",                                   "-0.5",                               "-3^(-1/2)"},
 {"-45/2",  "\x8A*(-8)^(-1)",    "",                                   "(2-2^(1/2))^(1/2)*(-2)^(-1)",        "1-2^(1/2)"},
 {"-18",    "\x8A*(-10)^(-1)",   "",                                   "4^(-1)*(1-5^(1/2))",                 "-(1-2*5^(-1/2))^(1/2)"},
 {"-15",    "\x8A*(-12)^(-1)",   "",                                   "-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",     "3^(1/2)-2"},
 {"0",      "0",                 "1",                                  "0",                                  "0"},
 {"15",     "\x8A*12^(-1)",      "6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",      "6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)",   "-(3^(1/2)-2)"},
 {"18",     "\x8A*10^(-1)",      "(5/8+5^(1/2)/8)^(1/2)",              "4^(-1)*(5^(1/2)-1)",                 "(1-2*5^(-1/2))^(1/2)"},
 {"45/2",   "\x8A*8^(-1)",       "(2+2^(1/2))^(1/2)*2^(-1)",           "(2-2^(1/2))^(1/2)*2^(-1)",           "2^(1/2)-1"},
 {"30",     "\x8A*6^(-1)",       "3^(1/2)*2^(-1)",                     "0.5",                                "3^(-1/2)"},
 {"36",     "\x8A*5^(-1)",       "(5^(1/2)+1)*4^(-1)",                 "(5/8-5^(1/2)/8)^(1/2)",              "(5-2*5^(1/2))^(1/2)"},
 {"45",     "\x8A*4^(-1)",       "2^(-1/2)",                           "2^(-1/2)",                           "1"},
 {"54",     "\x8A*3*10^(-1)",    "(5/8-5^(1/2)/8)^(1/2)",              "4^(-1)*(5^(1/2)+1)",                 "(1+2*5^(-1/2))^(1/2)"},
 {"60",     "\x8A*3^(-1)",       "0.5",                                "3^(1/2)*2^(-1)",                     "3^(1/2)"},
 {"135/2",  "\x8A*3*8^(-1)",     "(2-2^(1/2))^(1/2)*2^(-1)",           "(2+2^(1/2))^(1/2)*2^(-1)",           "1+2^(1/2)"},
 {"72",     "\x8A*2*5^(-1)",     "(5^(1/2)-1)*4^(-1)",                 "(5/8+5^(1/2)/8)^(1/2)",              "(5+2*5^(1/2))^(1/2)"},
 {"75",     "\x8A*5*12^(-1)",    "6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)",   "6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",      "3^(1/2)+2"},
 {"90",     "\x8A*2^(-1)",       "0",                                  "1",                                  "undef"},
 {"105",    "\x8A*7*12^(-1)",    "-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",     "",                                   ""},
 {"108",    "\x8A*3*5^(-1)",     "(1-5^(1/2))*4^(-1)",                 "",                                   ""},
 {"225/2",  "\x8A*5*8^(-1)",     "(2-2^(1/2))^(1/2)*(-2)^(-1)",        "",                                   ""},
 {"120",    "\x8A*2*3^(-1)",     "-0.5",                               "",                                   ""},
 {"126",    "\x8A*7*10^(-1)",    "-(5*8^(-1)-5^(1/2)*8^(-1))^(1/2)",   "",                                   ""},
 {"135",    "\x8A*3*4^(-1)",     "(-1)*(2^(-1/2))",                    "",                                   ""},
 {"144",    "\x8A*4*5^(-1)",     "(-5^(1/2)-1)*4^(-1)",                "",                                   ""},
 {"150",    "\x8A*5*6^(-1)",     "-3^(1/2)*2^(-1)",                    "",                                   ""},
 {"315/2",  "\x8A*7*8^(-1)",     "-(2+2^(1/2))^(1/2)*2^(-1)",          "",                                   ""},
 {"162",    "\x8A*9*10^(-1)",    "-(5*8^(-1)+5^(1/2)*8^(-1))^(1/2)",   "",                                   ""},
 {"165",    "\x8A*11*12^(-1)",   "(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)", "",                                   ""},
 {"180",    "\x8A",              "-1",                                 "0",                                  "0"}};

Expression Trigonometry::table(const Expression e, ExpressionNode::Type type, Context & context, Preferences::AngleUnit angleUnit) {
  assert(type == ExpressionNode::Type::Sine
      || type == ExpressionNode::Type::Cosine
      || type == ExpressionNode::Type::Tangent
      || type == ExpressionNode::Type::ArcCosine
      || type == ExpressionNode::Type::ArcSine
      || type == ExpressionNode::Type::ArcTangent);

  int angleUnitIndex = angleUnit == Preferences::AngleUnit::Radian ? 1 : 0;
  int trigonometricFunctionIndex = type == ExpressionNode::Type::Cosine || type == ExpressionNode::Type::ArcCosine ? 2 : (type == ExpressionNode::Type::Sine || type == ExpressionNode::Type::ArcSine ? 3 : 4);
  int inputIndex = type == ExpressionNode::Type::ArcCosine || type == ExpressionNode::Type::ArcSine || type == ExpressionNode::Type::ArcTangent ? trigonometricFunctionIndex : angleUnitIndex;
  int outputIndex = type == ExpressionNode::Type::ArcCosine || type == ExpressionNode::Type::ArcSine || type == ExpressionNode::Type::ArcTangent ? angleUnitIndex : trigonometricFunctionIndex;

  /* Avoid looping if we can exclude quickly that the e is in the table */
  if (inputIndex == 0 && e.type() != ExpressionNode::Type::Rational) {
    return Expression();
  }
  if (inputIndex == 1 && e.type() != ExpressionNode::Type::Rational && e.type() != ExpressionNode::Type::Multiplication && e.type() != ExpressionNode::Type::Constant) {
    return Expression();
  }
  if (inputIndex >1 && e.type() != ExpressionNode::Type::Rational && e.type() != ExpressionNode::Type::Multiplication && e.type() != ExpressionNode::Type::Power && e.type() != ExpressionNode::Type::Addition) {
    return Expression();
  }
  for (int i = 0; i < k_numberOfEntries; i++) {
    Expression input = Expression::Parse(cheatTable[i][inputIndex]);
    if (input.isUninitialized()) {
      continue;
    }
    input = input.deepReduce(context, angleUnit);
    bool rightInput = input.isIdenticalTo(e);
    if (rightInput) {
      Expression output = Expression::Parse(cheatTable[i][outputIndex]);
      if (output.isUninitialized()) {
        return Expression();
      }
      return output.deepReduce(context, angleUnit);
    }
  }
  return Expression();
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
  if (input == 0.0 || std::fabs(result/input) <= Expression::epsilon<T>()) {
     T precision = 10*Expression::epsilon<T>();
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
