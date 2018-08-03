#include <poincare/integer.h>
#include <poincare/ieee754.h>
#include <poincare/layout_engine.h>
#include <poincare/char_layout_node.h>
#include <poincare/rational.h>
#include <cmath>
#include <utility>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
}
#include <cmath>
#include <poincare/ieee754.h>

namespace Poincare {

static inline int max(int x, int y) { return (x>y ? x : y); }

uint8_t log2(native_uint_t v) {
  constexpr int nativeUnsignedIntegerBitCount = 8*sizeof(native_uint_t);
  static_assert(nativeUnsignedIntegerBitCount < 256, "uint8_t cannot contain the log2 of a native_uint_t");
  for (uint8_t i=0; i<nativeUnsignedIntegerBitCount; i++) {
    if (v < ((native_uint_t)1<<i)) {
      return i;
    }
  }
  return 32;
}

static inline char char_from_digit(native_uint_t digit) {
  return '0'+digit;
}

static inline int8_t sign(bool negative) {
  return 1 - 2*(int8_t)negative;
}

/* Natural Integer Abstract */

// Layout

int NaturalIntegerAbstract::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  //TODO: handle 'inf'
  if (isInfinity()) {
    return strlcpy(buffer, "undef", bufferSize);
  }

  IntegerReference base(10);
  IntegerReference ref(this);
  IntegerDivisionReference d = IntegerReference::Division(ref, base);
  int size = 0;
  if (bufferSize == 1) {
    return 0;
  }
  if (isZero()) {
    buffer[size++] = '0';
  }
  while (!(d.remainder.isZero() &&
        d.quotient.isZero())) {
    char c = char_from_digit(d.remainder.digit(0));
    if (size >= bufferSize-1) {
      return strlcpy(buffer, "undef", bufferSize);
    }
    buffer[size++] = c;
    d = IntegerReference::Division(d.quotient, base);
  }
  buffer[size] = 0;

  // Flip the string
  for (int i=0, j=size-1 ; i < j ; i++, j--) {
    char c = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = c;
  }
  return size;
}

LayoutRef NaturalIntegerAbstract::createLayout() const {
  char buffer[k_maxNumberOfDigitsBase10];
  int numberOfChars = writeTextInBuffer(buffer, k_maxNumberOfDigitsBase10);
  return LayoutEngine::createStringLayout(buffer, numberOfChars);
}

// Approximation

template<typename T>
T NaturalIntegerAbstract::approximate() const {
  if (isZero()) {
    /* This special case for 0 is needed, because the current algorithm assumes
     * that the big integer is non zero, thus puts the exponent to 126 (integer
     * area), the issue is that when the mantissa is 0, a "shadow bit" is
     * assumed to be there, thus 126 0x000000 is equal to 0.5 and not zero.
     */
    return (T)0.0;
  }
  assert(sizeof(T) == 4 || sizeof(T) == 8);
  /* We're generating an IEEE 754 compliant float(double).
  * We can tell that:
  * - the sign depends on m_negative
  * - the exponent is the length of our BigInt, in bits - 1 + 127 (-1+1023);
  * - the mantissa is the beginning of our BigInt, discarding the first bit
  */

  native_uint_t lastDigit = digit(m_numberOfDigits-1);
  uint8_t numberOfBitsInLastDigit = log2(lastDigit);

  uint16_t exponent = IEEE754<T>::exponentOffset();
  /* Escape case if the exponent is too big to be stored */
  if ((m_numberOfDigits-1)*32+numberOfBitsInLastDigit-1> IEEE754<T>::maxExponent()-IEEE754<T>::exponentOffset()) {
    return INFINITY;
  }
  exponent += (m_numberOfDigits-1)*32;
  exponent += numberOfBitsInLastDigit-1;

  uint64_t mantissa = 0;
  /* Shift the most significant int to the left of the mantissa. The most
   * significant 1 will be ignore at the end when inserting the mantissa in
   * the resulting uint64_t (as required by IEEE754). */
  assert(IEEE754<T>::size()-numberOfBitsInLastDigit >= 0 && IEEE754<T>::size()-numberOfBitsInLastDigit < 64); // Shift operator behavior is undefined if the right operand is negative, or greater than or equal to the length in bits of the promoted left operand
  mantissa |= ((uint64_t)lastDigit << (IEEE754<T>::size()-numberOfBitsInLastDigit));
  int digitIndex = 2;
  int numberOfBits = numberOfBitsInLastDigit;
  /* Complete the mantissa by inserting, from left to right, every digit of the
   * Integer from the most significant one to the last from. We break when
   * the mantissa is complete to avoid undefined right shifting (Shift operator
   * behavior is undefined if the right operand is negative, or greater than or
   * equal to the length in bits of the promoted left operand). */
  while (m_numberOfDigits >= digitIndex && numberOfBits < IEEE754<T>::size()) {
    lastDigit = digit(m_numberOfDigits-digitIndex);
    numberOfBits += 32;
    if (IEEE754<T>::size() > numberOfBits) {
      assert(IEEE754<T>::size()-numberOfBits > 0 && IEEE754<T>::size()-numberOfBits < 64);
      mantissa |= ((uint64_t)lastDigit << (IEEE754<T>::size()-numberOfBits));
    } else {
      mantissa |= ((uint64_t)lastDigit >> (numberOfBits-IEEE754<T>::size()));
    }
    digitIndex++;
  }

  T result = IEEE754<T>::buildFloat(false, exponent, mantissa);

  /* If exponent is 255 and the float is undefined, we have exceed IEEE 754
   * representable float. */
  if (exponent == IEEE754<T>::maxExponent() && std::isnan(result)) {
    return INFINITY;
  }
  return result;
}

// Properties

int NaturalIntegerAbstract::numberOfDigits(const NaturalIntegerAbstract * i) {
  int numberOfDigits = 1;
  IntegerReference ref(i);
  IntegerReference base(10);
  IntegerDivisionReference d = IntegerReference::Division(ref, base);
  while (!d.quotient.isZero()) {
    ref = d.quotient;
    d = IntegerReference::Division(ref, base);
    numberOfDigits++;
  }
  return numberOfDigits;
}

// Arithmetic

int8_t NaturalIntegerAbstract::ucmp(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b) {
  if (a->m_numberOfDigits < b->m_numberOfDigits) {
    return -1;
  } else if (a->m_numberOfDigits > b->m_numberOfDigits) {
    return 1;
  }
  for (uint16_t i = 0; i < a->m_numberOfDigits; i++) {
    // Digits are stored most-significant last
    native_uint_t aDigit = a->digit(a->m_numberOfDigits-i-1);
    native_uint_t bDigit = b->digit(b->m_numberOfDigits-i-1);
    if (aDigit < bDigit) {
      return -1;
    } else if (aDigit > bDigit) {
      return 1;
    }
  }
  return 0;
}

IntegerReference NaturalIntegerAbstract::usum(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b, bool subtract) {
  size_t size = max(a->m_numberOfDigits, b->m_numberOfDigits);
  if (!subtract) {
    // Addition can overflow
    size++;
  }
  // Overflow
  if (size > k_maxNumberOfDigits + 1) {
    return IntegerReference::Overflow();
  }
  native_uint_t digits[k_maxNumberOfDigits+1];
  bool carry = false;
  for (size_t i = 0; i < size; i++) {
    native_uint_t aDigit = (i >= a->m_numberOfDigits ? 0 : a->digit(i));
    native_uint_t bDigit = (i >= b->m_numberOfDigits ? 0 : b->digit(i));
    native_uint_t result = (subtract ? aDigit - bDigit - carry : aDigit + bDigit + carry);
    digits[i] = result;
    if (subtract) {
      carry = (aDigit < result) || (carry && aDigit == result); // There's been an underflow
    } else {
      carry = (aDigit > result) || (bDigit > result); // There's been an overflow
    }
  }
  while (digits[size-1] == 0 && size>1) {
    size--;
  }
  if (size > k_maxNumberOfDigits) {
    return IntegerReference::Overflow();
  }
  return IntegerReference::IntegerReference(digits, size, false);
}

IntegerReference NaturalIntegerAbstract::umult(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b){
  size_t size = a->m_numberOfDigits + b->m_numberOfDigits;
  // Overflow
  if (size > k_maxNumberOfDigits + 1) {
    return IntegerReference::Overflow();
  }

  native_uint_t digits[k_maxNumberOfDigits+1];
  memset(digits, 0, size*sizeof(native_uint_t));

  double_native_uint_t carry = 0;
  for (size_t i=0; i<a->m_numberOfDigits; i++) {
    double_native_uint_t aDigit = a->digit(i);
    carry = 0;
    for (size_t j=0; j<b->m_numberOfDigits; j++) {
      double_native_uint_t bDigit = b->digit(j);
      /* The fact that aDigit and bDigit are double_native is very important,
       * otherwise the product might end up being computed on single_native size
       * and then zero-padded. */
      double_native_uint_t p = aDigit*bDigit + carry + (double_native_uint_t)(digits[i+j]); // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      digits[i+j] = l[0];
      carry = l[1];
    }
    digits[i+b->m_numberOfDigits] += carry;
  }

  while (digits[size-1] == 0 && size>1) {
    size--;
  }
  // Overflow
  if (size > k_maxNumberOfDigits + 1) {
    return IntegerReference::Overflow();
  }
  return IntegerReference::IntegerReference(digits, size, false);
}

// TODO: OPTIMIZE
IntegerDivisionReference NaturalIntegerAbstract::udiv(const NaturalIntegerAbstract * numerator, const NaturalIntegerAbstract * denominator) {
  /* Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann
   * (Algorithm 1.6) */
  assert(!denominator->isZero());
  if (ucmp(numerator,denominator) < 0) {
    IntegerDivisionReference div = {.quotient = IntegerReference(0), .remainder = IntegerReference(numerator)};
    return div;
  }

  IntegerReference A(numerator);
  IntegerReference B(denominator);
  native_int_t base = 1 << 16;
  // TODO: optimize by just swifting digit and finding 2^kB that makes B normalized
  // Afterwards, we require B to hold Integer node, we check then for allocation failures
  if (B.isAllocationFailure()) {
    return {.quotient = IntegerReference(ExpressionNode::FailedAllocationStaticNode()), .remainder = IntegerReference(ExpressionNode::FailedAllocationStaticNode())};
  }
  native_int_t d = base/(native_int_t)(B.typedNode()->halfDigit(B.typedNode()->numberOfHalfDigits()-1)+1);
  A = IntegerReference::Multiplication(IntegerReference(d), A);
  B = IntegerReference::Multiplication(IntegerReference(d), B);

  // Afterwards, we require A and B to hold Integer node, we check then for allocation failures
  if (A.isAllocationFailure() || B.isAllocationFailure()) {
    return {.quotient = IntegerReference(ExpressionNode::FailedAllocationStaticNode()), .remainder = IntegerReference(ExpressionNode::FailedAllocationStaticNode())};
  }
  int n = B.typedNode()->numberOfHalfDigits();
  int m = A.typedNode()->numberOfHalfDigits()-n;
  half_native_uint_t qDigits[k_maxNumberOfDigits+1];
  memset(qDigits, 0, (m/2+1)*sizeof(native_uint_t));
  IntegerReference betam = IntegerWithHalfDigitAtIndex(1, m+1);
  IntegerReference betaMB = IntegerReference::Multiplication(betam, B); // TODO: can swift all digits by m! B.swift16(mg)
  if (IntegerReference::NaturalOrder(A,betaMB) > 0) {
    qDigits[m] = 1;
    A = IntegerReference::Subtraction(A, betaMB);
  }
  for (int j = m-1; j >= 0; j--) {
    // Afterwards, we require A to hold Integer node, we check then for allocation failure
    if (A.isAllocationFailure()) {
      return {.quotient = IntegerReference(ExpressionNode::FailedAllocationStaticNode()), .remainder = IntegerReference(ExpressionNode::FailedAllocationStaticNode())};
    }
    native_uint_t qj2 = ((native_uint_t)A.typedNode()->halfDigit(n+j)*base+(native_uint_t)A.typedNode()->halfDigit(n+j-1))/(native_uint_t)B.typedNode()->halfDigit(n-1);
    half_native_uint_t baseMinus1 = (1 << 16) -1;
    qDigits[j] = qj2 < (native_uint_t)baseMinus1 ? (half_native_uint_t)qj2 : baseMinus1;
    IntegerReference factor = qDigits[j] > 0 ? IntegerWithHalfDigitAtIndex(qDigits[j], j+1) : IntegerReference(0);
    A = IntegerReference::Subtraction(A, IntegerReference::Multiplication(factor, B));
    IntegerReference m = IntegerReference::Multiplication(IntegerWithHalfDigitAtIndex(1, j+1), B);
    while (A.sign() == ExpressionNode::Sign::Negative) {
      qDigits[j] = qDigits[j]-1;
      A = IntegerReference::Addition(A, m);
    }
  }
  int qNumberOfDigits = m+1;
  while (qDigits[qNumberOfDigits-1] == 0 && qNumberOfDigits > 1) {
    qNumberOfDigits--;
  }
  int qNumberOfDigitsInBase32 = qNumberOfDigits%2 == 1 ? qNumberOfDigits/2+1 : qNumberOfDigits/2;
  IntegerDivisionReference div = {.quotient = IntegerReference((native_uint_t *)qDigits, qNumberOfDigitsInBase32, false), .remainder = A};
  if (d != 1 && !div.remainder.isZero()) {
    IntegerReference dReference(d);
    if (div.remainder.isAllocationFailure() || dReference.isAllocationFailure()) {
      return {.quotient = IntegerReference(ExpressionNode::FailedAllocationStaticNode()), .remainder = IntegerReference(ExpressionNode::FailedAllocationStaticNode())};
    }
    div.remainder = udiv(div.remainder.typedNode(), dReference.typedNode()).quotient;
  }
  return div;
}

IntegerReference NaturalIntegerAbstract::upow(const NaturalIntegerAbstract * i, const NaturalIntegerAbstract * j) {
  // TODO: optimize with dichotomia
  IntegerReference index(j);
  IntegerReference result(1);
  while (!index.isZero()) {
    result = IntegerReference::Multiplication(result, i);
    index = IntegerReference::Subtraction(index, IntegerReference(1));
  }
  return result;
}

IntegerReference NaturalIntegerAbstract::ufact(const NaturalIntegerAbstract * i) {
  IntegerReference j(2);
  if (j.isAllocationFailure()) {
    return IntegerReference(ExpressionNode::FailedAllocationStaticNode());
  }
  IntegerReference result(1);
  while (ucmp(i,j.typedNode()) > 0) {
    result = IntegerReference::Multiplication(j, result);
    j = IntegerReference::Addition(j, IntegerReference(1));
  }
  return result;
}

IntegerReference NaturalIntegerAbstract::IntegerWithHalfDigitAtIndex(half_native_uint_t halfDigit, int index) {
  assert(halfDigit != 0);
  // Overflow
  if ((index + 1)/2) {
    return IntegerReference::Overflow();
  }
  half_native_uint_t digits[k_maxNumberOfDigits+1];
  memset(digits, 0, (index+1)/2*sizeof(native_uint_t));
  digits[index-1] = halfDigit;
  int indexInBase32 = index%2 == 1 ? index/2+1 : index/2;
  return IntegerReference((native_uint_t *)digits, indexInBase32, false);
}

/* Natural Integer Pointer */

NaturalIntegerPointer::NaturalIntegerPointer(native_uint_t * buffer, size_t size) :
  NaturalIntegerAbstract(size)
{
  memcpy(m_digits, buffer, size*sizeof(native_uint_t));
}

/* Integer Node */

void IntegerNode::setDigits(native_int_t i) {
  if (i == 0) {
    m_numberOfDigits = 0;
    m_negative = false;
    return;
  }
  m_negative = i < 0;
  m_digits[0] = i < 0 ? -i : i;
  m_numberOfDigits = 1;
}

void IntegerNode::setDigits(double_native_int_t i) {
  if (i == 0) {
    m_numberOfDigits = 0;
    m_negative = false;
    return;
  }
  double_native_uint_t j = i < 0 ? -i : i;
  native_uint_t * digits = (native_uint_t *)&j;
  native_uint_t leastSignificantDigit = *digits;
  native_uint_t mostSignificantDigit = *(digits+1);
  m_numberOfDigits = (mostSignificantDigit == 0) ? 1 : 2;
  m_digits[0] = leastSignificantDigit;
  if (m_numberOfDigits > 1) {
    digits[1] = mostSignificantDigit;
  }
  m_negative = i < 0;

}
void IntegerNode::setDigits(const native_uint_t * digits, size_t size, bool negative) {
  memcpy(m_digits, digits, size*sizeof(native_uint_t));
  m_numberOfDigits = size;
  m_negative = negative;
}

ExpressionReference IntegerNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return RationalReference(IntegerReference((NaturalIntegerAbstract *)this), IntegerReference(1));
}

template<typename T>
T IntegerNode::templatedApproximate() const {
  T a = NaturalIntegerAbstract::approximate<T>();
  return m_negative ? -a : a;
}

// Layout

LayoutRef IntegerNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  LayoutRef naturalLayout = NaturalIntegerAbstract::createLayout();
  if (m_negative) {
    naturalLayout.addChildTreeAtIndex(CharLayoutRef('-'), 0, naturalLayout.numberOfChildren(), nullptr);
  }
  return naturalLayout;
}

int IntegerNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
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
  numberOfChar += NaturalIntegerAbstract::writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  return numberOfChar;
}

size_t IntegerNode::size() const {
  return m_numberOfDigits*sizeof(native_uint_t)+sizeof(IntegerNode);
}

ExpressionReference IntegerNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  setNegative(s == Sign::Negative);
  return IntegerReference((NaturalIntegerAbstract *)this);
}

void IntegerNode::setNegative(bool negative) {
  if (isZero()) { // Zero cannot be negative
    return;
  }
  m_negative = negative;
}

int IntegerNode::NaturalOrder(const IntegerNode * i, const IntegerNode * j) {
  if (i->sign() == Sign::Negative && j->sign() == Sign::Positive) {
    return -1;
  }
  if (i->sign() == Sign::Positive && j->sign() == Sign::Negative) {
    return 1;
  }
  return ::Poincare::sign(i->sign() == Sign::Negative)*ucmp(i, j);
}

/* IntegerReference */

IntegerReference::IntegerReference(const native_uint_t * digits, size_t numberOfDigits, bool negative) :
  IntegerReference(numberOfDigits*sizeof(native_uint_t)+sizeof(IntegerNode))
{
  if (node()->isAllocationFailure()) {
    return;
  }
  if (numberOfDigits == 1 && digits[0] == 0) {
    negative = false;
  }
  typedNode()->setDigits(digits, numberOfDigits, negative);
}

IntegerReference::IntegerReference(const char * digits, size_t length, bool negative) {
  if (digits != nullptr && digits[0] == '-') {
    negative = true;
    digits++;
  }
  native_uint_t buffer[IntegerNode::k_maxNumberOfDigits];
  double_native_uint_t d = 0;
  int size = 0;
  if (digits != nullptr) {
    for (size_t s = 0; s < length; s++) {
      if (d*10+(*digits-'0') > 0xFFFFFFFF) {
        buffer[size++] = d;
        d = 0;
      }
      d = 10*d+(*digits-'0');
      digits++;
    }
  }
  buffer[size++] = d;
  *this = IntegerReference(buffer, size, negative);
}

IntegerReference::IntegerReference(const NaturalIntegerAbstract * naturalInteger) :
  IntegerReference(naturalInteger->numberOfDigits()*sizeof(native_uint_t)+sizeof(IntegerNode))
{
  if (!node()->isAllocationFailure()) {
    typedNode()->setDigits(naturalInteger->digits(), naturalInteger->numberOfDigits(), false);
  }
}

IntegerReference::IntegerReference(native_int_t i) :
  IntegerReference(i == 0 ? sizeof(IntegerNode) : sizeof(IntegerNode)+sizeof(native_uint_t))
{
  if (!node()->isAllocationFailure()) {
    typedNode()->setDigits(i);
  }
}

IntegerReference::IntegerReference(double_native_int_t i)
{
  double_native_uint_t j = i;
  if (i == 0) {
    *this = IntegerReference(sizeof(IntegerNode));
  } else if (j <= 0xFFFFFFFF) {
    *this = IntegerReference(sizeof(IntegerNode)+sizeof(native_uint_t));
  } else {
    *this = IntegerReference(sizeof(IntegerNode)+2*sizeof(native_uint_t));
  }
  if (!node()->isAllocationFailure()) {
    typedNode()->setDigits(i);
  }
}

int IntegerReference::extractedInt() const {
  if (isAllocationFailure()) {
    return 0;
  }
  assert(numberOfDigits() == 1 && digit(0) <= k_maxExtractableInteger);
  return node()->sign() == ExpressionNode::Sign::Negative ? -digit(0) : digit(0);
}

int IntegerReference::NaturalOrder(const IntegerReference i, const IntegerReference j) {
  if (i.isAllocationFailure() || j.isAllocationFailure()) {
    return 0;
  }
  return IntegerNode::NaturalOrder(i.typedNode(), j.typedNode());
}

bool IntegerReference::isZero() const {
  if (isAllocationFailure()) {
    return false;
  }
  return typedNode()->isZero();
}

bool IntegerReference::isOne() const {
  if (isAllocationFailure()) {
    return false;
  }
  return typedNode()->isOne();
}

bool IntegerReference::isInfinity() const {
  if (isAllocationFailure()) {
    return false;
  }
  return typedNode()->isInfinity();
}

bool IntegerReference::isEven() const {
  if (isAllocationFailure()) {
    return false;
  }
  return typedNode()->isEven();
}

void IntegerReference::setNegative(bool negative) {
  if (!isAllocationFailure()) {
    return typedNode()->setNegative(negative);
  }
}

// Arithmetic

IntegerReference IntegerReference::Addition(const IntegerReference a, const IntegerReference b) {
  return addition(a, b, false);
}

IntegerReference IntegerReference::Subtraction(const IntegerReference a, const IntegerReference b) {
  return addition(a, b, true);
}

IntegerReference IntegerReference::Multiplication(const IntegerReference a, const IntegerReference b) {
  if (a.isAllocationFailure() || b.isAllocationFailure()) {
    return IntegerReference(ExpressionNode::FailedAllocationStaticNode());
  }
  IntegerReference um = IntegerNode::umult(a.typedNode(), b.typedNode());
  um.setNegative(a.sign() != b.sign());
  return um;
}

IntegerDivisionReference IntegerReference::Division(const IntegerReference numerator, const IntegerReference denominator) {
  if (numerator.isAllocationFailure() || denominator.isAllocationFailure()) {
    return {.quotient = IntegerReference(ExpressionNode::FailedAllocationStaticNode()), .remainder = IntegerReference(ExpressionNode::FailedAllocationStaticNode())};
  }
  IntegerDivisionReference ud = IntegerNode::udiv(numerator.typedNode(), denominator.typedNode());
  if (numerator.sign() == ExpressionNode::Sign::Positive && denominator.sign() == ExpressionNode::Sign::Positive) {
    return ud;
  }
  if (NaturalOrder(ud.remainder, IntegerReference(0)) == 0) {
    if (numerator.sign() == ExpressionNode::Sign::Positive || denominator.sign() == ExpressionNode::Sign::Positive) {
      ud.quotient.setNegative(true);
    }
    return ud;
  }
  if (numerator.sign() == ExpressionNode::Sign::Negative) {
    if (denominator.sign() == ExpressionNode::Sign::Negative) {
      ud.remainder.setNegative(true);
      ud.quotient = Addition(ud.quotient, IntegerReference(1));
      ud.remainder = Subtraction(ud.remainder, denominator);
   } else {
      ud.quotient.setNegative(true);
      ud.quotient = Subtraction(ud.quotient, IntegerReference(1));
      ud.remainder = Subtraction(denominator, ud.remainder);
    }
  } else {
    assert(denominator.sign() == ExpressionNode::Sign::Negative);
    ud.quotient.setNegative(true);
  }
  return ud;
}

IntegerReference IntegerReference::Power(const IntegerReference i, const IntegerReference j) {
  if (i.isAllocationFailure() || j.isAllocationFailure()) {
    return IntegerReference(ExpressionNode::FailedAllocationStaticNode());
  }
  assert(j.sign() == ExpressionNode::Sign::Positive);
  IntegerReference upow = IntegerNode::upow(i.typedNode(), j.typedNode());
  upow.setNegative(i.sign() == ExpressionNode::Sign::Negative && !j.isEven());
  return upow;
}

IntegerReference IntegerReference::Factorial(const IntegerReference i) {
  if (i.isAllocationFailure()) {
    return IntegerReference(ExpressionNode::FailedAllocationStaticNode());
  }
  assert(i.sign() == ExpressionNode::Sign::Positive);
  return IntegerNode::ufact(i.typedNode());
}

IntegerReference IntegerReference::addition(const IntegerReference a, const IntegerReference b, bool inverseBNegative) {
  if (a.isAllocationFailure() || b.isAllocationFailure()) {
    return IntegerReference(ExpressionNode::FailedAllocationStaticNode());
  }
  bool bNegative = (inverseBNegative ? b.sign() == ExpressionNode::Sign::Positive : b.sign() == ExpressionNode::Sign::Negative);
  if ((a.sign() == ExpressionNode::Sign::Negative) == bNegative) {
    IntegerReference us = IntegerNode::usum(a.typedNode(), b.typedNode(), false);
    us.setNegative(a.sign() == ExpressionNode::Sign::Negative);
    return us;
  } else {
    /* The signs are different, this is in fact a subtraction
     * s = a+b = (abs(a)-abs(b) OR abs(b)-abs(a))
     * 1/abs(a)>abs(b) : s = sign*udiff(a, b)
     * 2/abs(b)>abs(a) : s = sign*udiff(b, a)
     * sign? sign of the greater! */
    if (IntegerNode::ucmp(a.typedNode(), b.typedNode()) >= 0) {
      IntegerReference us = IntegerNode::usum(a.typedNode(), b.typedNode(), true);
      us.setNegative(a.sign() == ExpressionNode::Sign::Negative);
      return us;
    } else {
      IntegerReference us = IntegerNode::usum(a.typedNode(), b.typedNode(), true);
      us.setNegative(bNegative);
      return us;
    }
  }
}

}
