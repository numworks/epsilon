#include <poincare/integer.h>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
}
#include <poincare/complex.h>
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
  m_negative = negative;
#if 0
  // Pilfer v's ivars
  m_numberOfDigits = result.m_numberOfDigits;
  if (result.usesImmediateDigit()) {
    m_digit = result.m_digit;
  } else {
    m_digits = result.m_digits;
  }

  // Zero-out v
  result.m_numberOfDigits = 0;
  if (result.usesImmediateDigit()) {
    result.m_digit = 0;
  } else {
    result.m_digits = NULL;
  }
#endif
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
  Integer zero = Integer(0);
  Integer base = Integer(10);
  Integer numerator = Integer(integralPart, negative);
  for (int i = 0; i < fractionalPartLength; i++) {
    numerator = Multiplication(numerator, base);
    numerator = Addition(numerator, Integer(*fractionalPart-'0'));
    fractionalPart++;
  }
  if (exponent->isNegative()) {
    while (exponent->compareTo(&zero) != 0) {
      numerator = Multiplication(numerator, base);
      *exponent = Addition(*exponent, Integer(1));
    }
  }
  return numerator;
}

Integer Integer::denominator(Integer * exponent) {
  Integer zero = Integer(0);
  Integer base = Integer(10);
  Integer denominator = Integer(1);
  if (!exponent->isNegative()) {
    while (exponent->compareTo(&zero) != 0) {
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

// Expression subclassing

Expression::Type Integer::type() const {
  return Type::Integer;
}

void Integer::setNegative(bool negative) {
  m_negative = negative;
}

Expression * Integer::clone() const {
  return new Integer(*this);
}

// Comparison

int Integer::compareTo(const Expression * e) const {
  int typeComparison = Expression::compareTo(e);
  if (typeComparison != 0) {
    return typeComparison;
  }
  assert(e->type() == Expression::Type::Integer);
  const Integer * other = static_cast<const Integer *>(e);

  if (m_negative && !other->m_negative) {
    return -1;
  }
  if (!m_negative && other->m_negative) {
    return 1;
  }
  return sign(m_negative)*ucmp(*this, *other);
}

bool Integer::isEqualTo(const Integer & other) const {
  return (compareTo(&other) == 0);
}

bool Integer::isLowerThan(const Integer & other) const {
  return (compareTo(&other) < 0);
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
    carry = (subtract ? (aDigit<result) : ((aDigit>result)||(bDigit>result))); // There's been an underflow or overflow
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

IntegerDivision Integer::udiv(const Integer & numerator, const Integer & denominator) {
  assert(!numerator.isNegative() && !denominator.isNegative());
  // FIXME: First, test if denominator is zero.
  if (numerator.isLowerThan(denominator)) {
    IntegerDivision div = {.quotient = 0, .remainder = numerator};
    return div;
  }

  // Recursive case
  IntegerDivision div = Division(numerator, Integer::Addition(denominator, denominator));
  div.quotient = Integer::Addition(div.quotient, div.quotient);
  if (!(div.remainder.isLowerThan(denominator))) {
    div.remainder = Integer::Subtraction(div.remainder, denominator);
    div.quotient = Integer::Addition(div.quotient, Integer(1));
  }
  return div;
}

int Integer::identifier() const {
  assert(m_numberOfDigits > 0);
  int sign = m_negative ? -1 : 1;
  return sign*m_digit;
}

/*
Integer Integer::divide_by(const Integer &other) const {
  return Division(*this, other).quotient;
}


Multiplication Integer::primeFactorization() const {
  Integer result = this;
  Integer i = 2;
  while (i<result) {
    Int occurrencesOfI = 0;
    while (i divides result) {
      result = remainder;
      occurrencesOfI += 1;
    }
    if (occurrencesOfI > 0) {
      set(i, occurrencesOfI);
    }
    i++;
  }
}

*/

Evaluation<float> * Integer::privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const {
  union {
    uint32_t uint_result;
    float float_result;
  };
  assert(sizeof(float) == 4);
  /* We're generating an IEEE 754 compliant float.
  * Theses numbers are 32-bit values, stored as follow:
  * sign (1 bit)
  * exponent (8 bits)
  * mantissa (23 bits)
  *
  * We can tell that:
  * - the sign is going to be 0 for now, we only handle positive numbers
  * - the exponent is the length of our BigInt, in bits - 1 + 127;
  * - the mantissa is the beginning of our BigInt, discarding the first bit
  */

  native_uint_t lastDigit = digit(m_numberOfDigits-1);
  uint8_t numberOfBitsInLastDigit = log2(lastDigit);

  bool sign = m_negative;

  uint8_t exponent = 126;
  /* if the exponent is bigger then 255, it cannot be stored as a uint8. Also,
   * the integer whose 2-exponent is bigger than 255 cannot be stored as a
   * float (IEEE 754 floating point). The approximation is thus INFINITY. */
  if ((int)exponent + (m_numberOfDigits-1)*32 +numberOfBitsInLastDigit> 255) {
    return new Complex<float>(Complex<float>::Float(INFINITY));
  }
  exponent += (m_numberOfDigits-1)*32;
  exponent += numberOfBitsInLastDigit;

  uint32_t mantissa = 0;
  mantissa |= (lastDigit << (32-numberOfBitsInLastDigit));
  if (m_numberOfDigits >= 2) {
    native_uint_t beforeLastDigit = digit(m_numberOfDigits-2);
    mantissa |= (beforeLastDigit >> numberOfBitsInLastDigit);
}

  if ((m_numberOfDigits==1) && (digit(0)==0)) {
    /* This special case for 0 is needed, because the current algorithm assumes
     * that the big integer is non zero, thus puts the exponent to 126 (integer
     * area), the issue is that when the mantissa is 0, a "shadow bit" is
     * assumed to be there, thus 126 0x000000 is equal to 0.5 and not zero.
     */
    float result = m_negative ? -0.0f : 0.0f;
    return new Complex<float>(Complex<float>::Float(result));
  }

  uint_result = 0;
  uint_result |= (sign << 31);
  uint_result |= (exponent << 23);
  uint_result |= (mantissa >> (32-23-1) & 0x7FFFFF);

  /* If exponent is 255 and the float is undefined, we have exceed IEEE 754
   * representable float. */
  if (exponent == 255 && isnan(float_result)) {
    return new Complex<float>(Complex<float>::Float(INFINITY));
  }

  return new Complex<float>(Complex<float>::Float(float_result));
}

Evaluation<double> * Integer::privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const {
  union {
    uint64_t uint_result;
    double double_result;
  };
  assert(sizeof(double) == 8);
  /* We're generating an IEEE 754 compliant double.
  * Theses numbers are 64-bit values, stored as follow:
  * sign (1 bit)
  * exponent (11 bits)
  * mantissa (52 bits)
  *
  * We can tell that:
  * - the exponent is the length of our BigInt, in bits - 1 + 1023;
  * - the mantissa is the beginning of our BigInt, discarding the first bit
  */
  native_uint_t lastDigit = digit(m_numberOfDigits-1);
  uint8_t numberOfBitsInLastDigit = log2(lastDigit);

  bool sign = m_negative;

  uint16_t exponent = 1022;
  /* if the exponent is bigger then 2047, it cannot be stored as a uint11. Also,
   * the integer whose 2-exponent is bigger than 2047 cannot be stored as a
   * double (IEEE 754 double point). The approximation is thus INFINITY. */
  if ((int)exponent + (m_numberOfDigits-1)*32 +numberOfBitsInLastDigit> 2047) {
    return new Complex<double>(Complex<double>::Float(INFINITY));
  }
  exponent += (m_numberOfDigits-1)*32;
  exponent += numberOfBitsInLastDigit;

  uint64_t mantissa = 0;
  mantissa |= ((uint64_t)lastDigit << (64-numberOfBitsInLastDigit));
  int digitIndex = 2;
  int numberOfBits = log2(lastDigit);
  while (m_numberOfDigits >= digitIndex) {
    lastDigit = digit(m_numberOfDigits-digitIndex);
    numberOfBits += 32;
    if (64 > numberOfBits) {
      mantissa |= ((uint64_t)lastDigit << (64-numberOfBits));
    } else {
      mantissa |= ((uint64_t)lastDigit >> (numberOfBits-64));
    }
    digitIndex++;
  }

  if ((m_numberOfDigits==1) && (digit(0)==0)) {
    /* This special case for 0 is needed, because the current algorithm assumes
     * that the big integer is non zero, thus puts the exponent to 126 (integer
     * area), the issue is that when the mantissa is 0, a "shadow bit" is
     * assumed to be there, thus 126 0x000000 is equal to 0.5 and not zero.
     */
    float result = m_negative ? -0.0f : 0.0f;
    return new Complex<double>(Complex<double>::Float(result));
  }

  uint_result = 0;
  uint_result |= ((uint64_t)sign << 63);
  uint_result |= ((uint64_t)exponent << 52);
  uint_result |= ((uint64_t)mantissa >> (64-52-1) & 0xFFFFFFFFFFFFF);

  /* If exponent is 2047 and the double is undefined, we have exceed IEEE 754
   * representable double. */
  if (exponent == 2047 && isnan(double_result)) {
    return new Complex<double>(Complex<double>::Float(INFINITY));
  }
  return new Complex<double>(Complex<double>::Float(double_result));
}

ExpressionLayout * Integer::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  /* If the integer is too long, this method may overflow the stack.
   * Experimentally, we can display at most integer whose number of digits is
   * around 7. However, to avoid crashing when the stack is already half full,
   * we decide not to display integers whose number of digits > 5. */
  if (m_numberOfDigits > 5) {
    return new StringLayout("inf", 3);
  }

  char buffer[255];

  Integer base = Integer(10);
  IntegerDivision d = Division(*this, base);
  int size = 0;
  if (isEqualTo(Integer(0))) {
    buffer[size++] = '0';
  }
  while (!(d.remainder.isEqualTo(Integer(0)) &&
        d.quotient.isEqualTo(Integer(0)))) {
    assert(size<255); //TODO: malloc an extra buffer
    char c = char_from_digit(d.remainder.digit(0));
    buffer[size++] = c;
    d = Division(d.quotient, base);
  }
  buffer[size] = 0;

  // Flip the string
  for (int i=0, j=size-1 ; i < j ; i++, j--) {
    char c = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = c;
  }

  return new StringLayout(buffer, size);
}

}
