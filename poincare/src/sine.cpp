#include <poincare/sine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/multiplication.h>
#include <poincare/symbol.h>
#include <poincare/trigonometry.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

Expression * Sine::clone() const {
  Sine * a = new Sine(m_operands, true);
  return a;
}

Expression * Sine::immediateSimplify() {
  Expression * lookup = Trigonometry::table(operand(0), Trigonometry::Function::Sine, false);
  if (lookup != nullptr) {
    return replaceWith(lookup, true);
  }
  if (operand(0)->sign() < 0) {
    ((Expression *)operand(0))->turnIntoPositive();
    ((Expression *)operand(0))->immediateSimplify();
    const Expression * multOperands[2] = {new Rational(Integer(-1)), clone()};
    Multiplication * m = new Multiplication(multOperands, 2, false);
    ((Expression *)m->operand(1))->immediateSimplify();
    return replaceWith(m, true)->immediateSimplify();
  }
  if (operand(0)->type() == Type::Multiplication && operand(0)->operand(1)->type() == Type::Symbol && static_cast<const Symbol *>(operand(0)->operand(1))->name() == Ion::Charset::SmallPi && operand(0)->operand(0)->type() == Type::Rational) {
    Rational * r = static_cast<Rational *>((Expression *)operand(0)->operand(0));
    if (r->denominator().isLowerThan(r->numerator())) {
      IntegerDivision div = Integer::Division(r->numerator(), r->denominator());
      Rational * newR = new Rational(div.remainder, r->denominator());
      const_cast<Expression *>(operand(0))->replaceOperand(r, newR, true);
      const_cast<Expression *>(operand(0))->immediateSimplify();
      if (Integer::Division(div.quotient, Integer(2)).remainder.isOne()) {
        Expression * simplifiedSine = immediateSimplify(); // recursive
        const Expression * multOperands[2] = {new Rational(Integer(-1)), simplifiedSine->clone()};
        Multiplication * m = new Multiplication(multOperands, 2, false);
        return simplifiedSine->replaceWith(m, true)->immediateSimplify();
      } else {
        return immediateSimplify(); // recursive
      }
    }
    assert(r->sign() > 0);
    assert(r->numerator().isLowerThan(r->denominator()));
  }
  return this;
}

template<typename T>
Complex<T> Sine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() == 0) {
    T input = c.a();
    if (angleUnit == AngleUnit::Degree) {
      input *= M_PI/180;
    }
    T result = std::sin(input);
    /* Cheat: see comment in cosine.cpp
     * We cheat to avoid returning sin(Pi) = epsilon */
    if (input !=  0 && std::fabs(result/input) <= epsilon<T>()) {
      return Complex<T>::Float(0);
    }
    return Complex<T>::Float(result);
  }
  Complex<T> arg = Complex<T>::Cartesian(-c.b(), c.a());
  Complex<T> sinh = HyperbolicSine::computeOnComplex(arg, angleUnit);
  return Multiplication::compute(Complex<T>::Cartesian(0, -1), sinh);
}

}
