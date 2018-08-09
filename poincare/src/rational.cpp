#include <poincare/rational.h>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
}
#include <poincare/arithmetic.h>
#include <poincare/opposite.h>
#include <poincare/fraction_layout_node.h>
#include <poincare/char_layout_node.h>

namespace Poincare {

/* Rational Node */

void RationalNode::setDigits(native_uint_t * numeratorDigits, size_t numeratorSize, native_uint_t * denominatorDigits, size_t denominatorSize, bool negative) {
  m_negative = negative;
  m_numberOfDigitsNumerator = numeratorSize;
  m_numberOfDigitsDenominator = denominatorSize;
  memcpy(m_digits, numeratorDigits, numeratorSize*sizeof(native_uint_t));
  memcpy(m_digits + m_numberOfDigitsNumerator, denominatorDigits, denominatorSize*sizeof(native_uint_t));
}

RationalNode * RationalNode::FailedAllocationStaticNode() {
  static AllocationFailureRationalNode failure;
  return &failure;
}

NaturalIntegerPointer RationalNode::numerator() const {
  return NaturalIntegerPointer((native_uint_t *)m_digits, m_numberOfDigitsNumerator);
}

NaturalIntegerPointer RationalNode::denominator() const {
  return NaturalIntegerPointer(((native_uint_t *)m_digits+m_numberOfDigitsNumerator), m_numberOfDigitsDenominator);
}

// Tree Node

size_t RationalNode::size() const {
  return sizeof(RationalNode)+sizeof(native_uint_t)*(m_numberOfDigitsNumerator+m_numberOfDigitsDenominator);
}

// Serialization Node

bool RationalNode::needsParenthesesWithParent(const SerializationHelperInterface * e) const {
  if (denominator().isOne()) {
    return false;
  }
  Type types[] = {Type::Division, Type::Power, Type::Factorial};
  return static_cast<const ExpressionNode *>(e)->isOfType(types, 3);
}

int RationalNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (m_negative) {
    buffer[numberOfChar++] = '-';
  }
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }
  numberOfChar += numerator().serialize(buffer+numberOfChar, bufferSize-numberOfChar);
  if (denominator().isOne()) {
    return numberOfChar;
  }
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  buffer[numberOfChar++] = '/';
  numberOfChar += denominator().serialize(buffer+numberOfChar, bufferSize-numberOfChar);
  return numberOfChar;
}

// Expression subclassing

Expression RationalNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  return Rational(this).setSign(s);
}

// Layout

LayoutRef RationalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayoutRef numeratorLayout = numerator().createLayout();
  if (m_negative) {
    numeratorLayout.addChildAtIndex(CharLayoutRef('-'), 0, numeratorLayout.numberOfChildren(), nullptr);
  }
  if (denominator().isOne()) {
    return numeratorLayout;
  }
  HorizontalLayoutRef denominatorLayout = denominator().createLayout();
  return FractionLayoutRef(numeratorLayout, denominatorLayout);
}

// Approximation

template<typename T> T RationalNode::templatedApproximate() const {
  T n = numerator().approximate<T>();
  T d = denominator().approximate<T>();
  return m_negative ? -n/d : n/d;
}

// Comparison

int RationalNode::NaturalOrder(const RationalNode i, const RationalNode j) {
  if (i.sign() == Sign::Negative && j.sign() == Sign::Positive) {
    return -1;
  }
  if (i.sign() == Sign::Positive && j.sign() == Sign::Negative) {
    return 1;
  }
  NaturalIntegerPointer in = i.numerator();
  NaturalIntegerPointer id = i.denominator();
  NaturalIntegerPointer jn = j.numerator();
  NaturalIntegerPointer jd = j.denominator();
  Integer i1 = NaturalIntegerAbstract::umult(&in, &jd);
  Integer i2 = NaturalIntegerAbstract::umult(&id, &jn);
  return ((int)i.sign())*Integer::NaturalOrder(i1, i2);
}

int RationalNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == ExpressionNode::Type::Rational);
  const RationalNode * other = static_cast<const RationalNode *>(e);
  return NaturalOrder(*this, *other);
}

// Simplification

Expression RationalNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const {
  return Rational(this).shallowBeautify(context, angleUnit);
}

Expression RationalNode::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  return Rational(this).shallowBeautify(context, angleUnit);
}

/* Rational  */

// Constructors

Rational::Rational(Integer numerator, Integer denominator) :
  Number(nullptr)
{
  assert(!denominator.isZero());
  if (!numerator.isOne() && !denominator.isOne()) {
    // Avoid computing GCD if possible
    Integer gcd = Arithmetic::GCD(numerator, denominator);
    numerator = Integer::Division(numerator, gcd).quotient;
    denominator = Integer::Division(denominator, gcd).quotient;
  }
  if (numerator.node()->isAllocationFailure() || denominator.node()->isAllocationFailure()) {
    *this = Rational(RationalNode::FailedAllocationStaticNode());
    return;
  }
  *this = Rational(sizeof(RationalNode)+sizeof(native_uint_t)*(numerator.node()->numberOfDigits()+denominator.node()->numberOfDigits()));
  bool negative = (numerator.sign() == ExpressionNode::Sign::Positive && denominator.sign() == ExpressionNode::Sign::Negative) || (denominator.sign() == ExpressionNode::Sign::Positive && numerator.sign() == ExpressionNode::Sign::Negative);
  node()->setDigits(numerator.node()->digits(), numerator.node()->numberOfDigits(), denominator.node()->digits(), denominator.node()->numberOfDigits(), negative);
  return;
}

Rational::Rational(const Integer numerator) :
  Number(nullptr)
{
  if (numerator.node()->isAllocationFailure()) {
    *this = Rational(RationalNode::FailedAllocationStaticNode());
  }
  *this = Rational(numerator.node(), numerator.sign() == ExpressionNode::Sign::Negative);
}

Rational::Rational(const NaturalIntegerAbstract * numerator, bool negative) : Number(nullptr) {
  *this = Rational(sizeof(RationalNode)+sizeof(native_uint_t)*(numerator->numberOfDigits()+1));
  native_uint_t one = 1;
  node()->setDigits(numerator->digits(), numerator->numberOfDigits(), &one, 1, negative);
  return;
}

Rational::Rational(native_int_t i) : Number(nullptr) {
  *this = Rational(sizeof(RationalNode)+sizeof(native_uint_t)*2);
  native_uint_t absI = i < 0 ? -i : i;
  native_uint_t one = 1;
  node()->setDigits(&absI, 1, &one, 1, i < 0);
  return;
}

Rational::Rational(native_int_t i, native_int_t j) : Number(nullptr) {
  assert(j != 0);
  *this = Rational(sizeof(RationalNode)+sizeof(native_uint_t)*2);
  native_uint_t absI = i < 0 ? -i : i;
  native_uint_t absJ = j < 0 ? -j : j;
  node()->setDigits(&absI, 1, &absJ, 1, (i < 0 && j > 0) || (i > 0 && j < 0));
  return;
}


bool Rational::numeratorOrDenominatorIsInfinity() const {
  return node()->numerator().isInfinity() || node()->denominator().isInfinity();
}

// Basic operations

Rational Rational::Addition(const Rational i, const Rational j) {
  NaturalIntegerPointer in = i.node()->numerator();
  NaturalIntegerPointer id = i.node()->denominator();
  NaturalIntegerPointer jn = j.node()->numerator();
  NaturalIntegerPointer jd = j.node()->denominator();
  Integer newDenominator = NaturalIntegerAbstract::umult(&id, &jd);
  Integer newNumeratorPart1 = NaturalIntegerAbstract::umult(&in, &jd);
  Integer newNumeratorPart2 = NaturalIntegerAbstract::umult(&jn, &id);
  newNumeratorPart1.setNegative(i.sign() == ExpressionNode::Sign::Negative);
  newNumeratorPart2.setNegative(j.sign() == ExpressionNode::Sign::Negative);
  Integer newNumerator = Integer::Addition(newNumeratorPart1, newNumeratorPart2);
  return Rational(newNumerator, newDenominator);
}

Rational Rational::Multiplication(const Rational i, const Rational j) {
  NaturalIntegerPointer in = i.node()->numerator();
  NaturalIntegerPointer id = i.node()->denominator();
  NaturalIntegerPointer jn = j.node()->numerator();
  NaturalIntegerPointer jd = j.node()->denominator();
  Integer newNumerator = NaturalIntegerAbstract::umult(&in, &jn);
  Integer newDenominator = NaturalIntegerAbstract::umult(&id, &jd);
  newNumerator.setNegative(i.sign() != j.sign());
  return Rational(newNumerator, newDenominator);
}

Rational Rational::IntegerPower(const Rational i, const Rational j) {
  NaturalIntegerPointer in = i.node()->numerator();
  NaturalIntegerPointer id = i.node()->denominator();
  NaturalIntegerPointer jn = j.node()->numerator();
  Integer newNumerator = NaturalIntegerPointer::upow(&in, &jn);
  Integer newDenominator = NaturalIntegerPointer::upow(&id, &jn);
  if (j.sign() == ExpressionNode::Sign::Negative) {
    return Rational(newDenominator, newNumerator);
  }
  return Rational(newNumerator, newDenominator);
}

Expression Rational::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const {
  if (sign() == ExpressionNode::Sign::Negative) {
    Expression abs = setSign(ExpressionNode::Sign::Positive);
    return Opposite(abs);
  }
  return *this;
}

Expression Rational::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  NaturalIntegerPointer d = node()->denominator();
  if (d.isOne()) {
    return Expression();
  }
  assert(!d.isInfinity());
  return Rational(&d, false);
}

Expression Rational::setSign(ExpressionNode::Sign s) const {
  assert(s != ExpressionNode::Sign::Unknown);
  Rational result = *this;
  result.node()->setNegative(s == ExpressionNode::Sign::Negative);
  return result;
}

}
