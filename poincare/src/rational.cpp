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

bool RationalNode::needsParenthesisWithParent(SerializableNode * e) const {
  if (denominator().isOne()) {
    return false;
  }
  Type types[] = {Type::Division, Type::Power, Type::Factorial};
  return static_cast<ExpressionNode *>(e)->isOfType(types, 3);
}

int RationalNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
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
  numberOfChar += numerator().writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (denominator().isOne()) {
    return numberOfChar;
  }
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  buffer[numberOfChar++] = '/';
  numberOfChar += denominator().writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  return numberOfChar;
}

// Expression subclassing

ExpressionNode::Sign RationalNode::sign() const {
  if (m_negative) {
    return Sign::Negative;
  }
  return Sign::Positive;
}

void RationalNode::setSign(Sign s) {
  assert(s != Sign::Unknown);
  m_negative = s == Sign::Negative ? true : false;
}

// Layout

LayoutRef RationalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  LayoutRef numeratorLayout = numerator().createLayout();
  if (m_negative) {
    numeratorLayout.addChildAtIndex(CharLayoutRef('-'), 0, numeratorLayout.numberOfChildren(), nullptr);
  }
  if (denominator().isOne()) {
    return numeratorLayout;
  }
  LayoutRef denominatorLayout = denominator().createLayout();
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
  IntegerReference i1 = NaturalIntegerAbstract::umult(&in, &jd);
  IntegerReference i2 = NaturalIntegerAbstract::umult(&id, &jn);
  return ((int)i.sign())*IntegerReference::NaturalOrder(i1, i2);
}

int RationalNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == ExpressionNode::Type::Rational);
  const RationalNode * other = static_cast<const RationalNode *>(e);
  return NaturalOrder(*this, *other);
}

// Simplification

ExpressionReference RationalNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  ExpressionReference reference(this);
  if (m_negative) {
    m_negative = false;
    return OppositeReference(reference);
  }
  return reference;
}

ExpressionReference RationalNode::cloneDenominator(Context & context, Preferences::AngleUnit angleUnit) const {
  if (denominator().isOne()) {
    return ExpressionReference(nullptr);
  }
  NaturalIntegerPointer d = denominator();
  assert(!d.isInfinity());
  return RationalReference(&d, false);
}

/* Rational Reference */

// Constructors

RationalReference::RationalReference(IntegerReference numerator, IntegerReference denominator) {
  assert(!denominator.isZero());
  if (!numerator.isOne() && !denominator.isOne()) {
    // Avoid computing GCD if possible
    IntegerReference gcd = Arithmetic::GCD(numerator, denominator);
    numerator = IntegerReference::Division(numerator, gcd).quotient;
    denominator = IntegerReference::Division(denominator, gcd).quotient;
  }
  if (numerator.isAllocationFailure() || denominator.isAllocationFailure()) {
    *this = RationalReference(ExpressionNode::FailedAllocationStaticNode());
  }
  *this = RationalReference(sizeof(RationalNode)+sizeof(native_uint_t)*(numerator.typedNode()->numberOfDigits()+denominator.typedNode()->numberOfDigits()));
  if (isAllocationFailure()) {
    return;
  }
  bool negative = (numerator.sign() == ExpressionNode::Sign::Positive && denominator.sign() == ExpressionNode::Sign::Negative) || (denominator.sign() == ExpressionNode::Sign::Positive && numerator.sign() == ExpressionNode::Sign::Negative);
  typedNode()->setDigits(numerator.typedNode()->digits(), numerator.typedNode()->numberOfDigits(), denominator.typedNode()->digits(), denominator.typedNode()->numberOfDigits(), negative);
  return;
}

RationalReference::RationalReference(const IntegerReference numerator) {
  if (numerator.isAllocationFailure()) {
    *this = RationalReference(ExpressionNode::FailedAllocationStaticNode());
  }
  *this = RationalReference(numerator.typedNode(), numerator.sign() == ExpressionNode::Sign::Negative);
}

RationalReference::RationalReference(const NaturalIntegerAbstract * numerator, bool negative) {
  *this = RationalReference(sizeof(RationalNode)+sizeof(native_uint_t)*(numerator->numberOfDigits()+1));
  if (isAllocationFailure()) {
    return;
  }
  native_uint_t one = 1;
  typedNode()->setDigits(numerator->digits(), numerator->numberOfDigits(), &one, 1, negative);
  return;
}

RationalReference::RationalReference(native_int_t i) {
  *this = RationalReference(sizeof(RationalNode)+sizeof(native_uint_t)*2);
  if (isAllocationFailure()) {
    return;
  }
  native_uint_t absI = i < 0 ? -i : i;
  native_uint_t one = 1;
  typedNode()->setDigits(&absI, 1, &one, 1, i < 0);
  return;
}

RationalReference::RationalReference(native_int_t i, native_int_t j) {
  assert(j != 0);
  *this = RationalReference(sizeof(RationalNode)+sizeof(native_uint_t)*2);
  if (isAllocationFailure()) {
    return;
  }
  native_uint_t absI = i < 0 ? -i : i;
  native_uint_t absJ = j < 0 ? -j : j;
  typedNode()->setDigits(&absI, 1, &absJ, 1, (i < 0 && j > 0) || (i > 0 && j < 0));
  return;
}

bool RationalReference::isOne() const {
  if (isAllocationFailure()) {
    return false;
  }
  return typedNode()->isOne();
}

bool RationalReference::numeratorOrDenominatorIsInfinity() const {
  if (isAllocationFailure()) {
    return false;
  }
  return typedNode()->numerator().isInfinity() || typedNode()->denominator().isInfinity();
}

// Basic operations

RationalReference RationalReference::Addition(const RationalReference i, const RationalReference j) {
  if (i.isAllocationFailure() || j.isAllocationFailure()) {
    return RationalReference(ExpressionNode::FailedAllocationStaticNode());
  }
  NaturalIntegerPointer in = i.typedNode()->numerator();
  NaturalIntegerPointer id = i.typedNode()->denominator();
  NaturalIntegerPointer jn = j.typedNode()->numerator();
  NaturalIntegerPointer jd = j.typedNode()->denominator();
  IntegerReference newDenominator = NaturalIntegerAbstract::umult(&id, &jd);
  IntegerReference newNumeratorPart1 = NaturalIntegerAbstract::umult(&in, &jd);
  IntegerReference newNumeratorPart2 = NaturalIntegerAbstract::umult(&jn, &id);
  newNumeratorPart1.setNegative(i.sign() == ExpressionNode::Sign::Negative);
  newNumeratorPart2.setNegative(j.sign() == ExpressionNode::Sign::Negative);
  IntegerReference newNumerator = IntegerReference::Addition(newNumeratorPart1, newNumeratorPart2);
  return RationalReference(newNumerator, newDenominator);
}

RationalReference RationalReference::Multiplication(const RationalReference i, const RationalReference j) {
  if (i.isAllocationFailure() || j.isAllocationFailure()) {
    return RationalReference(ExpressionNode::FailedAllocationStaticNode());
  }
  NaturalIntegerPointer in = i.typedNode()->numerator();
  NaturalIntegerPointer id = i.typedNode()->denominator();
  NaturalIntegerPointer jn = j.typedNode()->numerator();
  NaturalIntegerPointer jd = j.typedNode()->denominator();
  IntegerReference newNumerator = NaturalIntegerAbstract::umult(&in, &jn);
  IntegerReference newDenominator = NaturalIntegerAbstract::umult(&id, &jd);
  newNumerator.setNegative(i.sign() != j.sign());
  return RationalReference(newNumerator, newDenominator);
}

RationalReference RationalReference::IntegerPower(const RationalReference i, const RationalReference j) {
  if (i.isAllocationFailure() || j.isAllocationFailure()) {
    return RationalReference(ExpressionNode::FailedAllocationStaticNode());
  }
  NaturalIntegerPointer in = i.typedNode()->numerator();
  NaturalIntegerPointer id = i.typedNode()->denominator();
  NaturalIntegerPointer jn = j.typedNode()->numerator();
  IntegerReference newNumerator = NaturalIntegerPointer::upow(&in, &jn);
  IntegerReference newDenominator = NaturalIntegerPointer::upow(&id, &jn);
  if (j.sign() == ExpressionNode::Sign::Negative) {
    return RationalReference(newDenominator, newNumerator);
  }
  return RationalReference(newNumerator, newDenominator);
}

}
