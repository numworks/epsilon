#include <poincare/rational.h>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
}
#include <poincare/arithmetic.h>
#include <poincare/opposite.h>
#include <poincare/infinity.h>
#include <poincare/fraction_layout.h>
#include <poincare/char_layout.h>

namespace Poincare {

/* Rational Node */

void RationalNode::setDigits(const native_uint_t * numeratorDigits, uint8_t numeratorSize, const native_uint_t * denominatorDigits, uint8_t denominatorSize, bool negative) {
  m_negative = negative;
  m_numberOfDigitsNumerator = numeratorSize;
  m_numberOfDigitsDenominator = denominatorSize;
  if (numeratorDigits) {
    memcpy(m_digits, numeratorDigits, numeratorSize*sizeof(native_uint_t));
  }
  if (denominatorDigits) {
    memcpy(m_digits + numeratorSize, denominatorDigits, denominatorSize*sizeof(native_uint_t));
  }
}

void RationalNode::initToMatchSize(size_t goalSize) {
  assert(goalSize != sizeof(RationalNode));
  int digitsSize = goalSize - sizeof(RationalNode);
  assert(digitsSize%sizeof(native_uint_t) == 0);
  /* We are initing the Rational to match a specific size. The built rational
   * is dummy. However, we cannot assign to m_numberOfDigitsNumerator (or
   * m_numberOfDigitsDenominator) values that are aboce k_maxNumberOfDigits.
   * To prevent that, we evenly separe digits between numerator and denominator. */
  size_t numberOfDigits = digitsSize/sizeof(native_uint_t);
  m_numberOfDigitsNumerator = numberOfDigits/2;
  m_numberOfDigitsDenominator = numberOfDigits-m_numberOfDigitsNumerator;
  assert(size() == goalSize);
}

Integer RationalNode::signedNumerator() const {
  return Integer::BuildInteger((native_uint_t *)m_digits, m_numberOfDigitsNumerator, m_negative);
}

Integer RationalNode::unsignedNumerator() const {
  return Integer::BuildInteger((native_uint_t *)m_digits, m_numberOfDigitsNumerator, false);
}

Integer RationalNode::denominator() const {
  return Integer::BuildInteger(((native_uint_t *)m_digits+m_numberOfDigitsNumerator), m_numberOfDigitsDenominator, false);
}

// Tree Node

static size_t RationalSize(uint8_t numeratorNumberOfDigits, uint8_t denominatorNumberOfDigits) {
  uint8_t realNumeratorSize = numeratorNumberOfDigits > Integer::k_maxNumberOfDigits ? 0 : numeratorNumberOfDigits;
  uint8_t realDenominatorSize = denominatorNumberOfDigits > Integer::k_maxNumberOfDigits ? 0 : denominatorNumberOfDigits;
  return sizeof(RationalNode) + sizeof(native_uint_t)*(realNumeratorSize + realDenominatorSize);
}

size_t RationalNode::size() const {
  return RationalSize(m_numberOfDigitsNumerator, m_numberOfDigitsDenominator);
}

// Serialization Node

#if POINCARE_TREE_LOG
void RationalNode::logAttributes(std::ostream & stream) const {
  stream << " negative=\"" << m_negative << "\"";
  stream << " numerator=\"";
  this->signedNumerator().log(stream);
  stream << "\"";
  stream << " denominator=\"";
  this->denominator().log(stream);
  stream << "\"";
}
#endif

int RationalNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = signedNumerator().serialize(buffer, bufferSize);
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

Expression RationalNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return Rational(this).setSign(s);
}

// Layout

Layout RationalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout numeratorLayout = signedNumerator().createLayout();
  if (denominator().isOne()) {
    return numeratorLayout;
  }
  HorizontalLayout denominatorLayout = denominator().createLayout();
  return FractionLayout(numeratorLayout, denominatorLayout);
}

// Approximation

template<typename T> T RationalNode::templatedApproximate() const {
  T n = signedNumerator().approximate<T>();
  T d = denominator().approximate<T>();
  return n/d;
}

// Comparison

int RationalNode::NaturalOrder(const RationalNode * i, const RationalNode * j) {
  if (i->sign() == Sign::Negative && j->sign() == Sign::Positive) {
    return -1;
  }
  if (i->sign() == Sign::Positive && j->sign() == Sign::Negative) {
    return 1;
  }
  Integer i1 = Integer::Multiplication(i->signedNumerator(), j->denominator());
  Integer i2 = Integer::Multiplication(i->denominator(), j->signedNumerator());
  return Integer::NaturalOrder(i1, i2);
}

int RationalNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == ExpressionNode::Type::Rational);
  const RationalNode * other = static_cast<const RationalNode *>(e);
  return NaturalOrder(this, other);
}

// Simplification

Expression RationalNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Rational(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Expression RationalNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Rational(this).shallowBeautify(context, angleUnit);
}

Expression RationalNode::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  return Rational(this).denominator(context, angleUnit);
}

/* Rational  */

// Constructors

Rational::Rational(Integer & num, Integer & den) : Number() {
  assert(!den.isZero());
  if (!num.isOne() && !den.isOne()) {
    // Avoid computing GCD if possible
    Integer gcd = Arithmetic::GCD(num, den);
    num = Integer::Division(num, gcd).quotient;
    den = Integer::Division(den, gcd).quotient;
  }
  bool negative = (!num.isNegative() && den.isNegative()) || (!den.isNegative() && num.isNegative());
  new (this) Rational(num.digits(), num.numberOfDigits(), den.digits(), den.numberOfDigits(), negative);
}

Rational::Rational(const Integer & numerator) : Number() {
  native_uint_t one = 1;
  new (this) Rational(numerator.digits(), numerator.numberOfDigits(), &one, 1, numerator.isNegative());
}

Rational::Rational(native_int_t i) : Number()  {
  native_uint_t one = 1;
  if (i == 0) {
    new (this) Rational(nullptr, 0, &one, 1, false);
    return;
  }
  native_uint_t absI = i < 0 ? -i : i;
  new (this) Rational(&absI, 1, &one, 1, i < 0);
}

Rational::Rational(native_int_t i, native_int_t j) : Number() {
  Integer iInteger(i);
  Integer jInteger(j);
  new (this) Rational(iInteger, jInteger);
}

Rational::Rational(const char * iString, const char * jString) : Number() {
  Integer iInteger(iString);
  Integer jInteger(jString);
  new (this) Rational(iInteger, jInteger);
}

bool Rational::numeratorOrDenominatorIsInfinity() const {
  return signedIntegerNumerator().isInfinity() || integerDenominator().isInfinity();
}

// Basic operations

Rational Rational::Addition(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Addition(Integer::Multiplication(i.signedIntegerNumerator(), j.integerDenominator()), Integer::Multiplication(j.signedIntegerNumerator(), i.integerDenominator()));
  Integer newDenominator = Integer::Multiplication(i.integerDenominator(), j.integerDenominator());
  return Rational(newNumerator, newDenominator);
}

Rational Rational::Multiplication(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Multiplication(i.signedIntegerNumerator(), j.signedIntegerNumerator());
  Integer newDenominator = Integer::Multiplication(i.integerDenominator(), j.integerDenominator());
  return Rational(newNumerator, newDenominator);
}

Rational Rational::IntegerPower(const Rational & i, const Integer & j) {
  Integer absJ = j;
  absJ.setNegative(false);
  Integer newNumerator = Integer::Power(i.signedIntegerNumerator(), absJ);
  Integer newDenominator = Integer::Power(i.integerDenominator(), absJ);
  if (j.isNegative()) {
    return Rational(newDenominator, newNumerator);
  }
  return Rational(newNumerator, newDenominator);
}

Rational::Rational(const native_uint_t * i, uint8_t numeratorSize, const native_uint_t * j, uint8_t denominatorSize, bool negative) :
  Number(TreePool::sharedPool()->createTreeNode<RationalNode>(RationalSize(numeratorSize, denominatorSize)))
{
  static_cast<RationalNode *>(node())->setDigits(i, numeratorSize, j, denominatorSize, negative);
}

Expression Rational::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  // FIXME:
  /* Infinite Rational should not exist as they aren't parsed and are supposed
   * to be turn in Float if they should appear. We assert(false) so far, but
   * the right behaviour would be to find the right float to represent them.
   * However at that point, it is too late to find it. The issue is earlier... */
#if 0
  if (unsignedIntegerNumerator().isInfinity() && integerDenominator().isInfinity()) {
    assert(false);
    return Undefined();
  }
  // Turn into Infinite if the numerator is too big.
  if (unsignedIntegerNumerator().isInfinity()) {
    assert(false);
    return Infinity(sign() == ExpressionNode::Sign::Negative);
  }
  // Turn into 0 if the denominator is too big.
  if (integerDenominator().isInfinity()) {
    assert(false);
    return Rational(0);
  }
#endif
  assert(!numeratorOrDenominatorIsInfinity());
  return *this;
}

Expression Rational::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  if (sign() == ExpressionNode::Sign::Negative) {
    Expression abs = setSign(ExpressionNode::Sign::Positive);
    Opposite o;
    replaceWithInPlace(o);
    o.replaceChildAtIndexInPlace(0, abs);
    return o;
  }
  return *this;
}

Expression Rational::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  Integer d = integerDenominator();
  if (d.isOne()) {
    return Expression();
  }
  if (d.isInfinity()) {
    return Infinity(false);
  }
  return Rational(d);
}

Expression Rational::setSign(ExpressionNode::Sign s) {
  assert(s != ExpressionNode::Sign::Unknown);
  node()->setNegative(s == ExpressionNode::Sign::Negative);
  return *this;
}

template double RationalNode::templatedApproximate() const;

}
