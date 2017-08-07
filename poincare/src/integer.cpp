#include <poincare/integer.h>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
}
#include <poincare/complex.h>
#include "layout/string_layout.h"

#define MAX(a,b) ((a)>(b)?a:b)
#define NATIVE_UINT_BIT_COUNT (8*sizeof(native_uint_t))

namespace Poincare {

uint8_t log2(native_uint_t v) {
  assert(NATIVE_UINT_BIT_COUNT < 256); // Otherwise uint8_t isn't OK
  for (uint8_t i=0; i<NATIVE_UINT_BIT_COUNT; i++) {
    if (v < ((native_uint_t)1<<i)) {
      return i;
    }
  }
  return 32;
}

static inline char char_from_digit(native_uint_t digit) {
  return '0'+digit;
}

Integer::Integer(Integer&& other) {
  // Pilfer other's data
  m_numberOfDigits = other.m_numberOfDigits;
  m_digits = other.m_digits;
  m_negative = other.m_negative;

  // Reset other
  other.m_negative = 0;
  other.m_numberOfDigits = 0;
  other.m_digits = NULL;
}

Integer::Integer(native_int_t i) {
  assert(sizeof(native_int_t) <= sizeof(native_uint_t));
  m_negative = (i<0);
  m_numberOfDigits = 1;
  m_digits = new native_uint_t[1];
  *m_digits = (native_uint_t)(i>0 ? i : -i);
}

/* Caution: string is NOT guaranteed to be NULL-terminated! */
Integer::Integer(const char * digits, bool negative) {
  m_negative = negative;

  if (digits != nullptr && digits[0] == '-') {
    m_negative = true;
    digits++;
  }

  Integer result = Integer(0);

  if (digits != nullptr) {
    Integer base = Integer(10);
    while (*digits >= '0' && *digits <= '9') {
      result = result.multiply_by(base);
      result = result.add(Integer(*digits-'0'));
      digits++;
    }
  }

  // Pilfer v's ivars
  m_numberOfDigits = result.m_numberOfDigits;
  m_digits = result.m_digits;

  // Zero-out v
  result.m_numberOfDigits = 0;
  result.m_digits = NULL;
}

Integer::~Integer() {
  if (m_digits) {
    delete[] m_digits;
  }
}

// Private methods

Integer::Integer(native_uint_t * digits, uint16_t numberOfDigits, bool negative) :
  m_digits(digits),
  m_numberOfDigits(numberOfDigits),
  m_negative(negative) {
}

int8_t Integer::ucmp(const Integer &other) const {
  if (m_numberOfDigits < other.m_numberOfDigits) {
    return -1;
  } else if (other.m_numberOfDigits < m_numberOfDigits) {
    return 1;
  }
  for (uint16_t i = 0; i < m_numberOfDigits; i++) {
    // Digits are stored most-significant last
    native_uint_t digit = m_digits[m_numberOfDigits-i-1];
    native_uint_t otherDigit = other.m_digits[m_numberOfDigits-i-1];
    if (digit < otherDigit) {
      return -1;
    } else if (otherDigit < digit) {
      return 1;
    }
  }
  return 0;
}

static inline int8_t sign(bool negative) {
  return 1 - 2*(int8_t)negative;
}

bool Integer::operator<(const Integer &other) const {
  if (m_negative != other.m_negative) {
    return m_negative;
  }
  return (sign(m_negative)*ucmp(other) < 0);
}

bool Integer::operator==(const Integer &other) const {
  if (m_negative != other.m_negative) {
    return false;
  }
  return (ucmp(other) == 0);
}

Integer& Integer::operator=(Integer&& other) {
  if (this != &other) {
    // Release our ivars
    m_negative = 0;
    m_numberOfDigits = 0;
    delete[] m_digits;

    // Pilfer other's ivars
    m_numberOfDigits = other.m_numberOfDigits;
    m_digits = other.m_digits;
    m_negative = other.m_negative;
    // Reset other
    other.m_negative = 0;
    other.m_numberOfDigits = 0;
    other.m_digits = NULL;
  }
  return *this;
}

Integer Integer::add(const Integer &other, bool inverse_other_negative) const {
  bool other_negative = (inverse_other_negative ? !other.m_negative : other.m_negative);
  if (m_negative == other_negative) {
    return usum(other, false, m_negative);
  } else {
    /* The signs are different, this is in fact a subtraction
     * s = this+other = (abs(this)-abs(other) OR abs(other)-abs(this))
     * 1/abs(this)>abs(other) : s = sign*udiff(this, other)
     * 2/abs(other)>abs(this) : s = sign*udiff(other, this)
     * sign? sign of the greater! */
    if (ucmp(other) >= 0) {
      return usum(other, true, m_negative);
    } else {
      return other.usum(*this, true, other_negative);
    }
  }
}

Integer Integer::add(const Integer &other) const {
  return add(other, false);
}

Integer Integer::subtract(const Integer &other) const {
  return add(other, true);
}

Integer Integer::usum(const Integer &other, bool subtract, bool output_negative) const {
  uint16_t size = MAX(m_numberOfDigits, other.m_numberOfDigits);
  if (!subtract) {
    // Addition can overflow
    size += 1;
  }
  native_uint_t * digits = new native_uint_t [size];
  bool carry = false;
  for (uint16_t i = 0; i<size; i++) {
    native_uint_t a = (i >= m_numberOfDigits ? 0 : m_digits[i]);
    native_uint_t b = (i >= other.m_numberOfDigits ? 0 : other.m_digits[i]);
    native_uint_t result = (subtract ? a - b - carry : a + b + carry);
    digits[i] = result;
    carry = (subtract ? (a<result) : ((a>result)||(b>result))); // There's been an underflow or overflow
  }
  while (digits[size-1] == 0 && size>1) {
    size--;
    // We could realloc digits to a smaller size. Probably not worth the trouble.
  }
  return Integer(digits, size, output_negative);
}

Integer Integer::multiply_by(const Integer &other) const {
  assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t));
  uint16_t productSize = other.m_numberOfDigits + m_numberOfDigits;
  native_uint_t * digits = new native_uint_t [productSize];
  memset(digits, 0, productSize*sizeof(native_uint_t));

  double_native_uint_t carry = 0;
  for (uint16_t i=0; i<m_numberOfDigits; i++) {
    double_native_uint_t a = m_digits[i];
    carry = 0;
    for (uint16_t j=0; j<other.m_numberOfDigits; j++) {
      double_native_uint_t b = other.m_digits[j];
      /* The fact that a and b are double_native is very important, otherwise
       * the product might end up being computed on single_native size and
       * then zero-padded. */
      double_native_uint_t p = a*b + carry + (double_native_uint_t)(digits[i+j]); // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      digits[i+j] = l[0];
      carry = l[1];
    }
    digits[i+other.m_numberOfDigits] += carry;
  }

  while (digits[productSize-1] == 0 && productSize>1) {
    productSize--;
    /* At this point we could realloc m_digits to a smaller size. */
  }

  return Integer(digits, productSize, m_negative != other.m_negative);
}

Division::Division(const Integer &numerator, const Integer &denominator) :
m_quotient(Integer((native_int_t)0)),
m_remainder(Integer((native_int_t)0)) {
  // FIXME: First, test if denominator is zero.

  if (numerator < denominator) {
    m_quotient = Integer((native_int_t)0);
    m_remainder = numerator.add(Integer((native_int_t)0));
    // FIXME: This is a ugly way to bypass creating a copy constructor!
    return;
  }

  // Recursive case
  *this = Division(numerator, denominator.add(denominator));
  m_quotient = m_quotient.add(m_quotient);
  if (!(m_remainder < denominator)) {
    m_remainder = m_remainder.subtract(denominator);
    m_quotient = m_quotient.add(Integer(1));
  }
}

Integer Integer::divide_by(const Integer &other) const {
  return Division(*this, other).m_quotient;
}

Expression * Integer::clone() const {
  Integer * clone = new Integer((native_int_t)0);
  clone->m_numberOfDigits = m_numberOfDigits;
  clone->m_negative = m_negative;
  delete[] clone->m_digits;
  clone->m_digits = new native_uint_t [m_numberOfDigits];
  for (unsigned int i=0;i<m_numberOfDigits; i++) {
    clone->m_digits[i] = m_digits[i];
  }
  return clone;
}

Evaluation * Integer::privateEvaluate(Context& context, AngleUnit angleUnit) const {
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

  native_uint_t lastDigit = m_digits[m_numberOfDigits-1];
  uint8_t numberOfBitsInLastDigit = log2(lastDigit);

  bool sign = m_negative;

  uint8_t exponent = 126;
  /* if the exponent is bigger then 255, it cannot be stored as a uint8. Also,
   * the integer whose 2-exponent is bigger than 255 cannot be stored as a
   * float (IEEE 754 floating point). The approximation is thus INFINITY. */
  if ((int)exponent + (m_numberOfDigits-1)*32 +numberOfBitsInLastDigit> 255) {
    return new Complex(Complex::Float(INFINITY));
  }
  exponent += (m_numberOfDigits-1)*32;
  exponent += numberOfBitsInLastDigit;

  uint32_t mantissa = 0;
  mantissa |= (lastDigit << (32-numberOfBitsInLastDigit));
  if (m_numberOfDigits >= 2) {
    native_uint_t beforeLastDigit = m_digits[m_numberOfDigits-2];
    mantissa |= (beforeLastDigit >> numberOfBitsInLastDigit);
  }

  if ((m_numberOfDigits==1) && (m_digits[0]==0)) {
    /* This special case for 0 is needed, because the current algorithm assumes
     * that the big integer is non zero, thus puts the exponent to 126 (integer
     * area), the issue is that when the mantissa is 0, a "shadow bit" is
     * assumed to be there, thus 126 0x000000 is equal to 0.5 and not zero.
     */
    float result = m_negative ? -0.0f : 0.0f;
    return new Complex(Complex::Float(result));
  }

  uint_result = 0;
  uint_result |= (sign << 31);
  uint_result |= (exponent << 23);
  uint_result |= (mantissa >> (32-23-1) & 0x7FFFFF);

  /* If exponent is 255 and the float is undefined, we have exceed IEEE 754
   * representable float. */
  if (exponent == 255 && isnan(float_result)) {
    return new Complex(Complex::Float(INFINITY));
  }

  return new Complex(Complex::Float(float_result));
}

Expression::Type Integer::type() const {
  return Type::Integer;
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
  Division d = Division(*this, base);
  int size = 0;
  if (*this == Integer((native_int_t)0)) {
    buffer[size++] = '0';
  }
  while (!(d.m_remainder == Integer((native_int_t)0) &&
        d.m_quotient == Integer((native_int_t)0))) {
    assert(size<255); //TODO: malloc an extra buffer
    char c = char_from_digit(d.m_remainder.m_digits[0]);
    buffer[size++] = c;
    d = Division(d.m_quotient, base);
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

bool Integer::valueEquals(const Expression * e) const {
  assert(e->type() == Type::Integer);
  return (*this == *(Integer *)e); // FIXME: Remove operator overloading
}

}
