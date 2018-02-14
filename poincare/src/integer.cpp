#include <poincare/integer.h>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
}
#include <cmath>
#include <poincare/complex.h>
#include <poincare/ieee754.h>
#include "layout/string_layout.h"
#include <utility>

namespace Poincare {

static inline int max(int x, int y) { return (x>y ? x : y); }

uint8_t log2(Integer::native_uint_t v) {
  constexpr int nativeUnsignedIntegerBitCount = 8*sizeof(Integer::native_uint_t);
  static_assert(nativeUnsignedIntegerBitCount < 256, "uint8_t cannot contain the log2 of a native_uint_t");
  for (uint8_t i=0; i<nativeUnsignedIntegerBitCount; i++) {
    if (v < ((Integer::native_uint_t)1<<i)) {
      return i;
    }
  }
  return 32;
}

static inline char char_from_digit(Integer::native_uint_t digit) {
  return '0'+digit;
}

static inline int8_t sign(bool negative) {
  return 1 - 2*(int8_t)negative;
}

// Constructors

static_assert(sizeof(Integer::double_native_int_t) == 2*sizeof(Integer::native_int_t), "double_native_int_t type has not the right size compared to native_int_t");
static_assert(sizeof(Integer::native_int_t) == sizeof(Integer::native_uint_t), "native_int_t type has not the right size compared to native_uint_t");

Integer::Integer(double_native_int_t i) {
  double_native_uint_t j = i < 0 ? -i : i;
  native_uint_t * digits = (native_uint_t *)&j;
  native_uint_t leastSignificantDigit = *digits;
  native_uint_t mostSignificantDigit = *(digits+1);
  m_numberOfDigits = (mostSignificantDigit == 0) ? 1 : 2;
  if (m_numberOfDigits == 1) {
    m_digit = leastSignificantDigit;
  } else {
    native_uint_t * digits = new native_uint_t [2];
    digits[0] = leastSignificantDigit;
    digits[1] = mostSignificantDigit;
    m_digits = digits;
  }
  m_negative = i < 0;
}

/* Caution: string is NOT guaranteed to be NULL-terminated! */
Integer::Integer(const char * digits, bool negative) :
  Integer(0)
{
  if (digits != nullptr && digits[0] == '-') {
    negative = true;
    digits++;
  }

  Integer result = Integer(0);

  if (digits != nullptr) {
    Integer base = Integer(10);
    while (*digits >= '0' && *digits <= '9') {
      result = Multiplication(result, base);
      result = Addition(result, Integer(*digits-'0'));
      digits++;
    }
  }

  *this = std::move(result);

  if (isZero()) {
    negative = false;
  }
  m_negative = negative;
}

Integer Integer::exponent(int fractionalPartLength, const char * exponent, int exponentLength, bool exponentNegative) {
  Integer base = Integer(10);
  Integer power = Integer(0);
  for (int i = 0; i < exponentLength; i++) {
    power = Multiplication(power, base);
    power = Addition(power, Integer(*exponent-'0'));
    exponent++;
  }
  if (exponentNegative) {
    power.setNegative(true);
  }
  return Subtraction(Integer(fractionalPartLength), power);
}

Integer Integer::numerator(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, bool negative, Integer * exponent) {
  Integer base = Integer(10);
  Integer numerator = Integer(integralPart, negative);
  for (int i = 0; i < fractionalPartLength; i++) {
    numerator = Multiplication(numerator, base);
    numerator = Addition(numerator, Integer(*fractionalPart-'0'));
    fractionalPart++;
  }
  if (exponent->isNegative()) {
    while (!exponent->isEqualTo(Integer(0))) {
      numerator = Multiplication(numerator, base);
      *exponent = Addition(*exponent, Integer(1));
    }
  }
  return numerator;
}

Integer Integer::denominator(Integer * exponent) {
  Integer base = Integer(10);
  Integer denominator = Integer(1);
  if (!exponent->isNegative()) {
    while (!exponent->isEqualTo(Integer(0))) {
      denominator = Multiplication(denominator, base);
      *exponent = Subtraction(*exponent, Integer(1));
    }
  }
  return denominator;
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
  other.m_digit = 0;
  other.m_numberOfDigits = 1;
  other.m_negative = 0;
}

Integer::Integer(const Integer& other) {
  // Copy other's data
  if (other.usesImmediateDigit()) {
    m_digit = other.m_digit;
  } else {
    native_uint_t * digits = new native_uint_t [other.m_numberOfDigits];
    for (int i=0; i<other.m_numberOfDigits; i++) {
      digits[i] = other.m_digits[i];
    }
    m_digits = digits;
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
    other.m_digit = 0;
    other.m_numberOfDigits = 1;
    other.m_negative = 0;
  }
  return *this;
}

Integer& Integer::operator=(const Integer& other) {
  if (this != &other) {
    releaseDynamicIvars();
    // Copy other's ivars
    if (other.usesImmediateDigit()) {
      m_digit = other.m_digit;
    } else {
      native_uint_t * digits = new native_uint_t [other.m_numberOfDigits];
      for (int i=0; i<other.m_numberOfDigits; i++) {
        digits[i] = other.m_digits[i];
      }
      m_digits = digits;
    }
    m_numberOfDigits = other.m_numberOfDigits;
    m_negative = other.m_negative;
  }
  return *this;
}

void Integer::setNegative(bool negative) {
  if (isZero()) { // Zero cannot be negative
    return;
  }
  m_negative = negative;
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

bool Integer::isEqualTo(const Integer & other) const {
  return (NaturalOrder(*this, other) == 0);
}

bool Integer::isLowerThan(const Integer & other) const {
  return (NaturalOrder(*this, other) < 0);
}

// Arithmetic

Integer Integer::Addition(const Integer & a, const Integer & b) {
  return addition(a, b, false);
}

Integer Integer::Subtraction(const Integer & a, const Integer & b) {
  return addition(a, b, true);
}

Integer Integer::Multiplication(const Integer & a, const Integer & b) {
  assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t));
  uint16_t productSize = a.m_numberOfDigits + b.m_numberOfDigits;
  native_uint_t * digits = new native_uint_t [productSize];
  memset(digits, 0, productSize*sizeof(native_uint_t));

  double_native_uint_t carry = 0;
  for (uint16_t i=0; i<a.m_numberOfDigits; i++) {
    double_native_uint_t aDigit = a.digit(i);
    carry = 0;
    for (uint16_t j=0; j<b.m_numberOfDigits; j++) {
      double_native_uint_t bDigit = b.digit(j);
      /* The fact that aDigit and bDigit are double_native is very important,
       * otherwise the product might end up being computed on single_native size
       * and then zero-padded. */
      double_native_uint_t p = aDigit*bDigit + carry + (double_native_uint_t)(digits[i+j]); // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      digits[i+j] = l[0];
      carry = l[1];
    }
    digits[i+b.m_numberOfDigits] += carry;
  }

  while (digits[productSize-1] == 0 && productSize>1) {
    productSize--;
    /* At this point we could realloc m_digits to a smaller size. */
  }

  return Integer(digits, productSize, a.m_negative != b.m_negative);
}

Integer Integer::Factorial(const Integer & i) {
  Integer j = Integer(2);
  Integer result = Integer(1);
  while (!i.isLowerThan(j)) {
    result = Multiplication(j, result);
    j = Addition(j, Integer(1));
  }
  return result;
}

IntegerDivision Integer::Division(const Integer & numerator, const Integer & denominator) {
  if (!numerator.isNegative() && !denominator.isNegative()) {
    return udiv(numerator, denominator);
  }
  Integer absNumerator = numerator;
  absNumerator.setNegative(false);
  Integer absDenominator = denominator;
  absDenominator.setNegative(false);
  IntegerDivision usignedDiv = udiv(absNumerator, absDenominator);
  if (usignedDiv.remainder.isEqualTo(Integer(0))) {
    if (!numerator.isNegative() || !denominator.isNegative()) {
      usignedDiv.quotient.setNegative(true);
    }
    return usignedDiv;
  }
  if (numerator.isNegative()) {
    if (denominator.isNegative()) {
      usignedDiv.remainder.setNegative(true);
      usignedDiv.quotient = Addition(usignedDiv.quotient, Integer(1));
      usignedDiv.remainder = Integer::Subtraction(usignedDiv.remainder, denominator);
   } else {
      usignedDiv.quotient.setNegative(true);
      usignedDiv.quotient = Subtraction(usignedDiv.quotient, Integer(1));
      usignedDiv.remainder = Integer::Subtraction(denominator, usignedDiv.remainder);
    }
  } else {
    assert(denominator.isNegative());
    usignedDiv.quotient.setNegative(true);
  }
  return usignedDiv;
}

Integer Integer::Power(const Integer & i, const Integer & j) {
  // TODO: optimize with dichotomia
  assert(!j.isNegative());
  Integer index = j;
  Integer result = Integer(1);
  while (!index.isEqualTo(Integer(0))) {
    result = Multiplication(result, i);
    index = Subtraction(index, Integer(1));
  }
  return result;
}

// Private methods

  /* WARNING: This constructor takes ownership of the digits array! */
Integer::Integer(const native_uint_t * digits, uint16_t numberOfDigits, bool negative) :
    m_numberOfDigits(numberOfDigits),
    m_negative(negative)
{
  assert(digits != nullptr);
  if (numberOfDigits == 1) {
    m_digit = digits[0];
    delete[] digits;
    if (isZero()) {
      // Normalize zero
      m_negative = false;
    }
  } else {
    assert(numberOfDigits > 1);
    m_digits = digits;
  }
}

void Integer::releaseDynamicIvars() {
  if (!usesImmediateDigit()) {
    assert(m_digits != nullptr);
    delete[] m_digits;
  }
}

int8_t Integer::ucmp(const Integer & a, const Integer & b) {
  if (a.m_numberOfDigits < b.m_numberOfDigits) {
    return -1;
  } else if (a.m_numberOfDigits > b.m_numberOfDigits) {
    return 1;
  }
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

Integer Integer::usum(const Integer & a, const Integer & b, bool subtract, bool outputNegative) {
  uint16_t size = max(a.m_numberOfDigits, b.m_numberOfDigits);
  if (!subtract) {
    // Addition can overflow
    size += 1;
  }
  native_uint_t * digits = new native_uint_t [size];
  bool carry = false;
  for (uint16_t i = 0; i<size; i++) {
    native_uint_t aDigit = (i >= a.m_numberOfDigits ? 0 : a.digit(i));
    native_uint_t bDigit = (i >= b.m_numberOfDigits ? 0 : b.digit(i));
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
    // We could realloc digits to a smaller size. Probably not worth the trouble.
  }
  return Integer(digits, size, outputNegative);
}


Integer Integer::addition(const Integer & a, const Integer & b, bool inverseBNegative) {
  bool bNegative = (inverseBNegative ? !b.m_negative : b.m_negative);
  if (a.m_negative == bNegative) {
    return usum(a, b, false, a.m_negative);
  } else {
    /* The signs are different, this is in fact a subtraction
     * s = a+b = (abs(a)-abs(b) OR abs(b)-abs(a))
     * 1/abs(a)>abs(b) : s = sign*udiff(a, b)
     * 2/abs(b)>abs(a) : s = sign*udiff(b, a)
     * sign? sign of the greater! */
    if (ucmp(a, b) >= 0) {
      return usum(a, b, true, a.m_negative);
    } else {
      return usum(b, a, true, bNegative);
    }
  }
}

Integer Integer::IntegerWithHalfDigitAtIndex(half_native_uint_t halfDigit, int index) {
  assert(halfDigit != 0);
  half_native_uint_t * digits = (half_native_uint_t *)new native_uint_t [(index+1)/2];
  memset(digits, 0, (index+1)/2*sizeof(native_uint_t));
  digits[index-1] = halfDigit;
  int indexInBase32 = index%2 == 1 ? index/2+1 : index/2;
  return Integer((native_uint_t *)digits, indexInBase32, false);
}

IntegerDivision Integer::udiv(const Integer & numerator, const Integer & denominator) {
  /* Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann
   * (Algorithm 1.6) */
  assert(!denominator.isZero());
  if (numerator.isLowerThan(denominator)) {
    IntegerDivision div = {.quotient = 0, .remainder = numerator};
    return div;
  }

  Integer A = numerator;
  Integer B = denominator;
  native_int_t base = 1 << 16;
  // TODO: optimize by just swifting digit and finding 2^kB that makes B normalized
  native_int_t d = base/(native_int_t)(B.halfDigit(B.numberOfHalfDigits()-1)+1);
  A = Multiplication(Integer(d), A);
  B = Multiplication(Integer(d), B);

  int n = B.numberOfHalfDigits();
  int m = A.numberOfHalfDigits()-n;
  half_native_uint_t * qDigits = (half_native_uint_t *)new native_uint_t [m/2+1];
  memset(qDigits, 0, (m/2+1)*sizeof(native_uint_t));
  Integer betam = IntegerWithHalfDigitAtIndex(1, m+1);
  Integer betaMB = Multiplication(betam, B); // TODO: can swift all digits by m! B.swift16(mg)
  if (!A.isLowerThan(betaMB)) {
    qDigits[m] = 1;
    A = Subtraction(A, betaMB);
  }
  for (int j = m-1; j >= 0; j--) {
    native_uint_t qj2 = ((native_uint_t)A.halfDigit(n+j)*base+(native_uint_t)A.halfDigit(n+j-1))/(native_uint_t)B.halfDigit(n-1);
    half_native_uint_t baseMinus1 = (1 << 16) -1;
    qDigits[j] = qj2 < (native_uint_t)baseMinus1 ? (half_native_uint_t)qj2 : baseMinus1;
    Integer factor = qDigits[j] > 0 ? IntegerWithHalfDigitAtIndex(qDigits[j], j+1) : Integer(0);
    A = Subtraction(A, Multiplication(factor, B));
    Integer m = Multiplication(IntegerWithHalfDigitAtIndex(1, j+1), B);
    while (A.isLowerThan(Integer(0))) {
      qDigits[j] = qDigits[j]-1;
      A = Addition(A, m);
    }
  }
  int qNumberOfDigits = m+1;
  while (qDigits[qNumberOfDigits-1] == 0 && qNumberOfDigits > 1) {
    qNumberOfDigits--;
    // We could realloc digits to a smaller size. Probably not worth the trouble.
  }
  int qNumberOfDigitsInBase32 = qNumberOfDigits%2 == 1 ? qNumberOfDigits/2+1 : qNumberOfDigits/2;
  IntegerDivision div = {.quotient = Integer((native_uint_t *)qDigits, qNumberOfDigitsInBase32, false), .remainder = A};
  if (d != 1 && !div.remainder.isZero()) {
    div.remainder = udiv(div.remainder, Integer(d)).quotient;
  }
  return div;
}

template<typename T>
T Integer::approximate() const {
  if (isZero()) {
    /* This special case for 0 is needed, because the current algorithm assumes
     * that the big integer is non zero, thus puts the exponent to 126 (integer
     * area), the issue is that when the mantissa is 0, a "shadow bit" is
     * assumed to be there, thus 126 0x000000 is equal to 0.5 and not zero.
     */
    T result = m_negative ? -0.0 : 0.0;
    return result;
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

  bool sign = m_negative;

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

  T result = IEEE754<T>::buildFloat(sign, exponent, mantissa);

  /* If exponent is 255 and the float is undefined, we have exceed IEEE 754
   * representable float. */
  if (exponent == IEEE754<T>::maxExponent() && std::isnan(result)) {
    return INFINITY;
  }
  return result;
}

int Integer::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  /* If the integer is too long, this method may be too slow.
   * Experimentally, we can display at most integer whose number of digits is
   * around 25. */
  if (m_numberOfDigits > 25) {
    return strlcpy(buffer, "undef", bufferSize);
  }

  Integer base = Integer(10);
  Integer abs = *this;
  abs.setNegative(false);
  IntegerDivision d = udiv(abs, base);
  int size = 0;
  if (bufferSize == 1) {
    return 0;
  }
  if (isEqualTo(Integer(0))) {
    buffer[size++] = '0';
  } else if (isNegative()) {
    buffer[size++] = '-';
  }
  while (!(d.remainder.isEqualTo(Integer(0)) &&
        d.quotient.isEqualTo(Integer(0)))) {
    char c = char_from_digit(d.remainder.digit(0));
    if (size >= bufferSize-1) {
      return strlcpy(buffer, "undef", bufferSize);
    }
    buffer[size++] = c;
    d = Division(d.quotient, base);
  }
  buffer[size] = 0;

  // Flip the string
  for (int i=m_negative, j=size-1 ; i < j ; i++, j--) {
    char c = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = c;
  }
  return size;
}

ExpressionLayout * Integer::createLayout() const {
  char buffer[255];
  int numberOfChars = writeTextInBuffer(buffer, 255);
  return new StringLayout(buffer, numberOfChars);
}

template float Poincare::Integer::approximate<float>() const;
template double Poincare::Integer::approximate<double>() const;

}
