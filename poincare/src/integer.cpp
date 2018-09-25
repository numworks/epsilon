#include <poincare/integer.h>
#include <poincare/ieee754.h>
#include <poincare/layout_helper.h>
#include <cmath>
#include <utility>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
}
#if POINCARE_INTEGER_LOG
#include<iostream>
#endif
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

#if POINCARE_TREE_LOG

void Integer::log(std::ostream & stream) const {
  double d = 0.0;
  double base = 1.0;
  for (int i = 0; i < m_numberOfDigits; i++) {
    d += digit(i)*base;
    base *= std::pow(2.0,32.0);
  }
  stream << "Integer: " << d;
}

#endif

/* new operator */

// This bit buffer indicates which cases of the sIntegerBuffer are already allocated
static uint16_t sbusyIntegerBuffer = 0;
static native_uint_t sIntegerBuffer[(Integer::k_maxNumberOfDigits+1)*Integer::k_maxNumberOfIntegerSimutaneously];

void Integer::TidyIntegerBuffer() {
  sbusyIntegerBuffer = 0;
}

native_uint_t * Integer::allocDigits(int numberOfDigits) {
  assert(numberOfDigits <= k_maxNumberOfDigits+1);
  uint16_t bitIndex = 1 << (16-1);
  int index = 0;
  while (sbusyIntegerBuffer & bitIndex) {
    bitIndex >>= 1;
    index++;
  }
  if (bitIndex == 0) { // we overflow the sIntegerBuffer
    assert(false);
    return nullptr;
  }
  sbusyIntegerBuffer |= bitIndex;
  return sIntegerBuffer+index*(Integer::k_maxNumberOfDigits+1);
}

void Integer::freeDigits(native_uint_t * digits) {
  int index = (digits - sIntegerBuffer)/(Integer::k_maxNumberOfDigits+1);
  assert(index < 16);
  sbusyIntegerBuffer &= ~((uint16_t)1 << (16-1-index));
}

// Constructor

Integer Integer::BuildInteger(native_uint_t * digits, uint16_t numberOfDigits, bool negative, bool enableOverflow) {
  if ((!digits || !enableOverflow) && numberOfDigits == k_maxNumberOfDigits+1) {
    return Overflow(negative);
  }
  native_uint_t * newDigits = allocDigits(numberOfDigits);
  for (uint8_t i = 0; i < numberOfDigits; i++) {
    newDigits[i] = digits[i];
  }
  return Integer(newDigits, numberOfDigits, negative, enableOverflow);
}

/* WARNING: This constructor takes ownership of the digits array! */
Integer::Integer(native_uint_t * digits, uint16_t numberOfDigits, bool negative, bool enableOverflow) :
  m_negative(numberOfDigits == 0 ? false : negative),
  m_numberOfDigits(!enableOverflow && numberOfDigits > k_maxNumberOfDigits ? k_maxNumberOfDigits+1 : numberOfDigits),
  m_digits(digits)
{
  if ((m_numberOfDigits <= 1 || (!enableOverflow && m_numberOfDigits > k_maxNumberOfDigits)) && m_digits) {
    freeDigits(m_digits);
    if (m_numberOfDigits == 1) {
      m_digit = digits[0];
    } else {
      m_digits = nullptr;
    }
  }
  m_negative = m_numberOfDigits == 0 ? false : m_negative;
}

Integer::Integer(native_int_t i) {
  if (i == 0) {
    m_digits = nullptr;
    m_numberOfDigits = 0;
    m_negative = false;
    return;
  }
  m_numberOfDigits = 1;
  m_digit = i > 0 ? i : -i;
  m_negative = i < 0;
}

Integer::Integer(double_native_int_t i) {
  if (i == 0) {
    m_digits = nullptr;
    m_numberOfDigits = 0;
    m_negative = false;
    return;
  }
  double_native_uint_t j = i < 0 ? -i : i;
  native_uint_t * d = (native_uint_t *)&j;
  native_uint_t leastSignificantDigit = *d;
  native_uint_t mostSignificantDigit = *(d+1);
  m_numberOfDigits = (mostSignificantDigit == 0) ? 1 : 2;
  if (m_numberOfDigits == 1) {
    m_digit = leastSignificantDigit;
  } else {
    native_uint_t * digits = allocDigits(m_numberOfDigits);
    digits[0] = leastSignificantDigit;
    digits[1] = mostSignificantDigit;
    m_digits = digits;
  }
  m_negative = i < 0;
}

Integer::Integer(const char * digits, size_t length, bool negative) :
  Integer(0)
{
  if (digits != nullptr && digits[0] == '-') {
    negative = true;
    digits++;
    length--;
  }
  if (digits != nullptr) {
    Integer base(10);
    for (size_t i = 0; i < length; i++) {
      *this = Multiplication(*this, base);
      *this = Addition(*this, Integer(*digits-'0'));
      digits++;
    }
  }

  setNegative(isZero() ? false : negative);
}

void Integer::releaseDynamicIvars() {
  if (!usesImmediateDigit() && m_digits) {
    freeDigits(m_digits);
  }
}

Integer::~Integer() {
  releaseDynamicIvars();
}

Integer::Integer(Integer && other) {
  // Pilfer other's data
  if (other.usesImmediateDigit()) {
    m_digit = other.m_digit;
  } else {
    m_digits = other.m_digits;
  }
  m_numberOfDigits = other.m_numberOfDigits;
  m_negative = other.m_negative;

  // Reset other
  other.m_digits = nullptr;
  other.m_numberOfDigits = 1;
  other.m_negative = 0;
}

Integer::Integer(const Integer& other) {
  // Copy other's data
  if (other.usesImmediateDigit() || other.isOverflow()) {
    m_digit = other.m_digit;
  } else {
    native_uint_t * newDigits = allocDigits(other.m_numberOfDigits);
    for (uint8_t i = 0; i < other.m_numberOfDigits; i++) {
      newDigits[i] = other.m_digits[i];
    }
    m_digits = newDigits;
  }
  m_numberOfDigits = other.m_numberOfDigits;
  m_negative = other.m_negative;
}

Integer& Integer::operator=(Integer && other) {
  if (this != &other) {
    releaseDynamicIvars();
    // Pilfer other's ivars
    if (other.usesImmediateDigit()) {
      m_digit = other.m_digit;
    } else {
      m_digits = other.m_digits;
    }
    m_numberOfDigits = other.m_numberOfDigits;
    m_negative = other.m_negative;

    // Reset other
    other.m_digits = nullptr;
    other.m_numberOfDigits = 1;
    other.m_negative = 0;
  }
  return *this;
}

Integer& Integer::operator=(const Integer& other) {
  if (this != &other) {
    releaseDynamicIvars();
    // Copy other's ivars
    if (other.usesImmediateDigit() || other.isOverflow()) {
      m_digit = other.m_digit;
    } else {
      native_uint_t * digits = allocDigits(other.m_numberOfDigits);
      for (uint8_t i = 0; i < other.m_numberOfDigits; i++) {
        digits[i] = other.m_digits[i];
      }
      m_digits = digits;
    }
    m_numberOfDigits = other.m_numberOfDigits;
    m_negative = other.m_negative;
  }
  return *this;
}

// Serialization

int Integer::serialize(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  if (isInfinity()) {
    return PrintFloat::convertFloatToText<float>(m_negative ? -INFINITY : INFINITY, buffer, bufferSize, PrintFloat::k_numberOfStoredSignificantDigits, Preferences::PrintFloatMode::Decimal);
  }

  Integer base(10);
  Integer abs = *this;
  abs.setNegative(false);
  IntegerDivision d = udiv(abs, base);
  int size = 0;
  if (bufferSize == 1) {
    return 0;
  }
  if (isZero()) {
    buffer[size++] = '0';
  } else if (isNegative()) {
    buffer[size++] = '-';
  }

  while (!(d.remainder.isZero() &&
        d.quotient.isZero())) {
    char c = char_from_digit(d.remainder.isZero() ? 0 : d.remainder.digit(0));
    if (size >= bufferSize-1) {
      return PrintFloat::convertFloatToText<float>(NAN, buffer, bufferSize, PrintFloat::k_numberOfStoredSignificantDigits, Preferences::PrintFloatMode::Decimal);
    }
    buffer[size++] = c;
    d = udiv(d.quotient, base);
  }
  buffer[size] = 0;

  // Flip the string
  for (int i = m_negative, j=size-1 ; i < j ; i++, j--) {
    char c = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = c;
  }
  return size;
}

// Layout

HorizontalLayout Integer::createLayout() const {
  char buffer[k_maxNumberOfDigitsBase10];
  int numberOfChars = serialize(buffer, k_maxNumberOfDigitsBase10);
  return LayoutHelper::String(buffer, numberOfChars);
}

// Approximation

template<typename T>
T Integer::approximate() const {
  if (m_numberOfDigits == 0) {
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

  if (isInfinity()) {
    return INFINITY;
  }

  native_uint_t lastDigit = m_numberOfDigits > 0 ? digit(m_numberOfDigits-1) : 0;
  uint8_t numberOfBitsInLastDigit = log2(lastDigit);

  bool sign = m_negative;
  uint16_t exponent = IEEE754<T>::exponentOffset();
  /* Escape case if the exponent is too big to be stored */
  assert(m_numberOfDigits > 0);
  if (((int)m_numberOfDigits-1)*32+numberOfBitsInLastDigit-1> IEEE754<T>::maxExponent()-IEEE754<T>::exponentOffset()) {
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
  uint8_t digitIndex = 2;
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

  T result = IEEE754<T>::buildFloat(sign, exponent, mantissa);

  /* If exponent is 255 and the float is undefined, we have exceed IEEE 754
   * representable float. */
  if (exponent == IEEE754<T>::maxExponent() && std::isnan(result)) {
    return INFINITY;
  }
  return result;
}

// Properties

int Integer::NumberOfBase10DigitsWithoutSign(const Integer & i) {
  assert(!i.isInfinity());
  int numberOfDigits = 1;
  Integer base(10);
  IntegerDivision d = udiv(i, base);
  while (!d.quotient.isZero()) {
    d = udiv(d.quotient, base);
    numberOfDigits++;
  }
  return numberOfDigits;
}

// Comparison

int Integer::NaturalOrder(const Integer & i, const Integer & j) {
  if (i.isNegative() && !j.isNegative()) {
    return -1;
  }
  if (!i.isNegative() && j.isNegative()) {
    return 1;
  }
  return ::Poincare::sign(i.isNegative())*ucmp(i, j);
}

// Arithmetic

IntegerDivision Integer::Division(const Integer & numerator, const Integer & denominator) {
  IntegerDivision ud = udiv(numerator, denominator);
  if (!numerator.isNegative() && !denominator.isNegative()) {
    return ud;
  }
  if (!ud.remainder.isZero() && numerator.isNegative()) {
    ud.quotient = usum(ud.quotient, Integer(1), false);
    ud.remainder = usum(denominator, ud.remainder, true); // |denominator|-remainder
  }
  ud.quotient.setNegative((numerator.isNegative() && !denominator.isNegative()) || (!numerator.isNegative() && denominator.isNegative()));
  return ud;
}

Integer Integer::Power(const Integer & i, const Integer & j) {
  // TODO: optimize with dichotomia
  assert(!j.isNegative());
  if (j.isOverflow()) {
    return Overflow(false);
  }
  Integer index(j);
  Integer result(1);
  while (!index.isZero()) {
    result = Multiplication(result, i);
    index = usum(index, Integer(1), true);
  }
  return result;
}

Integer Integer::Factorial(const Integer & i) {
  assert(!i.isNegative());
  if (i.isOverflow()) {
    return Overflow(false);
  }
  Integer j(2);
  Integer result(1);
  while (ucmp(i,j) >= 0) {
    result = Multiplication(j, result);
    j = usum(j, Integer(1), false);
  }
  return result;
}

Integer Integer::addition(const Integer & a, const Integer & b, bool inverseBNegative, bool oneDigitOverflow) {
  bool bNegative = (inverseBNegative ? !b.m_negative : b.m_negative);
  if (a.m_negative == bNegative) {
    Integer us = usum(a, b, false, oneDigitOverflow);
    us.setNegative(a.m_negative);
    return us;
  } else {
    /* The signs are different, this is in fact a subtraction
     * s = a+b = (abs(a)-abs(b) OR abs(b)-abs(a))
     * 1/abs(a)>abs(b) : s = sign*udiff(a, b)
     * 2/abs(b)>abs(a) : s = sign*udiff(b, a)
     * sign? sign of the greater! */
    if (ucmp(a, b) >= 0) {
      Integer us = usum(a, b, true, oneDigitOverflow);
      us.setNegative(a.m_negative);
      return us;
    } else {
      Integer us = usum(b, a, true, oneDigitOverflow);
      us.setNegative(bNegative);
      return us;
    }
  }
}

Integer Integer::multiplication(const Integer & a, const Integer & b, bool oneDigitOverflow) {
  if (a.isOverflow() || b.isOverflow()) {
    return Integer::Overflow(a.m_negative != b.m_negative);
  }

  uint8_t size = min(a.m_numberOfDigits + b.m_numberOfDigits, k_maxNumberOfDigits + oneDigitOverflow); // Enable overflowing of 1 digit

  native_uint_t * digits = allocDigits(size);
  memset(digits, 0, size*sizeof(native_uint_t));

  double_native_uint_t carry = 0;
  for (uint8_t i = 0; i < a.m_numberOfDigits; i++) {
    double_native_uint_t aDigit = a.digit(i);
    carry = 0;
    for (uint8_t j = 0; j < b.m_numberOfDigits; j++) {
      double_native_uint_t bDigit = b.digit(j);
      /* The fact that aDigit and bDigit are double_native is very important,
       * otherwise the product might end up being computed on single_native size
       * and then zero-padded. */
      double_native_uint_t p = aDigit*bDigit + carry + (double_native_uint_t)(digits[i+j]); // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      if (i+j < (uint8_t) k_maxNumberOfDigits+oneDigitOverflow) {
        digits[i+j] = l[0];
      } else {
        if (l[0] != 0) {
          // Overflow the largest Integer
          freeDigits(digits);
          return Integer::Overflow(a.m_negative != b.m_negative);
        }      }
      carry = l[1];
    }
    if (i+b.m_numberOfDigits < (uint8_t) k_maxNumberOfDigits+oneDigitOverflow) {
      digits[i+b.m_numberOfDigits] += carry;
    } else {
      if (carry != 0) {
        // Overflow the largest Integer
        freeDigits(digits);
        return Integer::Overflow(a.m_negative != b.m_negative);
      }
    }
  }
  while (size>0 && digits[size-1] == 0) {
    size--;
  }
  return Integer(digits, size, a.m_negative != b.m_negative, oneDigitOverflow);
}

int8_t Integer::ucmp(const Integer & a, const Integer & b) {
  if (a.m_numberOfDigits < b.m_numberOfDigits) {
    return -1;
  } else if (a.m_numberOfDigits > b.m_numberOfDigits) {
    return 1;
  }
  if (a.isOverflow() && b.isOverflow()) {
    return 0;
  }
  assert(!a.isOverflow());
  assert(!b.isOverflow());
  for (uint16_t i = 0; i < a.m_numberOfDigits; i++) {
    // Digits are stored most-significant last
    native_uint_t aDigit = a.digit(a.m_numberOfDigits-i-1);
    native_uint_t bDigit = b.digit(b.m_numberOfDigits-i-1);
    if (aDigit < bDigit) {
      return -1;
    } else if (aDigit > bDigit) {
      return 1;
    }
  }
  return 0;
}

Integer Integer::usum(const Integer & a, const Integer & b, bool subtract, bool oneDigitOverflow) {
  if (a.isOverflow() || b.isOverflow()) {
    return Integer::Overflow(a.m_negative != b.m_negative);
  }

  uint8_t size = max(a.m_numberOfDigits, b.m_numberOfDigits);
  if (!subtract) {
    // Addition can overflow
    size++;
  }
  native_uint_t * digits = allocDigits(max(size, k_maxNumberOfDigits+oneDigitOverflow));
  bool carry = false;
  for (uint8_t i = 0; i < size; i++) {
    native_uint_t aDigit = (i >= a.m_numberOfDigits ? 0 : a.digit(i));
    native_uint_t bDigit = (i >= b.m_numberOfDigits ? 0 : b.digit(i));
    native_uint_t result = (subtract ? aDigit - bDigit - carry : aDigit + bDigit + carry);
    if (i < (uint8_t) (k_maxNumberOfDigits + oneDigitOverflow)) {
      digits[i] = result;
    } else {
      if (result != 0) {
        // Overflow the largest Integer
        freeDigits(digits);
        return Integer::Overflow(false);
      }
    }
    if (subtract) {
      carry = (aDigit < result) || (carry && aDigit == result); // There's been an underflow
    } else {
      carry = (aDigit > result) || (bDigit > result); // There's been an overflow
    }
  }
  size = min(size, k_maxNumberOfDigits+oneDigitOverflow);
  while (size>0 && digits[size-1] == 0) {
    size--;
  }
  return Integer(digits, size, false, oneDigitOverflow);
}

Integer Integer::multiplyByPowerOf2(uint8_t pow) const {
  assert(pow < 32);
  native_uint_t * digits = allocDigits(m_numberOfDigits+1);
  native_uint_t carry = 0;
  for (uint8_t i = 0; i < m_numberOfDigits; i++) {
    digits[i] = digit(i) << pow | carry;
    carry = pow == 0 ? 0 : digit(i) >> (32-pow);
  }
  digits[m_numberOfDigits] = carry;
  return Integer(digits, carry ? m_numberOfDigits + 1 : m_numberOfDigits, false, true);
}

Integer Integer::divideByPowerOf2(uint8_t pow) const {
  assert(pow < 32);
  native_uint_t * digits = allocDigits(m_numberOfDigits);
  native_uint_t carry = 0;
  for (int i = m_numberOfDigits - 1; i >= 0; i--) {
    digits[i] = digit(i) >> pow | carry;
    carry = pow == 0 ? 0 : digit(i) << (32-pow);
  }
  return Integer(digits, digits[m_numberOfDigits-1] > 0 ? m_numberOfDigits : m_numberOfDigits-1, false, true);
}

// return this*(2^16)^pow
Integer Integer::multiplyByPowerOfBase(uint8_t pow) const {
  int nbOfHalfDigits = numberOfHalfDigits();
  half_native_uint_t * digits = (half_native_uint_t *)allocDigits(m_numberOfDigits+(pow+1)/2);
  memset(digits, 0, sizeof(native_uint_t)*(m_numberOfDigits+(pow+1)/2));
  for (uint8_t i = 0; i < nbOfHalfDigits; i++) {
    digits[i+pow] = halfDigit(i);
  }
  nbOfHalfDigits += pow;
  return Integer((native_uint_t *)digits, nbOfHalfDigits%2 == 1 ? nbOfHalfDigits/2+1 : nbOfHalfDigits/2, false, true);
}

IntegerDivision Integer::udiv(const Integer & numerator, const Integer & denominator) {
  if (denominator.isOverflow()) {
    return {.quotient = Integer::Overflow(false), .remainder = Integer::Overflow(false)};
  }
  if (numerator.isOverflow()) {
    return {.quotient = Integer::Overflow(false), .remainder = Integer::Overflow(false)};
  }
  /* Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann
   * (Algorithm 1.6) */
  assert(!denominator.isZero());
  if (ucmp(numerator,denominator) < 0) {
    IntegerDivision div = {.quotient = Integer(0), .remainder = Integer(numerator)};
    return div;
  }
  /* Let's call beta = 1 << 16 */
  /* Normalize numerator & denominator:
   * Find A = 2^k*numerator & B = 2^k*denominator such as B > beta/2
   * if A = B*Q+R (R < B) then numerator = denominator*Q + R/2^k. */
  half_native_uint_t b = denominator.halfDigit(denominator.numberOfHalfDigits()-1);
  half_native_uint_t halfBase = 1 << (16-1);
  int pow = 0;
  assert(b != 0);
  while (!(b & halfBase)) {
    b = b << 1;
    pow++;
  }
  Integer A = numerator.multiplyByPowerOf2(pow);
  Integer B = denominator.multiplyByPowerOf2(pow);

  /* A = a[0] + a[1]*beta + ... + a[n+m-1]*beta^(n+m-1)
   * B = b[0] + b[1]*beta + ... + b[n-1]*beta^(n-1) */
  int n = B.numberOfHalfDigits();
  int m = A.numberOfHalfDigits()-n;
  // qDigits is a half_native_uint_t array and enable one digit overflow
  half_native_uint_t * qDigits = (half_native_uint_t *)allocDigits(m/2+1);
  memset(qDigits, 0, (m/2+1)*sizeof(native_uint_t));
  // betaMB = B*beta^m
  Integer betaMB = B.multiplyByPowerOfBase(m);
  if (Integer::NaturalOrder(A,betaMB) >= 0) { // A >= B*beta^m
    qDigits[m] = 1; // q[m] = 1
    A = usum(A, betaMB, true, true); // A-B*beta^m
  }
  native_int_t base = 1 << 16;
  for (int j = m-1; j >= 0; j--) {
    native_uint_t qj2 = ((native_uint_t)A.halfDigit(n+j)*base+(native_uint_t)A.halfDigit(n+j-1))/(native_uint_t)B.halfDigit(n-1); // (a[n+j]*beta+a[n+j-1])/b[n-1]
    half_native_uint_t baseMinus1 = (1 << 16) -1; // beta-1
    qDigits[j] = qj2 < (native_uint_t)baseMinus1 ? (half_native_uint_t)qj2 : baseMinus1; // min(qj2, beta -1)
    A = Integer::addition(A, multiplication(qDigits[j], B.multiplyByPowerOfBase(j), true), true, true); // A-q[j]*beta^j*B
    Integer betaJM = B.multiplyByPowerOfBase(j); // betaJM = B*beta^j
    while (A.isNegative()) {
      qDigits[j] = qDigits[j]-1; // q[j] = q[j]-1
      A = addition(A, betaJM, false, true); // A = B*beta^j+A
    }
  }
  int qNumberOfDigits = m+1;
  while (qDigits[qNumberOfDigits-1] == 0 && qNumberOfDigits > 1) {
    qNumberOfDigits--;
  }
  int qNumberOfDigitsInBase32 = qNumberOfDigits%2 == 1 ? qNumberOfDigits/2+1 : qNumberOfDigits/2;
  IntegerDivision div = {.quotient = Integer((native_uint_t *)qDigits, qNumberOfDigitsInBase32, false), .remainder = A};
  if (pow > 0 && !div.remainder.isZero()) {
    div.remainder = div.remainder.divideByPowerOf2(pow);
  }
  return div;
}

template float Integer::approximate<float>() const;
template double Integer::approximate<double>() const;

}
