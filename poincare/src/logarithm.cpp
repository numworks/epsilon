#include <poincare/logarithm.h>
#include <poincare/division.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/symbol.h>
#include <poincare/arithmetic.h>
#include <poincare/power.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/approximation_engine.h>
#include <poincare/simplification_engine.h>
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

Expression * Logarithm::simpleShallowReduce(Context & context, AngleUnit angleUnit) {
  Expression * op = editableOperand(0);
  if (op->sign() == Sign::Negative || (numberOfOperands() == 2 && operand(1)->sign() == Sign::Negative)) {
    return replaceWith(new Undefined(), true);
  }
  // log(x,x)->1
  if (numberOfOperands() == 2 && op->isIdenticalTo(operand(1))) {
    return replaceWith(new Rational(1), true);
  }
  if (op->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(operand(0));
    // log(0) = undef
    if (r->isZero()) {
      return replaceWith(new Undefined(), true);
    }
    // log(1) = 0;
    if (r->isOne()) {
      return replaceWith(new Rational(0), true);
    }
    // log(10) ->1
    if (numberOfOperands() == 1 && r->isTen()) {
      return replaceWith(new Rational(1), true);
    }
  }
  return this;
}

Expression * Logarithm::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
#if MATRIX_EXACT_REDUCING
  if (numberOfOperands() == 1 && op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
  if (numberOfOperands() == 2 && (op->type() == Type::Matrix || operand(1)->type() == Type::Matrix)) {
    return replaceWith(new Undefined(), true);
  }
#endif
  Expression * f = simpleShallowReduce(context, angleUnit);
  if (f != this) {
    return f;
  }

  /* We do not apply some rules if the parent node is a power of b. In this
   * case there is a simplication of form e^ln(3^(1/2))->3^(1/2) */
  bool letLogAtRoot = parentIsAPowerOfSameBase();
  // log(x^y, b)->y*log(x, b) if x>0
  if (!letLogAtRoot && op->type() == Type::Power && op->operand(0)->sign() == Sign::Positive) {
    Power * p = static_cast<Power *>(op);
    Expression * x = p->editableOperand(0);
    Expression * y = p->editableOperand(1);
    p->detachOperands();
    replaceOperand(p, x, true);
    Expression * newLog = shallowReduce(context, angleUnit);
    newLog = newLog->replaceWith(new Multiplication(y, newLog->clone(), false), true);
    return newLog->shallowReduce(context, angleUnit);
  }
  // log(x*y, b)->log(x,b)+log(y, b) if x,y>0
  if (!letLogAtRoot && op->type() == Type::Multiplication) {
    Addition * a = new Addition();
    for (int i = 0; i<op->numberOfOperands()-1; i++) {
      Expression * factor = op->editableOperand(i);
      if (factor->sign() == Sign::Positive) {
        Expression * newLog = clone();
        static_cast<Multiplication *>(op)->removeOperand(factor, false);
        newLog->replaceOperand(newLog->editableOperand(0), factor, true);
        a->addOperand(newLog);
        newLog->shallowReduce(context, angleUnit);
      }
    }
    if (a->numberOfOperands() > 0) {
      op->shallowReduce(context, angleUnit);
      Expression * reducedLastLog = shallowReduce(context, angleUnit);
      reducedLastLog->replaceWith(a, false);
      a->addOperand(reducedLastLog);
      return a->shallowReduce(context, angleUnit);
    } else {
      delete a;
    }
  }
  // log(r) = a0log(p0)+a1log(p1)+... with r = p0^a0*p1^a1*... (Prime decomposition)
  if (!letLogAtRoot && op->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(operand(0));
    Expression * n = splitInteger(r->numerator(), false, context, angleUnit);
    Expression * d = splitInteger(r->denominator(), true, context, angleUnit);
    Addition * a = new Addition(n, d, false);
    replaceWith(a, true);
    return a->shallowReduce(context, angleUnit);
  }
  return this;
}

bool Logarithm::parentIsAPowerOfSameBase() const {
  // We look for expressions of types e^ln(x) or e^(ln(x)) where ln is this
  const Expression * parentExpression = parent();
  bool thisIsPowerExponent = parentExpression->type() == Type::Power ? parentExpression->operand(1) == this : false;
  if (parentExpression->type() == Type::Parenthesis) {
    const Expression * parentParentExpression = parentExpression->parent();
    if (parentExpression == nullptr) {
      return false;
    }
    thisIsPowerExponent = parentParentExpression->type() == Type::Power ? parentParentExpression->operand(1) == parentExpression : false;
    parentExpression = parentParentExpression;
  }
  if (thisIsPowerExponent) {
    const Expression * powerOperand0 = parentExpression->operand(0);
    if (numberOfOperands() == 1) {
      if (powerOperand0->type() == Type::Rational && static_cast<const Rational *>(powerOperand0)->isTen()) {
        return true;
      }
    }
    if (numberOfOperands() == 2) {
      if (powerOperand0->isIdenticalTo(operand(1))){
        return true;
      }
    }
  }
  return false;
}

Expression * Logarithm::splitInteger(Integer i, bool isDenominator, Context & context, AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(!i.isNegative());
  if (i.isOne()) {
    return new Rational(0);
  }
  assert(!i.isOne());
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(&i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (coefficients[0].isMinusOne()) {
    /* We could not break i in prime factor (either it might take too many
     * factors or too much time). */
    Expression * e = clone();
    e->replaceOperand(e->operand(0), new Rational(i), true);
    if (!isDenominator) {
      return e;
    }
    Multiplication * m = new Multiplication(new Rational(-1), e, false);
    return m;
  }
  Addition * a = new Addition();
  int index = 0;
  while (!coefficients[index].isZero() && index < Arithmetic::k_maxNumberOfPrimeFactors) {
    if (isDenominator) {
      coefficients[index].setNegative(true);
    }
    Expression * e = clone();
    e->replaceOperand(e->operand(0), new Rational(factors[index]), true);
    Multiplication * m = new Multiplication(new Rational(coefficients[index]), e, false);
    static_cast<Logarithm *>(e)->simpleShallowReduce(context, angleUnit);
    a->addOperand(m);
    m->shallowReduce(context, angleUnit);
    index++;
  }
  return a;
}

Expression * Logarithm::shallowBeautify(Context & context, AngleUnit angleUnit) {
  Symbol e = Symbol(Ion::Charset::Exponential);
  const Expression * op = operand(0);
  Rational one(1);
  if (numberOfOperands() == 2 && (operand(1)->isIdenticalTo(&e) || operand(1)->isIdenticalTo(&one))) {
    detachOperand(op);
    Expression * nl = operand(1)->isIdenticalTo(&e) ? static_cast<Expression *>(new NaperianLogarithm(op, false)) : static_cast<Expression *> (new Logarithm(op, false));
    return replaceWith(nl, true);
  }
  return this;
}

template<typename T>
Expression * Logarithm::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  if (numberOfOperands() == 1) {
    return ApproximationEngine::map(this, context, angleUnit, computeOnComplex<T>);
  }
  Expression * x = operand(0)->approximate<T>(context, angleUnit);
  Expression * n = operand(1)->approximate<T>(context, angleUnit);
  Complex<T> result = Complex<T>::Float(NAN);
  if (x->type() == Type::Complex && n->type() == Type::Complex) {
    Complex<T> * xc = static_cast<Complex<T> *>(x);
    Complex<T> * nc = static_cast<Complex<T> *>(n);
    result = Division::compute<T>(computeOnComplex(*xc, angleUnit), computeOnComplex(*nc, angleUnit));
  }
  delete x;
  delete n;
  return new Complex<T>(result);
}

ExpressionLayout * Logarithm::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  if (numberOfOperands() == 1) {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, "log");
  }
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new BaselineRelativeLayout(new StringLayout("log", strlen("log")), operand(1)->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[1] = new ParenthesisLayout(operand(0)->createLayout(floatDisplayMode, complexFormat));
  return new HorizontalLayout(childrenLayouts, 2);
}

}
