#include <poincare/logarithm.h>
#include <poincare/division.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/symbol.h>
#include <poincare/arithmetic.h>
#include <poincare/naperian_logarithm.h>
#include <cmath>
#include <ion.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include "layout/baseline_relative_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Expression::Type Logarithm::type() const {
  return Type::Logarithm;
}

Expression * Logarithm::clone() const {
  return new Logarithm(operands(), numberOfOperands(), true);
}

template<typename T>
Complex<T> Logarithm::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::log10(c.a()));
}

Expression * Logarithm::shallowSimplify(Context& context, AngleUnit angleUnit) {
  if (operand(0)->sign() == Sign::Negative || (numberOfOperands() == 2 && operand(1)->sign() == Sign::Negative)) {
    return replaceWith(new Undefined(), true);
  }
  if (operand(0)->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(operand(0));
    // log(0) = undef
    if (r->isZero()) {
      return replaceWith(new Undefined(), true);
    }
    // log(1) = 0;
    if (r->isOne()) {
      return replaceWith(new Rational(0), true);
    }
    Expression * n = splitInteger(r->numerator(), false, context, angleUnit);
    Expression * d = splitInteger(r->denominator(), true, context, angleUnit);
    Addition * a = new Addition(n, d, false);
    replaceWith(a, true);
    return a->shallowSimplify(context, angleUnit);
  }
  return this;
}

Expression * Logarithm::splitInteger(Integer i, bool isDenominator, Context & context, AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(!i.isNegative());
  if (i.isOne()) {
    return new Rational(0);
  }
  assert(!i.isOne());
  if (Arithmetic::k_primorial32.isLowerThan(i)) {
    // We do not want to break i in prime factor because it might be take too many factors... More than k_maxNumberOfPrimeFactors.
    Expression * e = clone();
    e->replaceOperand(operand(0), new Rational(i), true);
    if (!isDenominator) {
      return e;
    }
    Multiplication * m = new Multiplication(new Rational(-1), e, false);
    return m;
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(&i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  Addition * a = new Addition();
  int index = 0;
  while (!coefficients[index].isZero() && index < Arithmetic::k_maxNumberOfPrimeFactors) {
    if (isDenominator) {
      coefficients[index].setNegative(true);
    }
    Expression * e = clone();
    e->replaceOperand(e->operand(0), new Rational(factors[index]), true);
    Multiplication * m = new Multiplication(new Rational(coefficients[index]), e, false);
    const Expression * addNewOperand[1] = {m};
    a->addOperands(addNewOperand, 1);
    m->shallowSimplify(context, angleUnit);
    index++;
  }
  return a;
}

Expression * Logarithm::shallowBeautify(Context & context, AngleUnit angleUnit) {
  Symbol e = Symbol(Ion::Charset::Exponential);
  const Expression * op = operand(0);
  Rational one = Rational(Integer(1));
  if (numberOfOperands() == 2 && (operand(1)->isIdenticalTo(&e) || operand(1)->isIdenticalTo(&one))) {
    detachOperand(op);
    Expression * nl = operand(1)->isIdenticalTo(&e) ? static_cast<Expression *>(new NaperianLogarithm(op, false)) : static_cast<Expression *> (new Logarithm(op, false));
    return replaceWith(nl, true);
  }
  return this;
}

template<typename T>
Evaluation<T> * Logarithm::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  if (numberOfOperands() == 1) {
    return EvaluationEngine::map(this, context, angleUnit, computeOnComplex<T>);
  }
  Evaluation<T> * x = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * n = operand(1)->evaluate<T>(context, angleUnit);
  if (x->numberOfRows() != 1 || x->numberOfColumns() != 1 || n->numberOfRows() != 1 || n->numberOfColumns() != 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Complex<T> result = Division::compute<T>(computeOnComplex(*(n->complexOperand(0)), angleUnit), computeOnComplex(*(x->complexOperand(0)), angleUnit));
  delete x;
  delete n;
  return new Complex<T>(result);
}

ExpressionLayout * Logarithm::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  if (numberOfOperands() == 1) {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, "log");
  }
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new BaselineRelativeLayout(new StringLayout("log", strlen("log")), operand(0)->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[1] = new ParenthesisLayout(operand(1)->createLayout(floatDisplayMode, complexFormat));
  return new HorizontalLayout(childrenLayouts, 2);
}

}
