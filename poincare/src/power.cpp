extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <math.h>
#include <poincare/complex_matrix.h>
#include <poincare/power.h>
#include <poincare/opposite.h>
#include <poincare/undefined.h>
#include "layout/baseline_relative_layout.h"

namespace Poincare {

Expression::Type Power::type() const {
  return Type::Power;
}

Expression * Power::clone() const {
  return new Power(m_operands, true);
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
    if (a->isOne()) {
      replaceWith(new Rational(Integer(1)), true);
    }
  }
  if (operand(1)->type() == Type::Rational) {
    const Rational * b = static_cast<const Rational *>(operand(1));
    simplifyRationalPower(const_cast<Expression *>(operand(0)), const_cast<Rational *>(b));
  }
}

void Power::simplifyRationalPower(Expression * e, Rational * b) {
  if (b->isZero()) {
    replaceWith(new Rational(Integer(0)), true);
    return;
  }
  if (b->isOne()) {
    replaceWith(new Rational(Integer(1), true));
    return;
  }
  /* a^n with n Integer */
  if (b->denominator().isOne()) {
    if (e->type() == Type::Rational) {
      Rational r = Rational::Power(*(static_cast<Rational *>(e)), b->numerator());
      replaceWith(new Rational(r),true);
      return;
    } else if (e->type() == Type::Power) {
      return simplifyPowerPower(static_cast<Power *>(e), b);
    } else if (e->type() == Type::Multiplication) {
      return simplifyPowerMultiplication(static_cast<Multiplication *>(e), b);
    }
  }
  if (e->type() == Type::Rational) {
    return simplifyRationalRationalPower(static_cast<Rational *>((Expression *)operand(0)), b);
  }
}

void Power::simplifyPowerPower(Power * p, Rational * r) {
  const Expression * multOperands[2] = {const_cast<Expression *>(p->operand(1)), r};
  Multiplication * m = new Multiplication(multOperands, 2, false);
  replaceOperand(r, m, false);
  m->immediateSimplify();
  if (operand(1)->type() == Type::Rational) {
    simplifyRationalPower(const_cast<Expression *>(operand(0)), static_cast<Rational *>((Expression *)operand(1)));
  }
}

void Power::simplifyPowerMultiplication(Multiplication * m, Rational * r) {
  for (int index = 0; index < m->numberOfOperands(); index++) {
    Expression * rCopy = r->clone();
    Expression * factor = const_cast<Expression *>(m->operand(index));
    const Expression * powOperands[2] = {factor, rCopy};
    Power * p = new Power(powOperands, false);
    m->replaceOperand(factor, p, false);
    p->simplifyRationalPower(const_cast<Expression *>(p->operand(0)), static_cast<Rational *>(rCopy));
  }
  detachOperand(m);
  replaceWith(m, true); // delete r
  m->immediateSimplify();
}

void Power::simplifyRationalRationalPower(Rational * a, Rational * b) {
  /*const Expression * n = SimplifyIntegerRationalPower(a->numerator(), b);
  const Expression * d = SimplifyIntegerRationalPower(a->denominator(), b);
  const Expression * multOp[2] = {n, d};
  Multiplication * m = new Multiplication(multOp, 2, false);
  replaceWith(m, true);
  m->immediateSimplify();*/
}

/*Expression * Power::SimplifyIntegerRationalPower(Integer i, Rational * r) {
// TODO
}*/

}
