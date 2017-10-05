#include <poincare/rational.h>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
}
#include <poincare/arithmetic.h>
#include <poincare/complex.h>
#include "layout/string_layout.h"
#include "layout/fraction_layout.h"

namespace Poincare {

// Constructors

Rational::Rational(const Integer numerator, const Integer denominator) {
  assert(!denominator.isZero());
  Integer gcd = Arithmetic::GCD(&numerator, &denominator);
  m_numerator = Integer::Division(numerator, gcd).quotient;
  m_denominator = Integer::Division(denominator, gcd).quotient;
  if (m_numerator.isNegative() && m_denominator.isNegative()) {
    m_numerator.setNegative(false);
    m_denominator.setNegative(false);
  } else if (m_denominator.isNegative()) {
    m_numerator.setNegative(true);
    m_denominator.setNegative(false);
  }
}

Rational::Rational(const Integer numerator) {
  m_numerator = numerator;
  m_denominator = Integer(1);
}

Rational::Rational(const Rational & other) {
  m_numerator = other.m_numerator;
  m_denominator = other.m_denominator;
}

// Getter
const Integer Rational::numerator() const {
  return m_numerator;
}

const Integer Rational::denominator() const {
  return m_denominator;
}
// Expression subclassing

Expression::Type Rational::type() const {
  return Type::Rational;
}

Expression * Rational::clone() const {
  return new Rational(m_numerator, m_denominator);
}

// Basic operations

Rational Rational::Addition(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Addition(Integer::Multiplication(i.numerator(), j.denominator()), Integer::Multiplication(j.numerator(), i.denominator()));
  Integer newDenominator = Integer::Multiplication(i.denominator(), j.denominator());
  return Rational(newNumerator, newDenominator);
}

Rational Rational::Multiplication(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Multiplication(i.numerator(), j.numerator());
  Integer newDenominator = Integer::Multiplication(i.denominator(), j.denominator());
  return Rational(newNumerator, newDenominator);
}

// Comparison

int Rational::compareToSameTypeExpression(const Expression * e) const {
  assert(e->type() == Expression::Type::Rational);
  const Rational * other = static_cast<const Rational *>(e);
  Integer i1 = Integer::Multiplication(m_numerator, other->denominator());
  Integer i2 = Integer::Multiplication(m_denominator, other->numerator());
  return i1.compareTo(&i2);
}

Evaluation<float> * Rational::privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const {
  // TODO: implement when needed, use Integer.privateEvaluate
  return new Complex<float>(Complex<float>::Float(NAN));
}

Evaluation<double> * Rational::privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const {
  // TODO: implement when needed, use Integer.privateEvaluate
  return new Complex<double>(Complex<double>::Float(NAN));
}

ExpressionLayout * Rational::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  ExpressionLayout * numeratorLayout = m_numerator.createLayout(floatDisplayMode, complexFormat);
  if (m_denominator.isOne()) {
    return numeratorLayout;
  }
  ExpressionLayout * denominatorLayout = m_denominator.createLayout(floatDisplayMode, complexFormat);
  return new FractionLayout(numeratorLayout, denominatorLayout);
}

}

