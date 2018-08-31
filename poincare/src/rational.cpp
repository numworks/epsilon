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
#include <poincare/fraction_layout_node.h>
#include <poincare/char_layout_node.h>

namespace Poincare {

/* Rational Node */

void RationalNode::setDigits(native_uint_t * numeratorDigits, size_t numeratorSize, native_uint_t * denominatorDigits, size_t denominatorSize, bool negative) {
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

RationalNode * RationalNode::FailedAllocationStaticNode() {
  static AllocationFailureRationalNode failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
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
  return sizeof(RationalNode) + sizeof(native_uint_t)*(m_numberOfDigitsNumerator + m_numberOfDigitsDenominator);
}

// Serialization Node

bool RationalNode::needsParenthesesWithParent(const SerializationHelperInterface * e) const {
  if (denominator().isOne()) {
    return false;
  }
  Type types[] = {Type::Division, Type::Power, Type::Factorial};
  return static_cast<const ExpressionNode *>(e)->isOfType(types, 3);
}

#if POINCARE_TREE_LOG
void RationalNode::logAttributes(std::ostream & stream) const {
  stream << " negative=\"" << m_negative << "\"";
  stream << " numberNumDigits=\"" << m_numberOfDigitsNumerator << "\"";
  stream << " Num=\"";
  for (int i=0; i<m_numberOfDigitsNumerator; i++) {
    stream << m_digits[i];
    if (i != (m_numberOfDigitsNumerator-1)) {
      stream << ",";
    }
  }
  stream << "\"";
  stream << " numberDenDigits=\"" << m_numberOfDigitsDenominator << "\"";
  stream << " Den=\"";
  for (int i=m_numberOfDigitsNumerator; i<m_numberOfDigitsDenominator+m_numberOfDigitsNumerator; i++) {
    stream << m_digits[i];
    if (i != (m_numberOfDigitsDenominator+m_numberOfDigitsNumerator-1)) {
      stream << ",";
    }
  }
  stream << "\"";
}
#endif

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

Expression RationalNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
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

int RationalNode::NaturalOrder(const RationalNode * i, const RationalNode * j) {
  if (i->sign() == Sign::Negative && j->sign() == Sign::Positive) {
    return -1;
  }
  if (i->sign() == Sign::Positive && j->sign() == Sign::Negative) {
    return 1;
  }
  NaturalIntegerPointer in = i->numerator();
  NaturalIntegerPointer id = i->denominator();
  NaturalIntegerPointer jn = j->numerator();
  NaturalIntegerPointer jd = j->denominator();
  Integer i1 = NaturalIntegerAbstract::umult(&in, &jd);
  Integer i2 = NaturalIntegerAbstract::umult(&id, &jn);
  return ((int)i->sign())*Integer::NaturalOrder(i1, i2);
}

int RationalNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == ExpressionNode::Type::Rational);
  const RationalNode * other = static_cast<const RationalNode *>(e);
  return NaturalOrder(this, other);
}

// Simplification

Expression RationalNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Rational(this).shallowReduce(context, angleUnit, futureParent);
}

Expression RationalNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Rational(this).shallowBeautify(context, angleUnit);
}

Expression RationalNode::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  return Rational(this).denominator(context, angleUnit);
}

/* Rational  */

// Constructors

Rational::Rational(Integer numerator, Integer denominator) : Number() {
  assert(!denominator.isZero());
  if (!numerator.isOne() && !denominator.isOne()) {
    // Avoid computing GCD if possible
    Integer gcd = Arithmetic::GCD(numerator, denominator);
    numerator = Integer::Division(numerator, gcd).quotient;
    denominator = Integer::Division(denominator, gcd).quotient;
  }
  if (numerator.node()->isAllocationFailure() || denominator.node()->isAllocationFailure()) {
    new (this) Rational(RationalNode::FailedAllocationStaticNode());
    return;
  }
  bool negative = (numerator.sign() == ExpressionNode::Sign::Positive && denominator.sign() == ExpressionNode::Sign::Negative) || (denominator.sign() == ExpressionNode::Sign::Positive && numerator.sign() == ExpressionNode::Sign::Negative);
  size_t size = sizeof(RationalNode) + sizeof(native_uint_t)*(numerator.node()->numberOfDigits() + denominator.node()->numberOfDigits());
  new (this) Rational(size, numerator.node()->digits(), numerator.node()->numberOfDigits(), denominator.node()->digits(), denominator.node()->numberOfDigits(), negative);
}

Rational::Rational(const Integer numerator) : Number() {
  if (numerator.node()->isAllocationFailure()) {
    new (this) Rational(RationalNode::FailedAllocationStaticNode());
  }
  new (this) Rational(numerator.node(), numerator.sign() == ExpressionNode::Sign::Negative);
}

Rational::Rational(const NaturalIntegerAbstract * numerator, bool negative) : Number() {
  native_uint_t one = 1;
  size_t size = sizeof(RationalNode) + sizeof(native_uint_t)*(numerator->numberOfDigits() +1);
  new (this) Rational(size, numerator->digits(), numerator->numberOfDigits(), &one, 1, negative);
  return;
}

Rational::Rational(native_int_t i) : Number()  {
  native_uint_t one = 1;
  if (i == 0) {
    new (this) Rational(sizeof(RationalNode)+sizeof(native_uint_t), nullptr, 0, &one, 1, false);
    return;
  }
  native_uint_t absI = i < 0 ? -i : i;
  new (this) Rational(sizeof(RationalNode)+sizeof(native_uint_t)*2, &absI, 1, &one, 1, i < 0);
}

Integer Rational::signedIntegerNumerator() const {
  NaturalIntegerPointer n = const_cast<Rational *>(this)->node()->numerator();
  Integer i(&n);
  i.setNegative(node()->sign() == ExpressionNode::Sign::Negative);
  return i;
}

Integer Rational::unsignedIntegerNumerator() const {
  NaturalIntegerPointer n = const_cast<Rational *>(this)->node()->numerator();
  return Integer(&n);
}

Integer Rational::integerDenominator() const{
  NaturalIntegerPointer d = const_cast<Rational *>(this)->node()->denominator();
  return Integer(&d);
}

bool Rational::numeratorOrDenominatorIsInfinity() const {
  return node()->numerator().isInfinity() || node()->denominator().isInfinity();
}

// Basic operations

Rational Rational::Addition(const Rational & i, const Rational & j) {
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

Rational Rational::Multiplication(const Rational & i, const Rational & j) {
  NaturalIntegerPointer in = i.node()->numerator();
  NaturalIntegerPointer id = i.node()->denominator();
  NaturalIntegerPointer jn = j.node()->numerator();
  NaturalIntegerPointer jd = j.node()->denominator();
  Integer newNumerator = NaturalIntegerAbstract::umult(&in, &jn);
  Integer newDenominator = NaturalIntegerAbstract::umult(&id, &jd);
  newNumerator.setNegative(i.sign() != j.sign());
  return Rational(newNumerator, newDenominator);
}

Rational Rational::IntegerPower(const Rational & i, const Integer & j) {
  NaturalIntegerPointer in = i.node()->numerator();
  NaturalIntegerPointer id = i.node()->denominator();
  Integer newNumerator = NaturalIntegerPointer::upow(&in, j.node());
  if (i.isNegative() && !j.isEven()) {
    newNumerator.setNegative(true);
  }
  Integer newDenominator = NaturalIntegerPointer::upow(&id, j.node());
  if (j.isNegative()) {
    return Rational(newDenominator, newNumerator);
  }
  return Rational(newNumerator, newDenominator);
}

Rational::Rational(size_t size, native_uint_t * i, size_t numeratorSize, native_uint_t * j, size_t denominatorSize, bool negative) :
  Number(TreePool::sharedPool()->createTreeNode<RationalNode>(size))
{
  assert(j != 0);
  static_cast<RationalNode *>(node())->setDigits(i, numeratorSize, j, denominatorSize, negative);
}

Expression Rational::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  // FIXME:
  /* Infinite Rational should not exist as they aren't parsed and are supposed
   * to be turn in Float if they should appear. We assert(false) so far, but
   * the right behaviour would be to find the right float to represent them.
   * However at that point, it is too late to find it. The issue is earlier... */
#if 0
  if (node()->numerator().isInfinity() && node()->denominator().isInfinity()) {
    assert(false);
    return Undefined();
  }
  // Turn into Infinite if the numerator is too big.
  if (node()->numerator().isInfinity()) {
    assert(false);
    return Infinity(sign() == ExpressionNode::Sign::Negative);
  }
  // Turn into 0 if the denominator is too big.
  if (node()->denominator().isInfinity()) {
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
    return Opposite(abs);
  }
  return *this;
}

Expression Rational::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  NaturalIntegerPointer d = node()->denominator();
  if (d.isOne()) {
    return Expression();
  }
  if (d.isInfinity()) {
    return Infinity(false);
  }
  return Rational(&d, false);
}

Expression Rational::setSign(ExpressionNode::Sign s) const {
  assert(s != ExpressionNode::Sign::Unknown);
  Rational result = *this;
  result.node()->setNegative(s == ExpressionNode::Sign::Negative);
  return result;
}

template double RationalNode::templatedApproximate() const;

}
