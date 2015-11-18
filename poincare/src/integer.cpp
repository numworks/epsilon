#include <poincare/integer.h>
#include <kandinsky/text.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "layout/string_layout.h"

#define MAX(a,b) ((a)>(b)?a:b)
#define NATIVE_UINT_BIT_COUNT (8*sizeof(native_uint_t))

/*
const Integer Integer::Zero = {
  m_numberOfDigits = 0,
  m_digits = nullptr
};
*/

uint8_t log2(native_uint_t v) {
  assert(NATIVE_UINT_BIT_COUNT < 256); // Otherwise uint8_t isn't OK
  for (uint8_t i=0; i<NATIVE_UINT_BIT_COUNT; i++) {
    if (v < ((native_uint_t)1<<i)) {
      return i;
    }
  }
  return 32;
}

static inline native_uint_t digit_from_char(char c) {
  return (c > '9' ? (c-'A'+10) : (c-'0'));
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
  m_digits = (native_uint_t *)malloc(sizeof(native_uint_t));
  *m_digits = (native_uint_t)(i>0 ? i : -i);
}

Integer::Integer(const char * string) {
  int stringLength = strlen(string);

  Integer base = Integer(10);

  m_negative = false;

  if (stringLength > 1 && string[0] == '-') {
    m_negative = true;
    string += 1;
    stringLength -= 1;
  }
  if (stringLength > 2 && string[0] == '0') {
    switch (string[1]) {
      case 'x':
        base = Integer(16);
        string += 2;
        stringLength -= 2;
        break;
      case 'b':
        base = Integer(2);
        string += 2;
        stringLength -= 2;
        break;
    }
  }

  Integer v = Integer(digit_from_char(string[0]));
  for (int i=1; i<stringLength; i++) {
    v = v * base;
    v = v + Integer(digit_from_char(string[i])); // ASCII encoding
  }

  // Pilfer v's ivars
  m_numberOfDigits = v.m_numberOfDigits;
  m_digits = v.m_digits;

  // Zero-out v
  v.m_numberOfDigits = 0;
  v.m_digits = NULL;
}

Integer::~Integer() {
  if (m_digits) {
    free(m_digits);
  }
}

// Private methods

Integer::Integer(native_uint_t * digits, uint16_t numberOfDigits, bool negative) :
  m_numberOfDigits(numberOfDigits),
  m_digits(digits),
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
    free(m_digits);

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
    /* The signs are different, this is in fact a substraction
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

Integer Integer::operator+(const Integer &other) const {
  return add(other, false);
}

Integer Integer::operator-(const Integer &other) const {
  return add(other, true);
}

Integer Integer::usum(const Integer &other, bool subtract, bool output_negative) const {
  uint16_t size = MAX(m_numberOfDigits, other.m_numberOfDigits);
  if (!subtract) {
    // Addition can overflow
    size += 1;
  }
  native_uint_t * digits = (native_uint_t *)malloc(size*sizeof(native_uint_t));
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

Integer Integer::operator*(const Integer &other) const {
  assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t));
  uint16_t productSize = other.m_numberOfDigits + m_numberOfDigits;
  native_uint_t * digits = (native_uint_t *)malloc(productSize*sizeof(native_uint_t));
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
    m_remainder = numerator + Integer((native_int_t)0);
    // FIXME: This is a ugly way to bypass creating a copy constructor!
    return;
  }

  // Recursive case
  *this = Division(numerator, denominator+denominator);
  m_quotient = m_quotient + m_quotient;
  if (!(m_remainder < denominator)) {
    m_remainder = m_remainder - denominator;
    m_quotient = m_quotient + Integer(1);
  }
}


Integer Integer::operator/(const Integer &other) const {
  return Division(*this, other).m_quotient;
}

float Integer::approximate(Context& context) {
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
  exponent += (m_numberOfDigits-1)*32;
  exponent += numberOfBitsInLastDigit;

  uint32_t mantissa = 0;
  mantissa |= (lastDigit << (32-numberOfBitsInLastDigit));
  if (m_numberOfDigits >= 2) {
    native_uint_t beforeLastDigit = m_digits[m_numberOfDigits-2];
    mantissa |= (beforeLastDigit >> numberOfBitsInLastDigit);
  }

  uint_result = 0;
  uint_result |= (sign << 31);
  uint_result |= (exponent << 23);
  uint_result |= (mantissa >> (32-23-1) & 0x7FFFFF);

  return float_result;
}

ExpressionLayout * Integer::createLayout(ExpressionLayout * parent) {
  char buffer[255];

  Integer base = Integer(10);
  Division d = Division(*this, base);
  int size = 0;
  while (!(d.m_remainder == Integer((native_int_t)0))) {
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

  return new StringLayout(parent, buffer, size);
}
