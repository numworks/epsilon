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

int Power::sign() const {
  if (operand(0)->sign() > 0 && operand(1)->sign() != 0) {
    return 1;
  }
  if (operand(0)->sign() < 0 && operand(1)->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(operand(1));
    if (r->denominator().isOne()) {
      if (Integer::Division(r->numerator(), Integer(2)).remainder.isZero()) {
        return 1;
      } else {
        return -1;
      }
    }
  }
  return 0;
}

Expression * Power::turnIntoPositive() {
  assert(operand(0)->sign() < 0);
  const_cast<Expression *>(operand(0))->turnIntoPositive();
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
  Rational one(Integer(1));
  return operand(1)->compareTo(&one);
}

Expression * Power::immediateSimplify() {
  if (operand(0)->type() == Type::Undefined || operand(1)->type() == Type::Undefined) {
    return replaceWith(new Undefined(), true);
  }
  if (operand(1)->type() == Type::Rational) {
    const Rational * b = static_cast<const Rational *>(operand(1));
    // x^0
    if (b->isZero()) {
      return replaceWith(new Rational(Integer(1)), true);
    }
    // x^1
    if (b->isOne()) {
      return replaceWith(const_cast<Expression *>(operand(0)), true);
    }
  }
  if (operand(0)->type() == Type::Rational) {
    Rational * a = static_cast<Rational *>((Expression *)operand(0));
    // 0^x
    if (a->isZero()) {
      if (operand(1)->sign() > 0) {
        return replaceWith(new Rational(Integer(0)), true);
      }
      if (operand(1)->sign() < 0) {
        return replaceWith(new Undefined(), true);
      }
    }
    // 1^x
    if (a->isOne()) {
      return replaceWith(new Rational(Integer(1)), true);
    }
    // p^q with p, q rationals
    if (operand(1)->type() == Type::Rational) {
      return simplifyRationalRationalPower(this, a, static_cast<Rational *>((Expression *)operand(1)));
    }
  }
  // (a^b)^c -> a^(b+c) if a > 0 or c is integer
  if (operand(0)->type() == Type::Power) {
    Power * p = static_cast<Power *>((Expression *)operand(0));
    // Check is a > 0 or c is Integer
    if (p->operand(0)->sign() > 0 ||
        (operand(1)->type() == Type::Rational && static_cast<Rational *>((Expression *)operand(1))->denominator().isOne())) {
      return simplifyPowerPower(p, const_cast<Expression *>(operand(1)));
    }
  }
  // (a*b*c*...)^r ?
  if (operand(0)->type() == Type::Multiplication) {
    Multiplication * m = static_cast<Multiplication *>((Expression *)operand(0));
    // (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
    if (operand(1)->type() == Type::Rational && static_cast<Rational *>((Expression *)operand(1))->denominator().isOne()) {
      return simplifyPowerMultiplication(m, const_cast<Expression *>(operand(1)));
    }
    // (a*b*...)^r -> |a|^r*(sign(a)*b*)^r if a rational
    for (int i = 0; i < m->numberOfOperands(); i++) {
      if (m->operand(i)->sign() > 0 || m->operand(i)->type() == Type::Rational) {
        Expression * r = const_cast<Expression *>(operand(1));
        Expression * rCopy = r->clone();
        Expression * factor = const_cast<Expression *>(m->operand(0));
        if (factor->sign() < 0) {
          m->replaceOperand(factor, new Rational(Integer(-1)), false);
          static_cast<Rational *>(factor)->setNegative(false);
        } else {
          m->removeOperand(factor, false);
        }
        m->immediateSimplify();
        const Expression * powOperands[2] = {factor, rCopy};
        Power * p = new Power(powOperands, false);
        const Expression * multOperands[2] = {p, clone()};
        Multiplication * root = new Multiplication(multOperands, 2, false);
        p->immediateSimplify();
        const_cast<Expression *>(root->operand(1))->immediateSimplify();
        replaceWith(root, true);
        return root->immediateSimplify();
      }
    }
  }
  // a^(b+c) -> Rational(a^b)*a^c with a and b rational
  if (operand(0)->type() == Type::Rational && operand(1)->type() == Type::Addition) {
    Addition * a = static_cast<Addition *>((Expression *)operand(1));
    // Check is b is rational
    if (a->operand(0)->type() == Type::Rational) {
      Power * p1 = static_cast<Power *>(clone());
      replaceOperand(a, const_cast<Expression *>(a->operand(1)), true);
      Power * p2 = static_cast<Power *>(clone());
      const Expression * multOperands[2] = {p1, p2};
      Multiplication * m = new Multiplication(multOperands, 2, false);
      simplifyRationalRationalPower(p1, static_cast<Rational *>((Expression *)p1->operand(0)), static_cast<Rational *>((Expression *)(p1->operand(1)->operand(0))));
      replaceWith(m, true);
      return m->immediateSimplify();
    }
  }
  return this;
}

Expression * Power::simplifyPowerPower(Power * p, Expression * e) {
  Expression * p0 = const_cast<Expression *>(p->operand(0));
  Expression * p1 = const_cast<Expression *>(p->operand(1));
  p->detachOperands();
  const Expression * multOperands[2] = {p1, e};
  Multiplication * m = new Multiplication(multOperands, 2, false);
  replaceOperand(e, m, false);
  replaceOperand(p, p0, true);
  m->immediateSimplify();
  return immediateSimplify();
}

Expression * Power::simplifyPowerMultiplication(Multiplication * m, Expression * r) {
  for (int index = 0; index < m->numberOfOperands(); index++) {
    Expression * factor = const_cast<Expression *>(m->operand(index));
    const Expression * powOperands[2] = {factor, r};
    Power * p = new Power(powOperands, true); // We copy r and factor to avoid inheritance issues
    m->replaceOperand(factor, p, true);
    p->immediateSimplify();
  }
  detachOperand(m);
  Expression * newExpression = replaceWith(m, true); // delete r
  m->immediateSimplify();
  return newExpression;
}

Expression * Power::simplifyRationalRationalPower(Expression * result, Rational * a, Rational * b) {
  if (b->denominator().isOne()) {
    Rational r = Rational::Power(*a, b->numerator());
    return result->replaceWith(new Rational(r),true);
  }
  Expression * n = nullptr;
  Expression * d = nullptr;
  if (b->sign() < 0) {
    b->setNegative(false);
    n = CreateSimplifiedIntegerRationalPower(a->denominator(), b, false);
    d = CreateSimplifiedIntegerRationalPower(a->numerator(), b, true);
  } else {
    n = CreateSimplifiedIntegerRationalPower(a->numerator(), b, false);
    d = CreateSimplifiedIntegerRationalPower(a->denominator(), b, true);
  }
  const Expression * multOp[2] = {n, d};
  Multiplication * m = new Multiplication(multOp, 2, false);
  Expression * newExpression = result->replaceWith(m, true);
  m->immediateSimplify();
  return newExpression;
}

Expression * Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational * r, bool isDenominator) {
  assert(!i.isZero());
  assert(r->sign() > 0);
  if (i.isOne()) {
    return new Rational(Integer(1));
  }
  if (Arithmetic::k_primorial32.isLowerThan(i)) {
    r->setNegative(isDenominator);
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
  const Expression * powerOperands[2] = {p1, p2};
  Power * p = new Power(powerOperands, false);
  if (r1.isEqualTo(Integer(1)) && !i.isNegative()) {
    return p;
  }
  Rational * r3 = isDenominator ? new Rational(Integer(1), r1) : new Rational(r1);
  const Expression * multOp[2] = {r3, p};
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

Expression * Power::immediateBeautify() {
  // X^-y -> 1/(X->immediateBeautify)^y
  if (operand(1)->sign() < 0) {
    Expression * p = createDenominator();
    const Expression * divOperands[2] = {new Rational(Integer(1)), p};
    Division * d = new Division(divOperands, false);
    p->immediateSimplify();
    replaceWith(d, true);
    return d->immediateBeautify();
  }
  if (operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(1))->numerator().isOne()) {
    Integer index = static_cast<const Rational *>(operand(1))->denominator();
    Integer two = Integer(2);
    if (index.compareTo(&two) == 0) {
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

Expression * Power::createDenominator() {
  if (operand(1)->sign() < 0) {
    Expression * denominator = clone();
    Expression * newExponent = const_cast<Expression *>(denominator->operand(1))->turnIntoPositive();
    if (newExponent->type() == Type::Rational && static_cast<Rational *>(newExponent)->isOne()) {
      delete denominator;
      return operand(0)->clone();
    }
    return denominator;
  }
  return nullptr;
}

}
