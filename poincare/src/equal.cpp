extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
}
#include <poincare/equal.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"

namespace Poincare {

Expression::Type Equal::type() const {
  return Type::Equal;
}

Expression * Equal::clone() const {
  return new Equal(operands(), true);
}

int Equal::polynomialDegree(char symbolName) const {
  return -1;
}

int Equal::solve(Expression ** solutions, Expression ** delta, Context & context, AngleUnit angleUnit) const {
  Expression * sub = new Subtraction(operand(0), operand(1), true);
  Reduce(&sub, context, angleUnit);
  char variables[k_maxNumberOfVariables+1] = {0};
  int var = sub->getVariables(variables);
  if (var < 0) {
    delete sub;
    // Too many unknown variables
    return -10;
  }
  if (var == 0) {
    int sol = -1;
    if (sub->type() == Type::Rational && static_cast<Rational *>(sub)->isZero()) {
      // 0 == 0
      sol = INT_MAX;
    }
    delete sub;
    return sol;
  }
  if (var == 1) {
    char variable = variables[0];
    Expression * coefficients[k_maxNumberOfPolynomialCoefficients];
    int deg = sub->getPolynomialCoefficients(variable, coefficients);
    if (deg < 0 || deg > 2) {
      // Case 0: 1 unknown variable, non-polynomial --> approximate solution
      return 0;
    }
    delete sub;
    for (int i = 0; i <= deg; i++) {
      Reduce(&coefficients[i], context, angleUnit);
    }
    // Case 1: 1 unknown variable, polynomial of degree <= 2 --> Exact solution
    return oneDimensialPolynomialSolve(coefficients, deg, solutions, delta, context, angleUnit);
  }
  assert(var > 1);
  // Case 2: several variables, linear --> ?
  delete sub;
  return 0;
}

Expression * Equal::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  if (operand(0)->isIdenticalTo(operand(1))) {
    return replaceWith(new Rational(1), true);
  }
  return this;
}

int Equal::oneDimensialPolynomialSolve(Expression ** coefficients, int degree, Expression ** solutions, Expression ** delta, Context & context, AngleUnit angleUnit) const {
  int sol = -1;
  if (degree == 0) {
    if (coefficients[0]->type() == Type::Rational && static_cast<Rational *>(coefficients[0])->isZero()) {
      // 0 == 0
      sol = INT_MAX;
    }
    delete coefficients[0];
    return sol;
  }
  switch (degree) {
    case 1:
    {
      // ax+b = 0
      solutions[0] = new Division(new Opposite(coefficients[0], false), coefficients[1], false);
      sol = 1;
      break;
    }
    case 2:
    {
      Expression * deltaDenominator[3] = {new Rational(4), coefficients[0]->clone(), coefficients[2]->clone()};
      *delta = new Subtraction(new Power(coefficients[1]->clone(), new Rational(2), false), new Multiplication(deltaDenominator, 3, false), false);
      Simplify(delta, context, angleUnit);
      if ((*delta)->type() == Type::Rational && static_cast<Rational *>(*delta)->isZero()) {
        solutions[0] = new Division(new Opposite(coefficients[1], false), new Multiplication(new Rational(2), coefficients[2]), false);
        sol = 1;
      } else {
        solutions[0] = new Division(new Subtraction(new Opposite(coefficients[1]->clone(), false), new SquareRoot((*delta)->clone(), false), false), new Multiplication(new Rational(2), coefficients[2]->clone()), false);
        solutions[1] = new Division(new Addition(new Opposite(coefficients[1], false), new SquareRoot((*delta)->clone(), false), false), new Multiplication(new Rational(2), coefficients[2]), false);
        sol = 2;
      }
      delete coefficients[0];
      break;
    }
#if 0
    case 3:
    {
      Expression * a = coefficients[3];
      Expression * b = coefficients[2];
      Expression * c = coefficients[1];
      Expression * d = coefficients[0];
      // Delta = b^2*c^2+18abcd-27a^2*d^2-4ac^3-4db^3
      Expression * mult0Operands[2] = {new Power(b->clone(), new Rational(2), false), new Power(c->clone(), new Rational(2), false)};
      Expression * mult1Operands[5] = {new Rational(18), a->clone(), b->clone(), c->clone(), d->clone()};
      Expression * mult2Operands[3] = {new Rational(-27), new Power(a->clone(), new Rational(2), false), new Power(d->clone(), new Rational(2), false)};
      Expression * mult3Operands[3] = {new Rational(-4), a->clone(), new Power(c->clone(), new Rational(3), false)};
      Expression * mult4Operands[3] = {new Rational(-4), d->clone(), new Power(b->clone(), new Rational(3), false)};
      Expression * add0Operands[5] = {new Multiplication(mult0Operands, 2, false), new Multiplication(mult1Operands, 5, false), new Multiplication(mult2Operands, 3, false), new Multiplication(mult3Operands, 3, false), new Multiplication(mult4Operands, 3, false)};
      *delta = new Addition(add0Operands, 5, false);
      Simplify(delta, context, angleUnit);
      // Delta0 = b^2-3ac
      Expression * mult5Operands[3] = {new Rational(3), a->clone(), c->clone()};
      Expression * delta0 = new Subtraction(new Power(b->clone(), new Rational(2), false), new Multiplication(mult5Operands, 3, false), false);
      Reduce(&delta0, context, angleUnit);
      if ((*delta)->type() == Type::Rational && static_cast<Rational *>(*delta)->isZero()) {
        if (delta0->type() == Type::Rational && static_cast<Rational *>(delta0)->isZero()) {
        // delta0 = 0 && delta = 0 --> x0 = -b/(3a)
          delete delta0;
          solutions[0] = new Opposite(new Division(b, new Multiplication(new Rational(3), a, false), false), false);
          sol = 1;
          delete c;
          delete d;
          break;
        }
        // delta = 0 --> x0 = (9ad-bc)/(2delta0)
        //           --> x1 = (4abc-9a^2d-b^3)/(a*delta0)
        Expression * mult6Operands[3] = {new Rational(9), a, d};
        solutions[0] = new Division(new Subtraction(new Multiplication(mult6Operands, 3, false), new Multiplication(b, c, false), false), new Multiplication(new Rational(2), delta0, false), false);
        Expression * mult7Operands[4] = {new Rational(4), a->clone(), b->clone(), c->clone()};
        Expression * mult8Operands[3] = {new Rational(-9), new Power(a->clone(), new Rational(2), false), d->clone()};
        Expression * add1Operands[3] = {new Multiplication(mult7Operands, 4, false), new Multiplication(mult8Operands,3, false), new Opposite(new Power(b->clone(), new Rational(3), false), false)};
        solutions[1] = new Division(new Addition(add1Operands, 3, false), new Multiplication(a->clone(), delta0, false), false);
        sol = 2;
        break;
      } else {
      // delta1 = 2b^3-9abc+27a^2*d
        Expression * mult9Operands[4] = {new Rational(-9), a, b, c};
        Expression * mult10Operands[3] = {new Rational(27), new Power(a->clone(), new Rational(2), false), d};
        Expression * add2Operands[3] = {new Multiplication(new Rational(2), new Power(b->clone(), new Rational(3), false), false), new Multiplication(mult9Operands, 4, false), new Multiplication(mult10Operands, 3, false)};
        Expression * delta1 = new Addition(add2Operands, 3, false);
        // C = Root((delta1+sqrt(-27a^2*delta))/2, 3)
        Expression * mult11Operands[3] = {new Rational(-27), new Power(a->clone(), new Rational(2), false), (*delta)->clone()};
        Expression * c = new Power(new Division(new Addition(delta1, new SquareRoot(new Multiplication(mult11Operands, 3, false), false), false), new Rational(2), false), new Rational(1,3), false);
        Expression * unary3roots[2] = {new Addition(new Rational(-1,2), new Division(new Multiplication(new SquareRoot(new Rational(3), false), new Symbol(Ion::Charset::IComplex), false), new Rational(2), false), false), new Subtraction(new Rational(-1,2), new Division(new Multiplication(new SquareRoot(new Rational(3), false), new Symbol(Ion::Charset::IComplex), false), new Rational(2), false), false)};
        // x_k = -1/(3a)*(b+C*z+delta0/(zC)) with z = unary cube root
        for (int k = 0; k < 3; k++) {
          Expression * ccopy = c;
          Expression * delta0copy = delta0;
          if (k < 2) {
            ccopy = new Multiplication(c->clone(), unary3roots[k], false);
            delta0copy = delta0->clone();
          }
          Expression * add3Operands[3] = {b->clone(), ccopy, new Division(delta0copy, ccopy->clone(), false)};
          solutions[k] = new Multiplication(new Division(new Rational(-1), new Multiplication(new Rational(3), a->clone(), false), false), new Addition(add3Operands, 3, false), false);
        }
        sol = 3;
        break;
      }
    }
#endif
    default:
    {
      assert(false);
      sol = 0;
      break;
    }
  }
  for (int i = 0; i < sol; i++) {
    Simplify(&solutions[i], context, angleUnit);
  }
  return sol;
}

ExpressionLayout * Equal::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  HorizontalLayout * result = new HorizontalLayout();
  result->addOrMergeChildAtIndex(operand(0)->createLayout(floatDisplayMode, complexFormat), 0, false);
  result->addChildAtIndex(new CharLayout('='), result->numberOfChildren());
  result->addOrMergeChildAtIndex(operand(1)->createLayout(floatDisplayMode, complexFormat), result->numberOfChildren(), false);
  return result;
}

template<typename T>
Expression * Equal::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Float(NAN));
}

}
