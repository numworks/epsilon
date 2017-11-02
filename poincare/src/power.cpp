extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <math.h>
#include <ion.h>
#include <poincare/complex_matrix.h>
#include <poincare/power.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/addition.h>
#include <poincare/undefined.h>
#include <poincare/square_root.h>
#include <poincare/nth_root.h>
#include <poincare/division.h>
#include <poincare/arithmetic.h>
#include <poincare/symbol.h>
#include "layout/baseline_relative_layout.h"

namespace Poincare {

Expression::Type Power::type() const {
  return Type::Power;
}

Expression * Power::clone() const {
  return new Power(m_operands, true);
}

Expression::Sign Power::sign() const {
  if (operand(0)->sign() == Sign::Positive && operand(1)->sign() != Sign::Unknown) {
    return Sign::Positive;
  }
  if (operand(0)->sign() == Sign::Negative && operand(1)->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(operand(1));
    if (r->denominator().isOne()) {
      if (Integer::Division(r->numerator(), Integer(2)).remainder.isZero()) {
        return Sign::Positive;
      } else {
        return Sign::Negative;
      }
    }
  }
  return Sign::Unknown;
}

Expression * Power::setSign(Sign s, Context & context, AngleUnit angleUnit) {
  assert(s == Sign::Positive);
  assert(operand(0)->sign() == Sign::Negative);
  editableOperand(0)->setSign(Sign::Positive, context, angleUnit);
  return this;
}

template<typename T>
Complex<T> Power::compute(const Complex<T> c, const Complex<T> d) {
  if (d.b() != 0) {
    /* First case c and d is complex */
    if (c.b() != 0 || c.a() <= 0) {
      return Complex<T>::Float(NAN);
    }
    /* Second case only d is complex */
    T radius = std::pow(c.a(), d.a());
    T theta = d.b()*std::log(c.a());
    return Complex<T>::Polar(radius, theta);
  }
  /* Third case only c is complex */
  T radius = std::pow(c.r(), d.a());
  T theta = d.a()*c.th();
  return Complex<T>::Polar(radius, theta);
}

template<typename T> Evaluation<T> * Power::computeOnMatrixAndComplex(Evaluation<T> * m, const Complex<T> * d) {
 if (m->numberOfRows() != m->numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T power = d->toScalar();
  if (isnan(power) || isinf(power) || power != (int)power || std::fabs(power) > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  if (power < 0) {
    Evaluation<T> * inverse = m->createInverse();
    Complex<T> minusC = Opposite::compute(*d, AngleUnit::Default);
    Evaluation<T> * result = Power::computeOnMatrixAndComplex(inverse, &minusC);
    delete inverse;
    return result;
  }
  Evaluation<T> * result = ComplexMatrix<T>::createIdentity(m->numberOfRows());
  // TODO: implement a quick exponentiation
  for (int k = 0; k < (int)power; k++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex<T>(Complex<T>::Float(NAN));
    }
    result = Multiplication::computeOnMatrices(result, m);
  }
  return result;
}

template<typename T> Evaluation<T> * Power::computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n) {
  return new Complex<T>(Complex<T>::Float(NAN));
}

template<typename T> Evaluation<T> * Power::computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {

  return new Complex<T>(Complex<T>::Float(NAN));
}

ExpressionLayout * Power::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  const Expression * indiceOperand = m_operands[1];
  // Delete eventual parentheses of the indice in the pretty print
  if (m_operands[1]->type() == Type::Parenthesis) {
    indiceOperand = m_operands[1]->operand(0);
  }
  return new BaselineRelativeLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat),indiceOperand->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Superscript);
}

int Power::simplificationOrderSameType(const Expression * e) const {
  int baseComparison = SimplificationOrder(operand(0), e->operand(0));
  if (baseComparison != 0) {
    return baseComparison;
  }
  return SimplificationOrder(operand(1), e->operand(1));
}

int Power::simplificationOrderGreaterType(const Expression * e) const {
  int baseComparison = SimplificationOrder(operand(0), e);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Rational one(Integer(1));
  return SimplificationOrder(operand(1), &one);
}

Expression * Power::shallowSimplify(Context& context, AngleUnit angleUnit) {
  if (operand(1)->type() == Type::Rational) {
    const Rational * b = static_cast<const Rational *>(operand(1));
    // x^0
    if (b->isZero()) {
      return replaceWith(new Rational(Integer(1)), true);
    }
    // x^1
    if (b->isOne()) {
      return replaceWith(editableOperand(0), true);
    }
  }
  if (operand(0)->type() == Type::Rational) {
    Rational * a = static_cast<Rational *>(editableOperand(0));
    // 0^x
    if (a->isZero()) {
      if (operand(1)->sign() == Sign::Positive) {
        return replaceWith(new Rational(Integer(0)), true);
      }
      if (operand(1)->sign() == Sign::Negative) {
        return replaceWith(new Undefined(), true);
      }
    }
    // 1^x
    if (a->isOne()) {
      return replaceWith(new Rational(Integer(1)), true);
    }
    // p^q with p, q rationals
    if (operand(1)->type() == Type::Rational) {
      return simplifyRationalRationalPower(this, a, static_cast<Rational *>(editableOperand(1)), context, angleUnit);
    }
  }
  // (a^b)^c -> a^(b+c) if a > 0 or c is integer
  if (operand(0)->type() == Type::Power) {
    Power * p = static_cast<Power *>(editableOperand(0));
    // Check is a > 0 or c is Integer
    if (p->operand(0)->sign() == Sign::Positive ||
        (operand(1)->type() == Type::Rational && static_cast<Rational *>(editableOperand(1))->denominator().isOne())) {
      return simplifyPowerPower(p, editableOperand(1), context, angleUnit);
    }
  }
  // (a*b*c*...)^r ?
  if (operand(0)->type() == Type::Multiplication) {
    Multiplication * m = static_cast<Multiplication *>(editableOperand(0));
    // (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
    if (operand(1)->type() == Type::Rational && static_cast<Rational *>(editableOperand(1))->denominator().isOne()) {
      return simplifyPowerMultiplication(m, editableOperand(1), context, angleUnit);
    }
    // (a*b*...)^r -> |a|^r*(sign(a)*b*)^r if a rational
    for (int i = 0; i < m->numberOfOperands(); i++) {
      if (m->operand(i)->sign() == Sign::Positive || m->operand(i)->type() == Type::Rational) {
        Expression * r = editableOperand(1);
        Expression * rCopy = r->clone();
        Expression * factor = m->editableOperand(i);
        if (factor->sign() == Sign::Negative) {
          m->replaceOperand(factor, new Rational(Integer(-1)), false);
          factor->setSign(Sign::Positive, context, angleUnit);
        } else {
          m->removeOperand(factor, false);
        }
        m->shallowSimplify(context, angleUnit);
        Power * p = new Power(factor, rCopy, false);
        Multiplication * root = new Multiplication(p, clone(), false);
        p->shallowSimplify(context, angleUnit);
        root->editableOperand(1)->shallowSimplify(context, angleUnit);
        replaceWith(root, true);
        return root->shallowSimplify(context, angleUnit);
      }
    }
  }
  // a^(b+c) -> Rational(a^b)*a^c with a and b rational
  if (operand(0)->type() == Type::Rational && operand(1)->type() == Type::Addition) {
    Addition * a = static_cast<Addition *>(editableOperand(1));
    // Check is b is rational
    if (a->operand(0)->type() == Type::Rational) {
      Power * p1 = static_cast<Power *>(clone());
      replaceOperand(a, a->editableOperand(1), true);
      Power * p2 = static_cast<Power *>(clone());
      Multiplication * m = new Multiplication(p1, p2, false);
      simplifyRationalRationalPower(p1, static_cast<Rational *>(p1->editableOperand(0)), static_cast<Rational *>(p1->editableOperand(1)->editableOperand(0)), context, angleUnit);
      replaceWith(m, true);
      return m->shallowSimplify(context, angleUnit);
    }
  }
  return this;
}

Expression * Power::simplifyPowerPower(Power * p, Expression * e, Context& context, AngleUnit angleUnit) {
  Expression * p0 = p->editableOperand(0);
  Expression * p1 = p->editableOperand(1);
  p->detachOperands();
  Multiplication * m = new Multiplication(p1, e, false);
  replaceOperand(e, m, false);
  replaceOperand(p, p0, true);
  m->shallowSimplify(context, angleUnit);
  return shallowSimplify(context, angleUnit);
}

Expression * Power::simplifyPowerMultiplication(Multiplication * m, Expression * r, Context& context, AngleUnit angleUnit) {
  for (int index = 0; index < m->numberOfOperands(); index++) {
    Expression * factor = m->editableOperand(index);
    Power * p = new Power(factor, r, true); // We copy r and factor to avoid inheritance issues
    m->replaceOperand(factor, p, true);
    p->shallowSimplify(context, angleUnit);
  }
  detachOperand(m);
  return replaceWith(m, true)->shallowSimplify(context, angleUnit); // delete r
}

Expression * Power::simplifyRationalRationalPower(Expression * result, Rational * a, Rational * b, Context& context, AngleUnit angleUnit) {
  if (b->denominator().isOne()) {
    Rational r = Rational::Power(*a, b->numerator());
    return result->replaceWith(new Rational(r),true);
  }
  Expression * n = nullptr;
  Expression * d = nullptr;
  if (b->sign() == Sign::Negative) {
    b->setSign(Sign::Positive);
    n = CreateSimplifiedIntegerRationalPower(a->denominator(), b, false);
    d = CreateSimplifiedIntegerRationalPower(a->numerator(), b, true);
  } else {
    n = CreateSimplifiedIntegerRationalPower(a->numerator(), b, false);
    d = CreateSimplifiedIntegerRationalPower(a->denominator(), b, true);
  }
  Multiplication * m = new Multiplication(n, d, false);
  result->replaceWith(m, true);
  return m->shallowSimplify(context, angleUnit);
}

Expression * Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational * r, bool isDenominator) {
  assert(!i.isZero());
  assert(r->sign() == Sign::Positive);
  if (i.isOne()) {
    return new Rational(Integer(1));
  }
  if (Arithmetic::k_primorial32.isLowerThan(i)) {
    r->setSign(isDenominator ? Sign::Negative : Sign::Positive);
    // We do not want to break i in prime factor because it might be take too many factors... More than k_maxNumberOfPrimeFactors.
    return new Power(new Rational(i), r->clone(), false);
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(&i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);

  Integer r1 = Integer(1);
  Integer r2 = Integer(1);
  int index = 0;
  while (!coefficients[index].isZero() && index < Arithmetic::k_maxNumberOfPrimeFactors) {
    Integer n = Integer::Multiplication(coefficients[index], r->numerator());
    IntegerDivision div = Integer::Division(n, r->denominator());
    r1 = Integer::Multiplication(r1, Integer::Power(factors[index], div.quotient));
    r2 = Integer::Multiplication(r2, Integer::Power(factors[index], div.remainder));
    index++;
  }
  Rational * p1 = new Rational(r2);
  Integer one = isDenominator ? Integer(-1) : Integer(1);
  Rational * p2 = new Rational(one, r->denominator());
  Power * p = new Power(p1, p2, false);
  if (r1.isEqualTo(Integer(1)) && !i.isNegative()) {
    return p;
  }
  Rational * r3 = isDenominator ? new Rational(Integer(1), r1) : new Rational(r1);
  Multiplication * m = new Multiplication(r3, p, false);
  if (r2.isOne()) {
    m->removeOperand(p);
  }
  if (i.isNegative()) {
    const Symbol * exp = new Symbol(Ion::Charset::Exponential);
    const Symbol * iComplex = new Symbol(Ion::Charset::IComplex);
    const Symbol * pi = new Symbol(Ion::Charset::SmallPi);
    const Expression * multExpOperands[3] = {iComplex, pi, r->clone()};
    Multiplication * mExp = new Multiplication(multExpOperands, 3, false);
    const Power * pExp = new Power(exp, mExp, false);
    const Expression * operand[1] = {pExp};
    m->addOperands(operand, 1);
  }
  m->sortOperands(SimplificationOrder);
  return m;
}

Expression * Power::shallowBeautify(Context& context, AngleUnit angleUnit) {
  // X^-y -> 1/(X->shallowBeautify)^y
  if (operand(1)->sign() == Sign::Negative) {
    Expression * p = cloneDenominator(context, angleUnit);
    Division * d = new Division(new Rational(Integer(1)), p, false);
    p->shallowSimplify(context, angleUnit);
    replaceWith(d, true);
    return d->shallowBeautify(context, angleUnit);
  }
  if (operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(1))->numerator().isOne()) {
    Integer index = static_cast<const Rational *>(operand(1))->denominator();
    if (index.isEqualTo(Integer(2))) {
      const Expression * sqrtOperand[1] = {operand(0)};
      SquareRoot * sqr = new SquareRoot(sqrtOperand, true);
      return replaceWith(sqr, true);
    }
    const Expression * rootOperand[2] = {operand(0)->clone(), new Rational(index)};
    NthRoot * nr = new NthRoot(rootOperand, false);
    return replaceWith(nr, true);
  }
  // +(a,b)^c ->(+(a,b))^c
  if (operand(0)->type() == Type::Addition || operand(0)->type() == Type::Multiplication) {
    const Expression * o[1] = {operand(0)};
    Parenthesis * p = new Parenthesis(o, true);
    replaceOperand(operand(0), p, true);
  }
  return this;
}

Expression * Power::cloneDenominator(Context & context, AngleUnit angleUnit) const {
  if (operand(1)->sign() == Sign::Negative) {
    Expression * denominator = clone();
    Expression * newExponent = denominator->editableOperand(1)->setSign(Sign::Positive, context, angleUnit);
    if (newExponent->type() == Type::Rational && static_cast<Rational *>(newExponent)->isOne()) {
      delete denominator;
      return operand(0)->clone();
    }
    return denominator;
  }
  return nullptr;
}

}
