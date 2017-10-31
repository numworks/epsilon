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
      return replaceWith(new Rational(Integer(0)), true);
    }
    Expression * n = splitInteger(r->numerator(), false, context, angleUnit);
    Expression * d = splitInteger(r->denominator(), true, context, angleUnit);
    const Expression * addOp[2] = {n, d};
    Addition * a = new Addition(addOp, 2, false);
    replaceWith(a, true);
    return a->shallowSimplify(context, angleUnit);
  }
  return this;
}

Expression * Logarithm::splitInteger(Integer i, bool isDenominator, Context & context, AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(!i.isNegative());
  if (i.isOne()) {
    return new Rational(Integer(0));
  }
  assert(!i.isOne());
  if (Arithmetic::k_primorial32.isLowerThan(i)) {
    // We do not want to break i in prime factor because it might be take too many factors... More than k_maxNumberOfPrimeFactors.
    Expression * e = clone();
    e->replaceOperand(operand(0), new Rational(i), true);
    if (!isDenominator) {
      return e;
    }
    const Expression * multOperands[2] = {new Rational(Integer(-1)), e};
    Multiplication * m = new Multiplication(multOperands, 2, false);
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
    const Expression * multOperands[2] = {new Rational(coefficients[index]), e};
    Multiplication * m = new Multiplication(multOperands, 2, false);
    const Expression * addNewOperand[1] = {m};
    a->addOperands(addNewOperand, 1);
    m->shallowSimplify(context, angleUnit);
    index++;
  }
  return a;
}

Expression * Logarithm::shallowBeautify(Context & context, AngleUnit angleUnit) {
  Symbol e = Symbol(Ion::Charset::Exponential);
  const Expression * logOperand[1] = {operand(0)};
  if (numberOfOperands() == 2 && operand(1)->compareTo(&e) == 0) {
    NaperianLogarithm * nl = new NaperianLogarithm(logOperand, true);
    return replaceWith(nl, true);
  }
  Rational one = Rational(Integer(1));
  if (numberOfOperands() == 2 && operand(1)->compareTo(&one) == 0) {
    Logarithm * l = new Logarithm(logOperand, 1, true);
    return replaceWith(l, true);
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
