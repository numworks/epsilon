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
#include <poincare/undefined.h>
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

bool Power::isPositive() const {
  if (operand(1)->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(operand(1));
    if (r->denominator().isOne() && Integer::Division(r->numerator(), Integer(2)).remainder.isZero()) {
      return true;
    }
  }
  return false;
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
    indiceOperand = (Expression *)m_operands[1]->operand(0);
  }
  return new BaselineRelativeLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat),indiceOperand->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Superscript);
}

int Power::compareToSameTypeExpression(const Expression * e) const {
  int baseComparison = operand(0)->compareTo(static_cast<const Power *>(e)->operand(0));
  if (baseComparison != 0) {
    return baseComparison;
  }
  return operand(1)->compareTo(static_cast<const Power *>(e)->operand(1));
}

int Power::compareToGreaterTypeExpression(const Expression * e) const {
  int baseComparison = operand(0)->compareTo(e);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Integer one(1);
  return operand(1)->compareTo(&one);
}

void Power::immediateSimplify() {
  if (operand(0)->type() == Type::Undefined || operand(1)->type() == Type::Undefined) {
    replaceWith(new Undefined(), true);
    return;
  }
  if (operand(0)->type() == Type::Rational) {
    const Rational * a = static_cast<const Rational *>(operand(0));
    // 0^x
    if (a->isZero()) {
      if (operand(1)->type() == Type::Rational) {
        const Rational * b = static_cast<const Rational *>(operand(1));
        if (!b->isNegative()) {
          replaceWith(new Rational(Integer(0)), true);
          return;
        } else {
          replaceWith(new Undefined(), true);
          return;
        }
      }
    }
    // 1^x
    if (a->isOne()) {
      replaceWith(new Rational(Integer(1)), true);
      return;
    }
  }
  if (operand(1)->type() == Type::Rational) {
    Rational * b = static_cast<Rational *>((Expression *)operand(1));
    // x^0
    if (b->isZero()) {
      replaceWith(new Rational(Integer(0)), true);
      return;
    }
    // x^1
    if (b->isOne()) {
      replaceWith(new Rational(Integer(1), true));
      return;
    }
    /* a^n with n Integer */
    if (b->denominator().isOne()) {
      if (operand(0)->type() == Type::Rational) {
        Rational * e = static_cast<Rational *>((Expression *)operand(0));
        Rational r = Rational::Power(*(e), b->numerator());
        replaceWith(new Rational(r),true);
        return;
      } else if (operand(0)->type() == Type::Multiplication) {
        Multiplication * e = static_cast<Multiplication *>((Expression *)operand(0));
        simplifyPowerMultiplication(e, b);
        return;
      }
    }
    // p^q with p, q rationals
    if (operand(0)->type() == Type::Rational) {
      simplifyRationalRationalPower(static_cast<Rational *>((Expression *)operand(0)), b);
      return;
    }
  }
  // (a^b)^c -> a^(b+c) if a > 0 or c is integer
  if (operand(0)->type() == Type::Power) {
     Power * p = static_cast<Power *>((Expression *)operand(0));
     // Check is a > 0 or c is Integer
     if (p->operand(0)->isPositive() ||
         (operand(1)->type() == Type::Rational && static_cast<Rational *>((Expression *)operand(1))->denominator().isOne())) {
       simplifyPowerPower(p, const_cast<Expression *>(operand(1)));
       return;
     }
  }
  // TODO: (a*b)^c -> |a|^c*(sign(a)*b)^c
}

void Power::simplifyPowerPower(Power * p, Expression * e) {
  Expression * p0 = const_cast<Expression *>(p->operand(0));
  Expression * p1 = const_cast<Expression *>(p->operand(1));
  p->detachOperands();
  const Expression * multOperands[2] = {p1, e};
  Multiplication * m = new Multiplication(multOperands, 2, false);
  replaceOperand(e, m, false);
  replaceOperand(p, p0, true);
  m->immediateSimplify();
  immediateSimplify();
}

void Power::simplifyPowerMultiplication(Multiplication * m, Rational * r) {
  for (int index = 0; index < m->numberOfOperands(); index++) {
    Expression * rCopy = r->clone();
    Expression * factor = const_cast<Expression *>(m->operand(index));
    const Expression * powOperands[2] = {factor, rCopy};
    Power * p = new Power(powOperands, false);
    m->replaceOperand(factor, p, false);
    p->immediateSimplify();
  }
  detachOperand(m);
  replaceWith(m, true); // delete r
  m->immediateSimplify();
}

void Power::simplifyRationalRationalPower(Rational * a, Rational * b) {
  Expression * n = CreateSimplifiedIntegerRationalPower(a->numerator(), b);
  Expression * d = CreateSimplifiedIntegerRationalPower(a->denominator(), b);
  Rational * minusOne = new Rational(Integer(-1));
  const Expression * powOp[2] = {d, minusOne};
  Power * p = new Power(powOp, false);
  const Expression * multOp[2] = {n, p};
  Multiplication * m = new Multiplication(multOp, 2, false);
  if (d->type() == Type::Rational && static_cast<Rational *>(d)->isOne()) {
    p->replaceWith(new Rational(Integer(1)), true);
  } else if (d->type() == Type::Multiplication) {
    p->simplifyPowerMultiplication(static_cast<Multiplication *>(d), minusOne);
  }
  replaceWith(m, true);
  m->immediateSimplify();
}

Expression * Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational * r) {
  assert(!i.isZero());
  if (i.isOne()) {
    return new Rational(Integer(1));
  }
  if (Arithmetic::k_primorial32.isLowerThan(i)) {
    const Expression * powOp[2] = {new Rational(i), r->clone()};
    // We do not want to break i in prime factor because it might be take too many factors... More than k_maxNumberOfPrimeFactors.
    return new Power(powOp, false);
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(&i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);

  Integer r1 = Integer(1);
  Integer r2 = Integer(1);
  int index = 0;
  while (!coefficients[index].isZero()) {
    Integer n = Integer::Multiplication(coefficients[index], r->numerator());
    IntegerDivision div = Integer::Division(n, r->denominator());
    r1 = Integer::Multiplication(r1, Integer::Power(factors[index], div.quotient));
    r2 = Integer::Multiplication(r2, Integer::Power(factors[index], div.remainder));
    index++;
  }
  Rational * p1 = new Rational(r2);
  Rational * p2 = new Rational(Integer(1), r->denominator());
  const Expression * powerOperands[2] = {p1, p2};
  Power * p = new Power(powerOperands, false);
  if (r1.isEqualTo(Integer(1)) && !i.isNegative()) {
    return p;
  }
  const Expression * multOp[2] = {new Rational(r1), p};
  Multiplication * m = new Multiplication(multOp, 2, false);
  if (r2.isOne()) {
    m->removeOperand(p);
  }
  if (i.isNegative()) {
    const Symbol * exp = new Symbol(Ion::Charset::Exponential);
    const Symbol * iComplex = new Symbol(Ion::Charset::IComplex);
    const Symbol * pi = new Symbol(Ion::Charset::SmallPi);
    const Expression * multExpOperands[3] = {iComplex, pi, r->clone()};
    Multiplication * mExp = new Multiplication(multExpOperands, 3, false);
    const Expression * powOperands[2] = {exp, mExp};
    const Power * pExp = new Power(powOperands, false);
    const Expression * operand[1] = {pExp};
    m->addOperands(operand, 1);
  }
  m->sortChildren();
  return m;
}

}
