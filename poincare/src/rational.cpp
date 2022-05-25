#include <poincare/rational.h>
#include <poincare/arithmetic.h>
#include <poincare/code_point_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/infinity.h>
#include <poincare/opposite.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <math.h>
#include <utility>
#include <stdlib.h>
#include <string.h>

namespace Poincare {

/* Rational Node */

RationalNode::RationalNode(const native_uint_t * numeratorDigits, uint8_t numeratorSize, const native_uint_t * denominatorDigits, uint8_t denominatorSize, bool negative) :
  m_negative(negative),
  m_numberOfDigitsNumerator(numeratorSize),
  m_numberOfDigitsDenominator(denominatorSize)
{
  assert(numeratorSize == 0 || (numeratorDigits == nullptr) == (numeratorSize > Integer::k_maxNumberOfDigits));
  if (numeratorDigits) {
    memcpy(m_digits, numeratorDigits, numeratorSize*sizeof(native_uint_t));
  } else {
    numeratorSize = 0;
  }
  assert(denominatorSize == 0 || (denominatorDigits == nullptr) == (denominatorSize > Integer::k_maxNumberOfDigits));
  if (denominatorDigits) {
    memcpy(m_digits + numeratorSize, denominatorDigits, denominatorSize*sizeof(native_uint_t));
  }
}

Integer RationalNode::signedNumerator() const {
  return Integer::BuildInteger((native_uint_t *)m_digits, m_numberOfDigitsNumerator, m_negative, false);
}

Integer RationalNode::unsignedNumerator() const {
  return Integer::BuildInteger((native_uint_t *)m_digits, m_numberOfDigitsNumerator, false, false);
}

Integer RationalNode::denominator() const {
  uint8_t numeratorSize = m_numberOfDigitsNumerator > Integer::k_maxNumberOfDigits ? 0 : m_numberOfDigitsNumerator;
  return Integer::BuildInteger(((native_uint_t *)m_digits + numeratorSize), m_numberOfDigitsDenominator, false, false);
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
  this->signedNumerator().logInteger(stream);
  stream << "\"";
  stream << " denominator=\"";
  this->denominator().logInteger(stream);
  stream << "\"";
}
#endif

int RationalNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize-1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = signedNumerator().serialize(buffer, bufferSize);
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  if (isInteger()) {
    return numberOfChar;
  }
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '/');
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  numberOfChar += denominator().serialize(buffer+numberOfChar, bufferSize-numberOfChar);
  return numberOfChar;
}

// Layout

Layout RationalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  Layout numeratorLayout = signedNumerator().createLayout();
  if (isInteger()) {
    return numeratorLayout;
  }
  Layout denominatorLayout = denominator().createLayout();
  return FractionLayout::Builder(numeratorLayout, denominatorLayout);
}

// Approximation

template<typename T> T RationalNode::templatedApproximate() const {
  T n = signedNumerator().approximate<T>();
  T d = denominator().approximate<T>();
  return n/d;
}

// Comparison

int RationalNode::NaturalOrder(const RationalNode * i, const RationalNode * j) {
  if (Number(i).sign() == Sign::Negative && Number(j).sign() == Sign::Positive) {
    return -1;
  }
  if (Number(i).sign() == Sign::Positive && Number(j).sign() == Sign::Negative) {
    return 1;
  }
  Integer i1 = Integer::Multiplication(i->signedNumerator(), j->denominator());
  Integer i2 = Integer::Multiplication(i->denominator(), j->signedNumerator());
  return Integer::NaturalOrder(i1, i2);
}

int RationalNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(e->type() == ExpressionNode::Type::Rational);
  const RationalNode * other = static_cast<const RationalNode *>(e);
  return NaturalOrder(this, other);
}

// Simplification

Expression RationalNode::shallowReduce(ReductionContext reductionContext) {
  return Rational(this).shallowReduce();
}

Expression RationalNode::shallowBeautify(ReductionContext * reductionContext) {
  return Rational(this).shallowBeautify();
}

Expression RationalNode::denominator(ReductionContext reductionContext) const {
  return Rational(this).denominator();
}

/* Rational  */

// Constructors

Rational Rational::Builder(Integer & num, Integer & den) {
  assert(!den.isZero());
  if (!num.isOne() && !den.isOne()) {
    // Avoid computing GCD if possible
    Integer gcd = Arithmetic::GCD(num, den);
    num = Integer::Division(num, gcd).quotient;
    den = Integer::Division(den, gcd).quotient;
  }
  bool negative = (!num.isNegative() && den.isNegative()) || (!den.isNegative() && num.isNegative());
  return Rational::Builder(num.digits(), num.numberOfDigits(), den.digits(), den.numberOfDigits(), negative);
}

Rational Rational::Builder(const Integer & numerator) {
  native_uint_t one = 1;
  return Rational::Builder(numerator.digits(), numerator.numberOfDigits(), &one, 1, numerator.isNegative());
}

Rational Rational::Builder(native_int_t i) {
  native_uint_t one = 1;
  if (i == 0) {
    return Rational::Builder(nullptr, 0, &one, 1, false);
  }
  native_uint_t absI = i < 0 ? -i : i;
  return Rational::Builder(&absI, 1, &one, 1, i < 0);
}

Rational Rational::Builder(native_int_t i, native_int_t j) {
  Integer iInteger(i);
  Integer jInteger(j);
  return Rational::Builder(iInteger, jInteger);
}

Rational Rational::Builder(const char * iString, const char * jString) {
  Integer iInteger(iString);
  Integer jInteger(jString);
  return Rational::Builder(iInteger, jInteger);
}

bool Rational::numeratorOrDenominatorIsInfinity() const {
  return signedIntegerNumerator().isOverflow() || integerDenominator().isOverflow();
}

// Basic operations

Rational Rational::Addition(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Addition(Integer::Multiplication(i.signedIntegerNumerator(), j.integerDenominator()), Integer::Multiplication(j.signedIntegerNumerator(), i.integerDenominator()));
  Integer newDenominator = Integer::Multiplication(i.integerDenominator(), j.integerDenominator());
  return Rational::Builder(newNumerator, newDenominator);
}

Rational Rational::Multiplication(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Multiplication(i.signedIntegerNumerator(), j.signedIntegerNumerator());
  Integer newDenominator = Integer::Multiplication(i.integerDenominator(), j.integerDenominator());
  return Rational::Builder(newNumerator, newDenominator);
}

Rational Rational::IntegerPower(const Rational & i, const Integer & j) {
  assert(!(i.isZero() && j.isNegative()));
  Integer absJ = j;
  absJ.setNegative(false);
  Integer newNumerator = Integer::Power(i.signedIntegerNumerator(), absJ);
  Integer newDenominator = Integer::Power(i.integerDenominator(), absJ);
  if (j.isNegative()) {
    return Rational::Builder(newDenominator, newNumerator);
  }
  return Rational::Builder(newNumerator, newDenominator);
}

Rational Rational::Builder(const native_uint_t * i, uint8_t numeratorSize, const native_uint_t * j, uint8_t denominatorSize, bool negative) {
  void * bufferNode = TreePool::sharedPool()->alloc(RationalSize(numeratorSize, denominatorSize));
  RationalNode * node = new (bufferNode) RationalNode(i, numeratorSize, j, denominatorSize, negative);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Rational &>(h);
}

Expression Rational::shallowReduce() {
  // FIXME:
  /* Infinite Rational should not exist as they aren't parsed and are supposed
   * to be turn in Float if they should appear. We assert(false) so far, but
   * the right behaviour would be to find the right float to represent them.
   * However at that point, it is too late to find it. The issue is earlier... */
#if 0
  if (unsignedIntegerNumerator().isOverflow() && integerDenominator().isOverflow()) {
    assert(false);
    return Undefined::Builder();
  }
  // Turn into Infinite if the numerator is too big.
  if (unsignedIntegerNumerator().isOverflow()) {
    assert(false);
    return Infinity::Builder(sign(&context) == ExpressionNode::Sign::Negative);
  }
  // Turn into 0 if the denominator is too big.
  if (integerDenominator().isOverflow()) {
    assert(false);
    return Rational::Builder(0);
  }
#endif
  assert(!numeratorOrDenominatorIsInfinity());
  return *this;
}

Expression Rational::shallowBeautify() {
  if (sign() == ExpressionNode::Sign::Negative) {
    Expression abs = setSign(ExpressionNode::Sign::Positive);
    Opposite o = Opposite::Builder();
    replaceWithInPlace(o);
    o.replaceChildAtIndexInPlace(0, abs);
    return std::move(o);
  }
  return *this;
}

Expression Rational::denominator() const {
  Integer d = integerDenominator();
  if (d.isOne()) {
    return Expression();
  }
  if (d.isOverflow()) {
    return Infinity::Builder(false);
  }
  return Rational::Builder(d);
}

template float RationalNode::templatedApproximate() const;
template double RationalNode::templatedApproximate() const;

}
