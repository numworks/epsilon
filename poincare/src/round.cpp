#include <poincare/round.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/power.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Round::type() const {
  return Type::Round;
}

Expression * Round::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (operand(0)->type() == Type::Matrix || operand(1)->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
#endif
  if (operand(0)->type() == Type::Rational && operand(1)->type() == Type::Rational) {
    Rational * r1 = static_cast<Rational *>(editableOperand(0));
    Rational * r2 = static_cast<Rational *>(editableOperand(1));
    if (!r2->denominator().isOne()) {
      return replaceWith(new Undefined(), true);
    }
    const Rational ten(10);
    if (Power::RationalExponentShouldNotBeReduced(&ten, r2)) {
      return this;
    }
    Rational err = Rational::Power(ten, r2->numerator());
    Rational mult = Rational::Multiplication(*r1, err);
    IntegerDivision d = Integer::Division(mult.numerator(), mult.denominator());
    Integer rounding = d.quotient;
    if (Rational::NaturalOrder(Rational(d.remainder, mult.denominator()), Rational(1,2)) >= 0) {
      rounding = Integer::Addition(rounding, Integer(1));
    }
    Rational result = Rational::Multiplication(rounding, Rational::Power(Rational(1,10), r2->numerator()));
    return replaceWith(new Rational(result), true);
  }
  return this; // TODO: implement for rationals!
}

template<typename T>
Complex<T> * Round::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  T f1 = operand(0)->approximateToScalar<T>(context, angleUnit);
  T f2 = operand(1)->approximateToScalar<T>(context, angleUnit);
  if (std::isnan(f2) || f2 != std::round(f2)) {
    return Complex<T>::NewFNAN();
  }
  T err = std::pow(10, std::floor(f2));
  return Complex<T>::NewFloat(std::round(f1*err)/err);
}

}


