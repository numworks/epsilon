#include <poincare/trigonometry.h>
#include <poincare/simplification_root.h>
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

Expression * Trigonometry::immediateSimplify(Context& context, AngleUnit angleUnit) {
  Expression * lookup = Trigonometry::table(operand(0), trigonometricFunctionType(), false, context, angleUnit);
  if (lookup != nullptr) {
    return replaceWith(lookup, true);
  }
  if (operand(0)->sign() < 0) {
    Expression * op = const_cast<Expression *>(operand(0));
    Expression * newOp = op->turnIntoPositive(context, angleUnit);
    newOp->immediateSimplify(context, angleUnit);
    if (trigonometricFunctionType() == Trigonometry::Function::Cosine) {
      return immediateSimplify(context, angleUnit);
    } else if (trigonometricFunctionType() == Trigonometry::Function::Sine) {
      const Expression * multOperands[2] = {new Rational(Integer(-1)), clone()};
      Multiplication * m = new Multiplication(multOperands, 2, false);
      ((Expression *)m->operand(1))->immediateSimplify(context, angleUnit);
      return replaceWith(m, true)->immediateSimplify(context, angleUnit);
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
      const_cast<Expression *>(operand(0))->immediateSimplify(context, angleUnit);
      if (Integer::Division(div.quotient, Integer(2)).remainder.isOne()) {
        Expression * simplifiedCosine = immediateSimplify(context, angleUnit); // recursive
        const Expression * multOperands[2] = {new Rational(Integer(-1)), simplifiedCosine->clone()};
        Multiplication * m = new Multiplication(multOperands, 2, false);
        return simplifiedCosine->replaceWith(m, true)->immediateSimplify(context, angleUnit);
      } else {

        return immediateSimplify(context, angleUnit); // recursive
      }
    }
    assert(r->sign() > 0);
    assert(r->numerator().isLowerThan(r->denominator()));
  }
  return this;
}

static_assert('\x89' == Ion::Charset::SmallPi, "Incorrect");
constexpr const char * cheatTable[Trigonometry::k_numberOfEntries][3] =
{{"\x89",            "\x89*(-2)^(-1)",       "-1"},
 {"\x89*11*12^(-1)", "\x89*(-5)*12^(-1)", "(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)"},
 {"\x89*7*8^(-1)",   "\x89*(-3)*8^(-1)",  "-(2+2^(1/2))^(1/2)*2^(-1)"},
 {"\x89*5*6^(-1)",   "\x89*(-3)^(-1)",    "-3^(1/2)*2^(-1)"},
 {"\x89*4*5^(-1)",   "",                  "(-5^(1/2)-1)*4^(-1)"},
 {"\x89*3*4^(-1)",   "\x89*(-4)^(-1)",    "(-1)*(2^(-1/2))"},
 {"\x89*2*3^(-1)",   "\x89*(-6)^(-1)",    "-0.5" },
 {"\x89*5*8^(-1)",   "\x89*(-8)^(-1)",    "(2-2^(1/2))^(1/2)*(-2)^(-1)"},
 {"\x89*3*5^(-1)",   "",                  "(1-5^(1/2))*4^(-1)"},
 {"\x89*7*12^(-1)",  "\x89*(-12)^(-1)",   "-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)"},
 {"\x89*2^(-1)",     "0",                 "0"},
 {"\x89*2^(-1)",     "\x89",              "0"},
 {"\x89*5*12^(-1)",  "\x89*12^(-1)",      "6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)"},
 {"\x89*2*5^(-1)",   "",                  "(5^(1/2)-1)*4^(-1)"},
 {"\x89*3*8^(-1)",   "\x89*8^(-1)",       "(2-2^(1/2))^(1/2)*2^(-1)"},
 {"\x89*3^(-1)",     "\x89*6^(-1)",       "0.5"},
 {"",                "\x89*5^(-1)",       "(5/8-5^(1/2)/8)^(1/2)"},
 {"\x89*4^(-1)",     "\x89*4^(-1)",       "2^(-1/2)"},
 {"\x89*5^(-1)",     "",                  "(5^(1/2)+1)*4^(-1)"},
 {"\x89*6^(-1)",     "\x89*3^(-1)",       "3^(1/2)*2^(-1)"},
 {"\x89*8^(-1)",     "\x89*3*8^(-1)",     "(2+2^(1/2))^(1/2)*2^(-1)"},
 {"",                "\x89*2*5^(-1)",     "(5/8+5^(1/2)/8)^(1/2)"},
 {"\x89*12^(-1)",    "\x89*5*12^(-1)",    "6^(1/2)*4^(-1)+2^(1/2)*4^(-1)"},
 {"0",               "\x89*2^(-1)",    "1"}
};

Expression * Trigonometry::table(const Expression * e, Function f, bool inverse, Context & context, AngleUnit angleUnit) {
  int inputIndex = inverse ? 2 : (int)f;
  int outputIndex = inverse ? (int)f : 2;
  for (int i = 0; i < k_numberOfEntries; i++) {
    Expression * input = Expression::parse(cheatTable[i][inputIndex]);
    if (input == nullptr) {
      continue;
    }
    SimplificationRoot inputRoot(input);
    inputRoot.simplify(context, angleUnit); // input expression does not change, no root needed and we can use entry after
    if (inputRoot.operand(0)->compareTo(e) == 0) {
      Expression * output = Expression::parse(cheatTable[i][outputIndex]);
      if (output == nullptr) {
        return nullptr;
      }
      SimplificationRoot outputRoot(output);
      return (Expression *)(outputRoot.simplify(context, angleUnit))->operand(0);
    }
  }
  return nullptr;
}

}
