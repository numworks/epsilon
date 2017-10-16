#include <poincare/trigonometric_function.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
#include <poincare/symbol.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression * TrigonometricFunction::immediateSimplify() {
  Expression * lookup = Trigonometry::table(operand(0), trigonometricFunctionType(), false);
  if (lookup != nullptr) {
    return replaceWith(lookup, true);
  }
  if (operand(0)->sign() < 0) {
    ((Expression *)operand(0))->turnIntoPositive();
    ((Expression *)operand(0))->immediateSimplify();
    if (trigonometricFunctionType() == Trigonometry::Function::Cosine) {
      return immediateSimplify();
    } else if (trigonometricFunctionType() == Trigonometry::Function::Sine) {
      const Expression * multOperands[2] = {new Rational(Integer(-1)), clone()};
      Multiplication * m = new Multiplication(multOperands, 2, false);
      ((Expression *)m->operand(1))->immediateSimplify();
      return replaceWith(m, true)->immediateSimplify();
    }
    assert(false);
  }
  if (operand(0)->type() == Type::Multiplication && operand(0)->operand(1)->type() == Type::Symbol && static_cast<const Symbol *>(operand(0)->operand(1))->name() == Ion::Charset::SmallPi && operand(0)->operand(0)->type() == Type::Rational) {
    Rational * r = static_cast<Rational *>((Expression *)operand(0)->operand(0));
    // Replace argument in [0, Pi[
    if (r->denominator().isLowerThan(r->numerator())) {
      IntegerDivision div = Integer::Division(r->numerator(), r->denominator());
      // For Sine, replace argument in [0, Pi/2[
      if (trigonometricFunctionType() == Trigonometry::Function::Sine && r->denominator().isLowerThan(Integer::Addition(div.remainder, div.remainder))) {
        div.remainder = Integer::Subtraction(r->denominator(), div.remainder);
      }
      Rational * newR = new Rational(div.remainder, r->denominator());
      const_cast<Expression *>(operand(0))->replaceOperand(r, newR, true);
      const_cast<Expression *>(operand(0))->immediateSimplify();
      if (Integer::Division(div.quotient, Integer(2)).remainder.isOne()) {
        Expression * simplifiedCosine = immediateSimplify(); // recursive
        const Expression * multOperands[2] = {new Rational(Integer(-1)), simplifiedCosine->clone()};
        Multiplication * m = new Multiplication(multOperands, 2, false);
        return simplifiedCosine->replaceWith(m, true)->immediateSimplify();
      } else {

        return immediateSimplify(); // recursive
      }
    }
    assert(r->sign() > 0);
    assert(r->numerator().isLowerThan(r->denominator()));
  }
  return this;
}

}
